/*
 * ipod_audio.c - audio driver for iPod
 *
 * Copyright (c) 2003,2004 Bernard Leach <leachbj@bouncycastle.org>
 */

#include <linux/module.h>
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/major.h>
#include <linux/delay.h>
#include <linux/soundcard.h>
#include <linux/sound.h>
#include <linux/devfs_fs_kernel.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/arch/irqs.h>
#include <asm/arch/hardware.h>
#include "pp5020.h"
#include "as3514.h"

extern int i2c_readbyte(unsigned int dev_addr, int addr);
void i2s_reset(void);

/* length of shared buffer in half-words (starting at DMA_BASE) */
#define BUF_LEN		(40*1024)

static int ipodaudio_isopen;
// static int ipodaudio_power_state;
static devfs_handle_t dsp_devfs_handle, mixer_devfs_handle;
static int ipod_sample_rate = 44100;
static volatile int *ipodaudio_stereo = (int *)DMA_STEREO;
static int ipod_mic_boost = 100;
static int ipod_line_level = 0x17;
static int ipod_pcm_level = 60;	
static int ipod_active_rec = SOUND_MASK_MIC;


#define ARRAYLEN(a) (sizeof(a)/sizeof((a)[0]))


/* Shadow registers */
struct as3514_info
{
   int          vol_r;       /* Cached volume level (R) */
   int          vol_l;       /* Cached volume level (L) */
   unsigned int regs[0x1e];  /* last audio register: PLLMODE 0x1d */
} as3514;

enum
{
   SOURCE_DAC = 0,
      SOURCE_MIC1,
      SOURCE_LINE_IN1,
      SOURCE_LINE_IN1_ANALOG
};

static unsigned int source = SOURCE_DAC;


static void as3514_write(unsigned int reg, unsigned int value)
{
   unsigned char data[2];
   
   data[0] = reg;
   data[1] = value;
 
   ipod_i2c_lock();
   if(ipod_i2c_send_bytes(AS3514_I2C_ADDR, 2, data) != 0)
   {
//      DEBUGF("as3514 error reg=0x%02x", reg);
   }
   ipod_i2c_unlock();
  
   if (reg < ARRAYLEN(as3514.regs))
   {
      as3514.regs[reg] = value;
   }
   else
   {
//      DEBUGF("as3514 error reg=0x%02x", reg);
   }
}

/* Helpers to set/clear bits */
static void as3514_write_or(unsigned int reg, unsigned int bits)
{
   as3514_write(reg, as3514.regs[reg] | bits);
}

static void as3514_write_and(unsigned int reg, unsigned int bits)
{
   as3514_write(reg, as3514.regs[reg] & bits);
}

/* convert tenth of dB volume to master volume register value */
int as3514_tenthdb2master(int db)
{
   /* +6 to -73.5dB in 1.5dB steps == 53 levels */
   if (db < VOLUME_MIN) {
      return 0x0;
   } else if (db >= VOLUME_MAX) {
      return 0x35;
   } else {
      return((db-VOLUME_MIN)/15); /* VOLUME_MIN is negative */
   }
}

void as3514_mute(bool mute)
{
   if (mute) {
      as3514_write_or(HPH_OUT_L, (1 << 7));
   } else {
      as3514_write_and(HPH_OUT_L, ~(1 << 7));
   }
}

void as3514_enable_output(bool enable)
{
   if (enable) {
      /* reset the I2S controller into known state */
      i2s_reset();
      
      as3514_write_or(HPH_OUT_L, (1 << 6)); /* power on */
      as3514_mute(0);
   } else {
      as3514_mute(1);
      as3514_write_and(HPH_OUT_L, ~(1 << 6)); /* power off */
   }
}

int as3514_set_master_vol(int vol_l, int vol_r)
{
    unsigned int hph_r = as3514.regs[HPH_OUT_R] & ~0x1f;
    unsigned int hph_l = as3514.regs[HPH_OUT_L] & ~0x1f;
    unsigned int mix_l, mix_r;
    unsigned int mix_reg_r, mix_reg_l;

    /* keep track of current setting */
    as3514.vol_l = vol_l;
    as3514.vol_r = vol_r;

    if (source == SOURCE_LINE_IN1_ANALOG) {
        mix_reg_r = LINE_IN1_R;
        mix_reg_l = LINE_IN1_L;
    } else {
        mix_reg_r = DAC_R;
        mix_reg_l = DAC_L;
    }

    mix_r = as3514.regs[mix_reg_r] & ~0x1f;    
    mix_l = as3514.regs[mix_reg_l] & ~0x1f;    

    /* we combine the mixer channel volume range with the headphone volume
       range */
    if (vol_r <= 0x16) {
        mix_r |= vol_r;
        /* hph_r - set 0 */
    } else {
        mix_r |= 0x16;
        hph_r += vol_r - 0x16;
    }

    if (vol_l <= 0x16) {
        mix_l |= vol_l;
        /* hph_l - set 0 */
    } else {
        mix_l |= 0x16;
        hph_l += vol_l - 0x16;
    }

    as3514_write(mix_reg_r, mix_r);
    as3514_write(mix_reg_l, mix_l);
    as3514_write(HPH_OUT_R, hph_r);
    as3514_write(HPH_OUT_L, hph_l);

    return 0;
}

int as3514_set_lineout_vol(int vol_l, int vol_r)
{
    as3514_write(LINE_OUT_R, vol_r);
    as3514_write(LINE_OUT_L, (1 << 6) | vol_l);

    return 0;
}

void as3514_close(void)
{
    /* mute headphones */
    as3514_mute(true);

    /* turn off everything */
    as3514_write(AUDIOSET1, 0x0);
}

void as3514_set_sample_rate(int sampling_control)
{
    (void)sampling_control;
}

void as3514_enable_recording(bool source_mic)
{
    if (source_mic) {
        source = SOURCE_MIC1;

        /* Sync mixer volumes before switching inputs */
        as3514_set_master_vol(as3514.vol_l, as3514.vol_r);

        /* ADCmux = Stereo Microphone */
        as3514_write_and(ADC_R, ~(0x3 << 6));
        /* MIC1_on, LIN1_off */
        as3514_write(AUDIOSET1,
            (as3514.regs[AUDIOSET1] & ~(1 << 2)) | (1 << 0));
        /* M1_AGC_off */
        as3514_write_and(MIC1_R, ~(1 << 7));
    } else {
        source = SOURCE_LINE_IN1;

        as3514_set_master_vol(as3514.vol_l, as3514.vol_r);

        /* ADCmux = Line_IN1 */
        as3514_write(ADC_R,
            (as3514.regs[ADC_R] & ~(0x3 << 6)) | (0x1 << 6));
        /* MIC1_off, LIN1_on */
        as3514_write(AUDIOSET1,
            (as3514.regs[AUDIOSET1] & ~(1 << 0)) | (1 << 2));
    }

    /* ADC_Mute_off */
    as3514_write_or(ADC_L, (1 << 6));
    /* ADC_on */
    as3514_write_or(AUDIOSET1, (1 << 7));
}

void as3514_disable_recording(void)
{
    source = SOURCE_DAC;

    /* ADC_Mute_on */
    as3514_write_and(ADC_L, ~(1 << 6));

    /* ADC_off, LIN1_off, MIC_off */
    as3514_write_and(AUDIOSET1, ~((1 << 7) | (1 << 2) | (1 << 0)));

    as3514_set_master_vol(as3514.vol_l, as3514.vol_r);
}

/**
 * Set recording volume
 *
 * Line in   : 0 .. 23 .. 31 =>
               Volume -34.5 .. +00.0 .. +12.0 dB
 * Mic (left): 0 .. 23 .. 39 =>
 *             Volume -34.5 .. +00.0 .. +24.0 dB
 *
 */
void as3514_set_recvol(int left, int right, int mic)
{
    if(mic)
    {
        /* Combine MIC gains seamlessly with ADC levels */
        unsigned int mic1_r = as3514.regs[MIC1_R] & ~(0x3 << 5);

        if (left >= 36) {
            /* M1_Gain = +40db, ADR_Vol = +7.5dB .. +12.0 dB =>
               +19.5 dB .. +24.0 dB */
            left -= 8;
            mic1_r |= (0x2 << 5);
        } else if (left >= 32) {
            /* M1_Gain = +34db, ADR_Vol = +7.5dB .. +12.0 dB =>
               +13.5 dB .. +18.0 dB */
            left -= 4; 
            mic1_r |= (0x1 << 5);
        }
            /* M1_Gain = +28db, ADR_Vol = -34.5dB .. +12.0 dB =>
               -34.5 dB .. +12.0 dB */

        right = left;

        as3514_write(MIC1_R, mic1_r);
    }
    as3514_write(ADC_R, (as3514.regs[ADC_R] & ~0x1f) | right);
    as3514_write(ADC_L, (as3514.regs[ADC_L] & ~0x1f) | left);
}

/**
 * Enable line in 1 analog monitoring
 *
 */
void as3514_set_monitor(bool enable)
{
    /* LI1R_Mute_on - default */
    unsigned int line_in1_r = as3514.regs[LINE_IN1_R] & ~(1 << 5);
    /* LI1L_Mute_on - default */
    unsigned int line_in1_l = as3514.regs[LINE_IN1_L] & ~(1 << 5);
    /* LIN1_off - default */
    unsigned int audioset1 = as3514.regs[AUDIOSET1] & ~(1 << 2);

    if (enable) {
        source = SOURCE_LINE_IN1_ANALOG;

        /* LI1R_Mute_off */
        line_in1_r |= (1 << 5);
        /* LI1L_Mute_off */
        line_in1_l |= (1 << 5);
        /* LIN1_on */
        audioset1 |= (1 << 2);
    }

    as3514_write(AUDIOSET1, audioset1);
    as3514_write(LINE_IN1_R, line_in1_r);
    as3514_write(LINE_IN1_L, line_in1_l);

    /* Sync mixer volume */
    as3514_set_master_vol(as3514.vol_l, as3514.vol_r);
}

static void as3514_init_pb(void)
{
   /* Set ADC off, mixer on, DAC on, line out off, line in off, mic off */
   
   /* Turn on SUM, DAC */
   as3514_write(AUDIOSET1, (1 << 6) | (1 << 5));
   /* Power on */
   as3514_enable_output(true);
   /* Unmute the master channel (DAC should be at zero point now). */
   as3514_mute(false);
   as3514_set_master_vol(as3514.vol_l, as3514.vol_r);
}

static int codec_set_sample_rate(int rate)
{
//	ipod_sample_rate = rate;
	return ipod_sample_rate;
}

static void codec_init_pb(void)
{
	as3514_init_pb();
}

static void codec_activate_mic(void)
{
	as3514_enable_recording(true);
}

static void codec_activate_linein(void)
{
   as3514_enable_recording(false);
}

static void codec_deinit(void)
{
	as3514_close();
}

#if 0
#define GPIOG_OUTPUT_VAL (*(volatile unsigned long *)(0x6000d0a8))
#define GPIO_CLEAR_BITWISE(port, mask) do { *(&port + (0x800/sizeof(long))) = mask << 8; } while(0)
#define GPIO_SET_BITWISE(port, mask) do { *(&port + (0x800/sizeof(long))) = (mask << 8) | mask; } while(0)

static bool btnOn = true;
#endif
static void i2s_pp5020_pb_dma(void)
{
	volatile int *r_off = (int *)DMA_READ_OFF;
	volatile int *w_off = (int *)DMA_WRITE_OFF;
	volatile int *dma_active = (int *)DMA_ACTIVE;
	volatile unsigned short *dma_buf = (unsigned short *)DMA_BASE;
	int stereo = *ipodaudio_stereo;
	int free_count;

	outl(inl(0x70002800) & ~0x2, 0x70002800);

repeat:
	while (*r_off != *w_off) {
		free_count = (inl(0x7000280c) & 0x3f0000) >> 16;

		if (free_count < 2) {
			/* enable interrupt */
			outl(inl(0x70002800) | 0x2, 0x70002800);

			return;
		}

		outl(((unsigned)dma_buf[*r_off]) << 16, 0x70002840);
		if (!stereo) {
			outl(((unsigned)dma_buf[*r_off]) << 16, 0x70002840);
		}

		/* enable playback fifo */
		outl(inl(0x70002800) | 0x20000000, 0x70002800);

		*r_off = (*r_off + 1) % BUF_LEN;
	}

	/* wait for fifo to empty */
	while ((inl(0x70002804) & 0x80000000) == 0) {
		if (*r_off != *w_off) {
			goto repeat;
		}
	}

	/* disable playback fifo */
	outl(inl(0x70002800) & ~0x20000000, 0x70002800);
   
   IIS_IRQTX_REG &= ~IIS_IRQTX;

	*dma_active = 0;

#if 0
   if(btnOn)
      GPIO_CLEAR_BITWISE(GPIOG_OUTPUT_VAL, 0x80);
   else
      GPIO_SET_BITWISE(GPIOG_OUTPUT_VAL, 0x80);
   btnOn = !btnOn;
#endif
}

static void i2s_pp5020_rec_dma(void)
{
	volatile int *r_off = (int *)DMA_READ_OFF;
	volatile int *w_off = (int *)DMA_WRITE_OFF;
	volatile int *dma_active = (int *)DMA_ACTIVE;
	volatile unsigned short *dma_buf = (unsigned short *)DMA_BASE;
	int stereo = *ipodaudio_stereo;

	outl(inl(0x70002800) & ~0x1, 0x70002800);

	while (*dma_active != 2) {
		int full_count = (inl(0x7000280c) & 0x3f000000) >> 24;
		if (full_count < 2) {
			/* enable interrupt */
			outl(inl(0x70002800) | 0x1, 0x70002800);
			return;
		}

		dma_buf[*w_off] = (unsigned short)(inl(0x70002880) >> 16);
		if (!stereo) {
			/* throw away second sample */
			inl(0x70002880);
		}

		*w_off = (*w_off + 1) % BUF_LEN;

		/* check for buffer over run */
		if (*r_off == *w_off) {
			*r_off = (*r_off + 1) % BUF_LEN;
		}
	}

	/* disable fifo */
	outl(inl(0x70002800) & ~0x10000000, 0x70002800);
   
	/* tell the cpu we are no longer active */
	*dma_active = 3;
}

static int ipodaudio_open(struct inode *inode, struct file *filep)
{
	volatile int *r_off = (int *)DMA_READ_OFF;
	volatile int *w_off = (int *)DMA_WRITE_OFF;
	volatile int *dma_active = (int *)DMA_ACTIVE;

	if (ipodaudio_isopen) {
		return -EBUSY;
	}

	/* initialise shared variables */
	*r_off = 0;
	*w_off = 0;
	*dma_active = 0;

	ipodaudio_isopen = 1;
	ipod_sample_rate = 44100;
	*ipodaudio_stereo = 0;

	/* reset the I2S controller into known state */
	i2s_reset();

	if (filep->f_mode & FMODE_WRITE) {
		codec_init_pb();

		/* setup I2S FIQ handler */
		ipod_set_process_dma(i2s_pp5020_pb_dma);

		/* setup I2S interrupt for FIQ */
		outl(inl(0x6000403c) | PP5020_I2S_MASK, 0x6000403c);
		outl(PP5020_I2S_MASK, 0x60004034);
	}

	if (filep->f_mode & FMODE_READ) {
      
		if (ipod_active_rec == SOUND_MASK_LINE) {
			codec_activate_linein();
		} else if (ipod_active_rec == SOUND_MASK_MIC) {
			codec_activate_mic();
		}
	}
	return 0;
}

static void ipodaudio_txdrain(void)
{
	volatile int *r_off = (int *)DMA_READ_OFF;
	volatile int *w_off = (int *)DMA_WRITE_OFF;
	volatile int *dma_active = (int *)DMA_ACTIVE;

	if (!*dma_active) {
		printk(KERN_ERR "dma not active\n");
		return;
	}

	while (*r_off != *w_off) {
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(2);

		if (signal_pending(current)) {
			break;
		}
	}
}

static int ipodaudio_close(struct inode *inode, struct file *filep)
{
   if (filep->f_mode & FMODE_WRITE) {
		ipodaudio_txdrain();

		/* mask the interrupt */
//		outl(PP5020_I2S_MASK, 0x60004038);
outl(inl(0x60004034) & ~PP5020_I2S_MASK, 0x60004034);
	}

	if (filep->f_mode & FMODE_READ) {
		volatile int *dma_active = (int *)DMA_ACTIVE;

		if (*dma_active) {
			/* tell COP dma to exit */
			*dma_active = 2;

			/* wait for the COP to signal its done */
			while (*dma_active != 3) {
				set_current_state(TASK_INTERRUPTIBLE);
				schedule_timeout(2);

				if (signal_pending(current)) {
					break;
				}
			}
		}
	}
	/* clear the handler */
	ipod_set_process_dma(0);

	codec_deinit();

	ipodaudio_isopen = 0;

	return 0;
}


static ssize_t ipodaudio_write(struct file *filp, const char *buf, size_t count, loff_t *ppos)
{
	unsigned short *bufsp;
	size_t rem;

	volatile int *r_off = (int *)DMA_READ_OFF;
	volatile int *w_off = (int *)DMA_WRITE_OFF;
	int write_off_current, write_off_next, read_off_current;
	volatile int *dma_active = (int *)DMA_ACTIVE;
	volatile unsigned short *dma_buf = (unsigned short *)DMA_BASE;

	if (count <= 0) {
		return 0;
	}

	bufsp = (unsigned short *)buf;
	rem = count/2;

	write_off_current = *w_off;

	while (rem > 0) {
		int cnt;

		write_off_next = (write_off_current + 1) % BUF_LEN;

		read_off_current = *r_off;

		/* buffer full? */
		if (write_off_next == read_off_current) {
			/* buffer is full */
			set_current_state(TASK_INTERRUPTIBLE);

			/* sleep a little */
			schedule_timeout(2);
		}

		if (read_off_current <= write_off_current) {
			/* room at end of buffer? */
			cnt = BUF_LEN - 1 - write_off_current;
			if (read_off_current > 0) cnt++;

			if (cnt > 0)  {
				if (cnt > rem) cnt = rem;

				memcpy((void*)&dma_buf[write_off_current], bufsp, cnt<<1);

				rem -= cnt;
				bufsp += cnt;

				write_off_current = (write_off_current + cnt) % BUF_LEN;
			}

			/* room at start of buffer (and more data)? */
			if (read_off_current > 0 && rem > 0) {
				int n;

				if (rem >= read_off_current) {
					n = read_off_current - 1;
				} else {
					n = rem;
				}

				memcpy((void*)&dma_buf[0], bufsp, n<<1);

				rem -= n;
				bufsp += n;

				write_off_current = n;
			}
		} else if (read_off_current > write_off_current) {
			cnt = read_off_current - 1 - write_off_current;
			if (cnt > rem) cnt = rem;

			memcpy((void*)&dma_buf[write_off_current], bufsp, cnt<<1);

			bufsp += cnt;
			rem -= cnt;

			write_off_current += cnt;
		}

		*w_off = write_off_current;

      if (!*dma_active) {
			*dma_active = 1;
			outl(inl(0x70002800) | 0x2, 0x70002800);
			outl(inl(0x70002800) | 0x20000000, 0x70002800);
		}
	}

	return count;
}

static ssize_t ipodaudio_read(struct file *filp, char *buf, size_t count, loff_t *ppos)
{
	unsigned short *bufsp;
	size_t rem;

	volatile int *r_off = (int *)DMA_READ_OFF;
	volatile int *w_off = (int *)DMA_WRITE_OFF;
	volatile int *dma_active = (int *)DMA_ACTIVE;
	volatile unsigned short *dma_buf = (unsigned short *)DMA_BASE;

	if (!*dma_active) {
		*dma_active = 1;

		*r_off = 0;
		*w_off = 0;

		/* setup I2S FIQ handler */
		ipod_set_process_dma(i2s_pp5020_rec_dma);

		/* setup FIQ */
		outl(inl(0x6000403c) | PP5020_I2S_MASK, 0x6000403c);
		outl(PP5020_I2S_MASK, 0x60004034);

		/* interrupt on full fifo */
		outl(inl(0x70002800) | 0x1, 0x70002800);

		/* enable record fifo */
		outl(inl(0x70002800) | 0x10000000, 0x70002800);
	}
	bufsp = (unsigned short *)buf;
	rem = count/2;

	while (rem > 0) {
		int write_pos = *w_off;
		int read_pos = *r_off;
		int len = 0;

		if (read_pos < write_pos) {
			/* read data between read pos and write pos */
			len = write_pos - read_pos;
		} else if (write_pos < read_pos) {
			/* read data to end of buffer */
			/* next loop iteration will read the rest */
			len = BUF_LEN - read_pos;
		} else {
			/* buffer is empty */
			set_current_state(TASK_INTERRUPTIBLE);

			/* sleep a little */
			schedule_timeout(2);
		}

		if (len > rem) {
			len = rem;
		}

		if (len) {
			memcpy(bufsp, (void*)&dma_buf[read_pos], len<<1);

			bufsp += len;
			rem -= len;

			/* check for buffer over run */
			if (read_pos == *r_off) {
				*r_off = (*r_off + len) % BUF_LEN;
			} else {
				printk(KERN_ERR "ADC buffer overrun\n");
			}
		}

		if (signal_pending(current)) {
			set_current_state(TASK_RUNNING);
			return count - (rem * 2);
		}
	}

	set_current_state(TASK_RUNNING);

	return count;
}

static int ipodaudio_getspace_ioctl(void *arg)
{
	audio_buf_info abinfo;
	int read_pos = *(int *)DMA_READ_OFF;
	int write_pos = *(int *)DMA_WRITE_OFF;
	int len;

	if (read_pos == write_pos) {
		len = BUF_LEN - 1;	/* ring buffer empty */
	} else if (read_pos < write_pos) {
		len = BUF_LEN - 1 - (write_pos - read_pos);
	} else {
		int next_write_pos = (write_pos + 1) % BUF_LEN;

		len = read_pos - next_write_pos;
	}

	abinfo.bytes = len * 2;
	abinfo.fragsize = BUF_LEN / 2;
	abinfo.fragments = abinfo.bytes / abinfo.fragsize;

	return copy_to_user(arg, &abinfo, sizeof(abinfo)) ? -EFAULT : 0;
}

static int ipodaudio_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	int rc = 0;
	int val = 0;

	switch (cmd) {
	case SNDCTL_DSP_SPEED:
		rc = verify_area(VERIFY_READ, (void *) arg, sizeof(val));
		if (rc == 0) {
			get_user(val, (int *) arg);

			val = codec_set_sample_rate(val);

			put_user(val, (int *) arg);
		}
		break;

	case SNDCTL_DSP_GETFMTS:
		rc = verify_area(VERIFY_READ, (void *) arg, sizeof(val));
		if (rc == 0) {
			put_user(AFMT_S16_LE, (int *) arg);
		}
		break;

	case SNDCTL_DSP_SETFMT:
	/* case SNDCTL_DSP_SAMPLESIZE: */
		rc = verify_area(VERIFY_READ, (void *) arg, sizeof(val));
		if (rc == 0) {
			get_user(val, (int *) arg);
			if (val != AFMT_S16_LE) {
				put_user(AFMT_S16_LE, (int *) arg);
			}
		}
		break;

	case SNDCTL_DSP_STEREO:
		rc = verify_area(VERIFY_READ, (void *) arg, sizeof(val));
		if (rc == 0) {
			get_user(val, (int *) arg);
			if (val != 0 && val != 1) {
				put_user(1, (int *) arg);
			} else {
				*ipodaudio_stereo = val;
			}
		}
		break;

	case SNDCTL_DSP_CHANNELS:
		rc = verify_area(VERIFY_READ, (void *) arg, sizeof(val));
		if (rc == 0) {
			get_user(val, (int *) arg);
			if (val > 2) {
				val = 2;
			}
			*ipodaudio_stereo = (val == 2);
			put_user(val, (int *) arg);
		}
		break;

	case SNDCTL_DSP_GETBLKSIZE:
		rc = verify_area(VERIFY_WRITE, (void *) arg, sizeof(long));
		if (rc == 0) {
			put_user(BUF_LEN/2, (int *) arg);
		}
		break;

	case SNDCTL_DSP_SYNC:
		rc = 0;
		ipodaudio_txdrain();
		break;

	case SNDCTL_DSP_RESET:
		rc = 0;
		break;

	case SNDCTL_DSP_GETOSPACE:
		return ipodaudio_getspace_ioctl((void *)arg);
	}

	return rc;
}

static struct file_operations ipod_dsp_fops = {
	owner: THIS_MODULE,
	llseek:	no_llseek,
	open: ipodaudio_open,
	release: ipodaudio_close,
	write: ipodaudio_write,
	read: ipodaudio_read,
	ioctl: ipodaudio_ioctl,
};

static int ipod_mixer_open(struct inode *inode, struct file *filep)
{
	return 0;
}

static int ipod_mixer_close(struct inode *inode, struct file *filep)
{
	return 0;
}

static int ipod_mixer_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	if (cmd == SOUND_MIXER_INFO) {
		mixer_info info;
		unsigned char *codec;

      codec = "AS3514";
	       
		strncpy(info.id, codec, sizeof(info.id));
		strncpy(info.name, "AS", sizeof(info.name));
		if (copy_to_user((void *) arg, &info, sizeof(info)))
			return -EFAULT;
		return 0;
	}

	if (_SIOC_DIR(cmd) == _SIOC_READ) {
		int val;

		switch (_IOC_NR(cmd)) {
		/* the devices which can be used as recording devices */
		case SOUND_MIXER_RECMASK:
			return put_user(SOUND_MASK_LINE | SOUND_MASK_MIC, (int *)arg);

		/* bit mask for each of the stereo channels */
		case SOUND_MIXER_STEREODEVS:
			return put_user(SOUND_MASK_PCM | SOUND_MASK_LINE, (int *)arg);

		/* bit mask for each of the supported channels */
		case SOUND_MIXER_DEVMASK:
			return put_user(SOUND_MASK_PCM | SOUND_MASK_LINE | SOUND_MASK_MIC, (int *)arg);

		/* bit mask which describes general capabilities of the mixer */
		case SOUND_MIXER_CAPS:
			/* only one mixer channel can be selected as a
			   recording source at any one time */
			return put_user(SOUND_CAP_EXCL_INPUT, (int *)arg);

		/* bit mask for each of the currently active recording sources */
		case SOUND_MIXER_RECSRC:
			return put_user(ipod_active_rec, (int *)arg);

		case SOUND_MIXER_PCM:		/* codec output level */
			val = ipod_pcm_level;
			val = val << 8 | val;
			return put_user(val, (int *)arg);

		case SOUND_MIXER_LINE:	/* line-in jack */
			val = ipod_line_level * 100 / 31;
			val = val << 8 | val;
			return put_user(val, (int *)arg);

		case SOUND_MIXER_MIC:		/* microphone */
			/* 0 or +20dB (mic boost) plus mute */
			return put_user(ipod_mic_boost, (int *)arg);
		}
	}
	else {
		int val, left, right;

		if (get_user(val, (int *)arg)) {
			return -EFAULT;
		}

		switch (_IOC_NR(cmd)) {
		/* select the active recording sources 0 == mic */
		case SOUND_MIXER_RECSRC:
			if (val != ipod_active_rec) {
				if (val == SOUND_MASK_LINE) {
					codec_activate_linein();
				} else if (val == SOUND_MASK_MIC) {
					codec_activate_mic();
				} else {
					val = ipod_active_rec;
				}

				ipod_active_rec = val;
			}

			return put_user(val, (int *)arg);

		case SOUND_MIXER_PCM:		/* codec output level */
			left = val & 0xff;
			right = (val >> 8) & 0xff;

			if (left > 100) left = 100;
			if (right > 100) right = 100;
         ipod_pcm_level = left;
         
         if(left > 5)
            left = ((26 * left)/100) + 23;  // max 47 (53 is too loud)
         else 
            left = 0;
         if(right > 5)
            right = ((26 * right)/100) + 23; // max 47 (53 is too loud)
         else
            right = 0;

         as3514_set_master_vol(left, right);

         return put_user(val, (int *)arg);

		case SOUND_MIXER_LINE:	/* line-in jack */
			/* +12 to -34.5dB 1.5dB steps (plus mute == 32levels) 5bits + mute */
			/* 10111 == 0dB */
			left = val & 0xff;
			right = (val >> 8) & 0xff;

			if (left > 100) left = 100;
			if (right > 100) right = 100;

         ipod_line_level = left * 31 / 100;
         as3514_set_recvol(left, right, false);

			return put_user(val, (int *)arg);

		case SOUND_MIXER_MIC:		/* microphone */
			/* 0 or +20dB (mic boost) plus mute */

         as3514_set_recvol(val, val, true);
         
			ipod_mic_boost = val;

			return put_user(val, (int *)arg);
		}
	}

	return -EINVAL;
}


static struct file_operations ipod_mixer_fops = {
	owner: THIS_MODULE,
	llseek:	no_llseek,
	open: ipod_mixer_open,
	release: ipod_mixer_close,
	ioctl: ipod_mixer_ioctl,
};

void i2s_reset(void)
{
   /* I2S soft reset */
   IISCONFIG |= IIS_RESET;
   IISCONFIG &= ~IIS_RESET;
   
   /* BIT.FORMAT */
   IISCONFIG = ((IISCONFIG & ~IIS_FORMAT_MASK) | IIS_FORMAT_IIS);
   /* BIT.SIZE */
   IISCONFIG = ((IISCONFIG & ~IIS_SIZE_MASK) | IIS_SIZE_16BIT);
   
   /* FIFO.FORMAT */
   /* If BIT.SIZE < FIFO.FORMAT low bits will be 0 */
   /* AS3514 can only operate as I2S Slave */
   IISCONFIG |= IIS_MASTER;
   /* Set I2S to 44.1kHz */
   IISCLK = (IISCLK & ~0x1ff) | 33;
   IISDIV = 7;
   
   IISCONFIG = ((IISCONFIG & ~IIS_FIFO_FORMAT_MASK) | IIS_FIFO_FORMAT_LE32);
   
   /* RX_ATN_LVL = when 12 slots full */
   /* TX_ATN_LVL = when 12 slots empty */
   IISFIFO_CFG |= IIS_RX_FULL_LVL_12 | IIS_TX_EMPTY_LVL_12;
   
   /* Rx.CLR = 1, TX.CLR = 1 */
   IISFIFO_CFG |= IIS_RXCLR | IIS_TXCLR;
}

static void __init ipodaudio_hw_init(void)
{
   int i;
	/* reset I2C */
	ipod_i2c_init();


   /* normal outputs for CDI and I2S pin groups */
   DEV_INIT2 &= ~0x300;
   
   /*mini2?*/
   DEV_INIT1 &=~0x3000000;
   /*mini2?*/
   
   /* device reset */
   DEV_RS |= DEV_I2S;
   DEV_RS &=~DEV_I2S;
   
   /* device enable */
   DEV_EN |= (DEV_I2S | 0x7);
   
   /* enable external dev clock clocks */
   DEV_EN |= 0x2;
   
   /* external dev clock to 24MHz */
   outl(inl(0x70000018) & ~0xc, 0x70000018);
   
   i2s_reset();
   
   /* Set ADC off, mixer on, DAC on, line out off, line in off, mic off */
   
   /* Turn on SUM, DAC */
   as3514_write(AUDIOSET1, (1 << 6) | (1 << 5));
   
   /* Set BIAS on, DITH on, AGC on, IBR_DAC max, LSP_LP on, IBR_LSP min */
   as3514_write(AUDIOSET2, (1 << 2) | (3 << 0));
   
   /* Set HPCM off, ZCU off*/
   as3514_write(AUDIOSET3, (1 << 2) | (1 << 0));
   
   /* Mute and disable speaker */
   as3514_write(LSP_OUT_R, 0);
   as3514_write(LSP_OUT_L, (1 << 7));
   
   /* set vol and set headphone over-current to 0 */
   as3514_write(HPH_OUT_R, (0x3 << 6) | 0x16);
   /* set default vol for headphone */
   as3514_write(HPH_OUT_L, 0x16);
   
   /* LRCK 24-48kHz */
   as3514_write(PLLMODE, 0x00);
   
   /* DAC_Mute_off */
   as3514_write_or(DAC_L, (1 << 6));
   
   /* M1_Sup_off */
   as3514_write_or(MIC1_L, (1 << 7));
   /* M2_Sup_off */
   as3514_write_or(MIC2_L, (1 << 7));
   
   /* read all reg values */
   ipod_i2c_lock();
   for (i = 0; i < ARRAYLEN(as3514.regs); i++)
   {
      as3514.regs[i] = i2c_readbyte(AS3514_I2C_ADDR, i);
   }
   ipod_i2c_unlock();
   as3514_set_master_vol(37, 37);
}

static int __init ipodaudio_init(void)
{
   printk("ipodaudio: (c) Copyright 2008 Bernard Leach <leachbj@bouncycastle.org>\n");

	dsp_devfs_handle = devfs_register(NULL, "dsp", DEVFS_FL_DEFAULT,
			SOUND_MAJOR, SND_DEV_DSP,
			S_IFCHR | S_IWUSR | S_IRUSR,
			&ipod_dsp_fops, NULL);
	if (dsp_devfs_handle < 0) {
		printk(KERN_WARNING "SOUND: failed to register major %d, minor %d\n",
			SOUND_MAJOR, SND_DEV_DSP);
		return 0;
	}
	mixer_devfs_handle = devfs_register(NULL, "mixer", DEVFS_FL_DEFAULT,
			SOUND_MAJOR, SND_DEV_CTL,
			S_IFCHR | S_IWUSR | S_IRUSR,
			&ipod_mixer_fops, NULL);
	if (mixer_devfs_handle < 0) {
		printk(KERN_WARNING "SOUND: failed to register major %d, minor %d\n",
			SOUND_MAJOR, SND_DEV_DSP);
		return 0;
	}

	ipodaudio_hw_init();
	return 0;
}

static void __exit ipodaudio_exit(void)
{
	devfs_unregister_chrdev(SOUND_MAJOR, "mixer");
	devfs_unregister(mixer_devfs_handle);

	devfs_unregister_chrdev(SOUND_MAJOR, "dsp");
	devfs_unregister(dsp_devfs_handle);
}

module_init(ipodaudio_init);
module_exit(ipodaudio_exit);

MODULE_AUTHOR("Bernard Leach <leachbj@bouncycastle.org>");
MODULE_DESCRIPTION("Audio driver for IPod");
MODULE_LICENSE("GPL");

