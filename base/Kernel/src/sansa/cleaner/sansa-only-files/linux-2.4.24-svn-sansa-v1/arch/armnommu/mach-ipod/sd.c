#include <linux/module.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/blkpg.h>
#include <linux/hdreg.h>
#include <linux/major.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#define DEVICE_NAME "sd"
#define DEVICE_NR(device) (MINOR(device))
#define DEVICE_ON(device)
#define DEVICE_OFF(device)
static int nr_major = 0;
#define MAJOR_NR nr_major
//#define MAJOR_NR 121

#include <linux/blk.h>
#include <linux/kmod.h>

#define SANSA_E200
#include "pp5020.h"


#define SD_BLOCK_SIZE      (512)
#define SD_SECTOR_SIZE     (512)
#define BLOCKS_PER_BANK 0x7a7800

#define STATUS_REG      (*(volatile unsigned int *)(0x70008204))
#define REG_1           (*(volatile unsigned int *)(0x70008208))
#define UNKNOWN         (*(volatile unsigned int *)(0x70008210))
#define BLOCK_SIZE_REG  (*(volatile unsigned int *)(0x7000821c))
#define BLOCK_COUNT_REG (*(volatile unsigned int *)(0x70008220))
#define REG_5           (*(volatile unsigned int *)(0x70008224))
#define CMD_REG0        (*(volatile unsigned int *)(0x70008228))
#define CMD_REG1        (*(volatile unsigned int *)(0x7000822c))
#define CMD_REG2        (*(volatile unsigned int *)(0x70008230))
#define RESPONSE_REG    (*(volatile unsigned int *)(0x70008234))
#define SD_STATE_REG    (*(volatile unsigned int *)(0x70008238))
#define REG_11          (*(volatile unsigned int *)(0x70008240))
#define REG_12          (*(volatile unsigned int *)(0x70008244))
#define DATA_REG        (*(volatile unsigned int *)(0x70008280))

/* STATUS_REG bits */
#define DATA_DONE       (1 << 12)
#define CMD_DONE        (1 << 13)
#define ERROR_BITS      (0x3f)
#define READY_FOR_DATA  (1 << 8)
#define FIFO_FULL       (1 << 7)
#define FIFO_EMPTY      (1 << 6)

#define CMD_OK          0x0 /* Command was successful */
#define CMD_ERROR_2     0x2 /* SD did not respond to command (either it doesn't
                               understand the command or is not inserted) */

/* SD States */
#define IDLE            0
#define READY           1
#define IDENT           2
#define STBY            3
#define TRAN            4
#define DATA            5
#define RCV             6
#define PRG             7
#define DIS             8

#define FIFO_LEN        16          /* FIFO is 16 words deep */

/* SD Commands */
#define GO_IDLE_STATE         0
#define ALL_SEND_CID          2
#define SEND_RELATIVE_ADDR    3
#define SET_DSR               4
#define SWITCH_FUNC           6
#define SELECT_CARD           7
#define DESELECT_CARD         7
#define SEND_IF_COND          8
#define SEND_CSD              9
#define SEND_CID             10
#define STOP_TRANSMISSION    12
#define SEND_STATUS          13
#define GO_INACTIVE_STATE    15
#define SET_BLOCKLEN         16
#define READ_SINGLE_BLOCK    17
#define READ_MULTIPLE_BLOCK  18
#define SEND_NUM_WR_BLOCKS   22
#define WRITE_BLOCK          24
#define WRITE_MULTIPLE_BLOCK 25
#define ERASE_WR_BLK_START   32
#define ERASE_WR_BLK_END     33
#define ERASE                38
#define APP_CMD              55

#define EC_OK                    0
#define EC_FAILED                1
#define EC_NOCARD                2
#define EC_WAIT_STATE_FAILED     3
#define EC_CHECK_TIMEOUT_FAILED  4
#define EC_POWER_UP              5
#define EC_READ_TIMEOUT          6
#define EC_WRITE_TIMEOUT         7
#define EC_TRAN_SEL_BANK         8
#define EC_TRAN_READ_ENTRY       9
#define EC_TRAN_READ_EXIT       10
#define EC_TRAN_WRITE_ENTRY     11
#define EC_TRAN_WRITE_EXIT      12
#define EC_FIFO_SEL_BANK_EMPTY  13
#define EC_FIFO_SEL_BANK_DONE   14
#define EC_FIFO_ENA_BANK_EMPTY  15
#define EC_FIFO_READ_FULL       16
#define EC_FIFO_WR_EMPTY        17
#define EC_FIFO_WR_DONE         18
#define EC_COMMAND              19
#define NUM_EC                  20

/* Application Specific commands */
#define SET_BUS_WIDTH   6
#define SD_APP_OP_COND  41

/** global, exported variables **/
#ifdef HAVE_HOTSWAP
#define NUM_VOLUMES 2
#else
#define NUM_VOLUMES 1
#endif

/* for compatibility */
int ata_spinup_time = 0;

//long last_disk_activity = -1;

#define intptr_t    long

typedef int bool;
#define false 0
#define true 1

/** static, private data **/ 
static bool initialized = false;

static long next_yield = 0;
#define MIN_YIELD_PERIOD 1000

#define TIME_AFTER(a,b)         ((long)(b) - (long)(a) < 0)

typedef struct
{
   int initialized;
   
   unsigned int ocr;            /* OCR register */
   unsigned int csd[4];         /* CSD register */
   unsigned int cid[4];         /* CID register */
   unsigned int rca;
   
   uint64_t capacity;           /* size in bytes */
   unsigned long numblocks;     /* size in flash blocks */
   unsigned int block_size;     /* block size in bytes */
   unsigned int max_read_bl_len;/* max read data block length */
   unsigned int block_exp;      /* block size exponent */
   unsigned char current_bank;  /* The bank that we are working with */
} tSDCardInfo;

static tSDCardInfo card_info[2];
static tSDCardInfo *currcard = NULL; /* current active card */

static int hd_sizes[1<<6];
static int hd_blocksizes[1<<6];
static int hd_hardsectsizes[1<<6];
static int hd_maxsect[1<<6];

static struct hd_struct hd[1<<6];

struct sd_card_status
{
    int retry;
    int retry_max;
};

static struct sd_card_status sd_status[NUM_VOLUMES] =
{
    { 0, 1  },
#ifdef HAVE_HOTSWAP
    { 0, 10 }
#endif
};

/* Shoot for around 75% usage */
//static long sd_stack [(DEFAULT_STACK_SIZE*2 + 0x1c0)/sizeof(long)];
spinlock_t    sd_spin = SPIN_LOCK_UNLOCKED;
//static struct event_queue sd_queue;

/* Posted when card plugged status has changed */
#define SD_HOTSWAP    1
/* Actions taken by sd_thread when card status has changed */
enum sd_thread_actions
{
    SDA_NONE      = 0x0,
    SDA_UNMOUNTED = 0x1,
    SDA_MOUNTED   = 0x2
};

/* Private Functions */

static unsigned int check_time[NUM_EC];

static inline bool sd_check_timeout(long timeout, int id)
{
    return !TIME_AFTER(USEC_TIMER, check_time[id] + timeout);
}

static bool sd_poll_status(unsigned int trigger, long timeout)
{
    long t = USEC_TIMER;

    while ((STATUS_REG & trigger) == 0)
    {
        long time = USEC_TIMER;

        if (TIME_AFTER(time, next_yield))
        {
            long ty = USEC_TIMER;
 //           priority_yield();
            timeout += USEC_TIMER - ty;
            next_yield = ty + MIN_YIELD_PERIOD;
        }

        if (TIME_AFTER(time, t + timeout))
            return false;
    }

    return true;
}

static int sd_command(unsigned int cmd, unsigned long arg1,
                      unsigned int *response, unsigned int type)
{
    int i, words; /* Number of 16 bit words to read from RESPONSE_REG */
    unsigned int data[9];

    CMD_REG0 = cmd;
    CMD_REG1 = (unsigned int)((arg1 & 0xffff0000) >> 16);
    CMD_REG2 = (unsigned int)((arg1 & 0xffff));
    UNKNOWN  = type;

    if (!sd_poll_status(CMD_DONE, 100000))
        return -EC_COMMAND;

    if ((STATUS_REG & ERROR_BITS) != CMD_OK)
        /* Error sending command */
        return -EC_COMMAND - (STATUS_REG & ERROR_BITS)*100;

    if (cmd == GO_IDLE_STATE)
        return 0; /* no response here */

    words = (type == 2) ? 9 : 3;

    for (i = 0; i < words; i++) /* RESPONSE_REG is read MSB first */
        data[i] = RESPONSE_REG; /* Read most significant 16-bit word */

    if (response == NULL)
    {
        /* response discarded */
    }
    else if (type == 2)
    {
        /* Response type 2 has the following structure:
         * [135:135] Start Bit - '0'
         * [134:134] Transmission bit - '0'
         * [133:128] Reserved - '111111'
         * [127:001] CID or CSD register including internal CRC7
         * [000:000] End Bit - '1'
         */
        response[3] = (data[0]<<24) + (data[1]<<8) + (data[2]>>8);
        response[2] = (data[2]<<24) + (data[3]<<8) + (data[4]>>8);
        response[1] = (data[4]<<24) + (data[5]<<8) + (data[6]>>8);
        response[0] = (data[6]<<24) + (data[7]<<8) + (data[8]>>8);
    }
    else
    {
        /* Response types 1, 1b, 3, 6, 7 have the following structure:
         * Types 4 and 5 are not supported.
         *
         *     [47] Start bit - '0'
         *     [46] Transmission bit - '0'
         *  [45:40] R1, R1b, R6, R7: Command index
         *          R3: Reserved - '111111'
         *   [39:8] R1, R1b: Card Status
         *          R3: OCR Register
         *          R6: [31:16] RCA
         *              [15: 0] Card Status Bits 23, 22, 19, 12:0
         *                     [23] COM_CRC_ERROR
         *                     [22] ILLEGAL_COMMAND
         *                     [19] ERROR
         *                   [12:9] CURRENT_STATE
         *                      [8] READY_FOR_DATA
         *                    [7:6]
         *                      [5] APP_CMD
         *                      [4]
         *                      [3] AKE_SEQ_ERROR
         *                      [2] Reserved
         *                    [1:0] Reserved for test mode
         *          R7: [19:16] Voltage accepted
         *              [15:8]  echo-back of check pattern
         *    [7:1] R1, R1b: CRC7
         *          R3: Reserved - '1111111'
         *      [0] End Bit - '1'
         */
        response[0] = (data[0]<<24) + (data[1]<<8) + (data[2]>>8);
    }

    return 0;
}

static int sd_wait_for_state(unsigned int state, int id)
{
    unsigned int response = 0;
    unsigned int timeout = 0x80000;

    check_time[id] = USEC_TIMER;

    while (1)
    {
        int ret = sd_command(SEND_STATUS, currcard->rca, &response, 1);
        long us;

        if (ret < 0)
            return ret*100 - id;

        if (((response >> 9) & 0xf) == state)
        {
            SD_STATE_REG = state;
            return 0;
        }

        if (!sd_check_timeout(timeout, id))
            return -EC_WAIT_STATE_FAILED*100 - id;

 //       mdelay(10);

        us = USEC_TIMER;
        if (TIME_AFTER(us, next_yield))
        {
   //         priority_yield();
            timeout += USEC_TIMER - us;
            next_yield = us + MIN_YIELD_PERIOD;
        }
    }
}

static inline void copy_read_sectors_fast(unsigned char **buf)
{
    /* Copy one chunk of 16 words using best method for start alignment */
    switch ( (intptr_t)*buf & 3 )
    {
    case 0:
        asm volatile (
            "ldmia  %1, { r2-r9 }          \r\n"
            "orr    r2, r2, r3, lsl #16         \r\n"
            "orr    r4, r4, r5, lsl #16         \r\n"
            "orr    r6, r6, r7, lsl #16         \r\n"
            "orr    r8, r8, r9, lsl #16         \r\n"
            "stmia  %0!, { r2, r4, r6, r8 } \r\n"
            "ldmia  %1, { r2-r9 }          \r\n"
            "orr    r2, r2, r3, lsl #16         \r\n"
            "orr    r4, r4, r5, lsl #16         \r\n"
            "orr    r6, r6, r7, lsl #16         \r\n"
            "orr    r8, r8, r9, lsl #16         \r\n"
            "stmia  %0!, { r2, r4, r6, r8 } \r\n"
            : "+&r"(*buf)
            : "r"(&DATA_REG)
            : "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9"
        );
        break;
    case 1:
        asm volatile (
            "ldmia  %1, { r2-r9 }          \r\n"
            "orr    r3, r2, r3, lsl #16         \r\n"
            "strb   r3, [%0], #1            \r\n"
            "mov    r3, r3, lsr #8              \r\n"
            "strh   r3, [%0], #2            \r\n"
            "mov    r3, r3, lsr #16             \r\n"
            "orr    r3, r3, r4, lsl #8          \r\n"
            "orr    r3, r3, r5, lsl #24         \r\n"
            "mov    r5, r5, lsr #8              \r\n"
            "orr    r5, r5, r6, lsl #8          \r\n"
            "orr    r5, r5, r7, lsl #24         \r\n"
            "mov    r7, r7, lsr #8              \r\n"
            "orr    r7, r7, r8, lsl #8          \r\n"
            "orr    r7, r7, r9, lsl #24         \r\n"
            "mov    r2, r9, lsr #8              \r\n"
            "stmia  %0!, { r3, r5, r7 }     \r\n"
            "ldmia  %1, { r3-r10 }         \r\n"
            "orr    r2, r2, r3, lsl #8          \r\n"
            "orr    r2, r2, r4, lsl #24         \r\n"
            "mov    r4, r4, lsr #8              \r\n"
            "orr    r4, r4, r5, lsl #8          \r\n"
            "orr    r4, r4, r6, lsl #24         \r\n"
            "mov    r6, r6, lsr #8              \r\n"
            "orr    r6, r6, r7, lsl #8          \r\n"
            "orr    r6, r6, r8, lsl #24         \r\n"
            "mov    r8, r8, lsr #8              \r\n"
            "orr    r8, r8, r9, lsl #8          \r\n"
            "orr    r8, r8, r10, lsl #24        \r\n"
            "mov    r10, r10, lsr #8            \r\n"
            "stmia  %0!, { r2, r4, r6, r8 } \r\n"
            "strb   r10, [%0], #1           \r\n"
            : "+&r"(*buf)
            : "r"(&DATA_REG)
            : "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10"
        );
        break;
    case 2:
        asm volatile (
            "ldmia  %1, { r2-r9 }          \r\n"
            "strh   r2, [%0], #2            \r\n"
            "orr    r3, r3, r4, lsl #16         \r\n"
            "orr    r5, r5, r6, lsl #16         \r\n"
            "orr    r7, r7, r8, lsl #16         \r\n"
            "stmia  %0!, { r3, r5, r7 }     \r\n"
            "ldmia  %1, { r2-r8, r10 }     \r\n"
            "orr    r2, r9, r2, lsl #16         \r\n"
            "orr    r3, r3, r4, lsl #16         \r\n"
            "orr    r5, r5, r6, lsl #16         \r\n"
            "orr    r7, r7, r8, lsl #16         \r\n"
            "stmia  %0!, { r2, r3, r5, r7 } \r\n"
            "strh   r10, [%0], #2           \r\n"
            : "+&r"(*buf)
            : "r"(&DATA_REG)
            : "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10"
        );
        break;
    case 3:
        asm volatile (
            "ldmia  %1, { r2-r9 }          \r\n"
            "orr    r3, r2, r3, lsl #16         \r\n"
            "strb   r3, [%0], #1            \r\n"
            "mov    r3, r3, lsr #8              \r\n"
            "orr    r3, r3, r4, lsl #24         \r\n"
            "mov    r4, r4, lsr #8              \r\n"
            "orr    r5, r4, r5, lsl #8          \r\n"
            "orr    r5, r5, r6, lsl #24         \r\n"
            "mov    r6, r6, lsr #8              \r\n"
            "orr    r7, r6, r7, lsl #8          \r\n"
            "orr    r7, r7, r8, lsl #24         \r\n"
            "mov    r8, r8, lsr #8              \r\n"
            "orr    r2, r8, r9, lsl #8          \r\n"
            "stmia  %0!, { r3, r5, r7 }     \r\n"
            "ldmia  %1, { r3-r10 }         \r\n"
            "orr    r2, r2, r3, lsl #24         \r\n"
            "mov    r3, r3, lsr #8              \r\n"
            "orr    r4, r3, r4, lsl #8          \r\n"
            "orr    r4, r4, r5, lsl #24         \r\n"
            "mov    r5, r5, lsr #8              \r\n"
            "orr    r6, r5, r6, lsl #8          \r\n"
            "orr    r6, r6, r7, lsl #24         \r\n"
            "mov    r7, r7, lsr #8              \r\n"
            "orr    r8, r7, r8, lsl #8          \r\n"
            "orr    r8, r8, r9, lsl #24         \r\n"
            "mov    r9, r9, lsr #8              \r\n"
            "orr    r10, r9, r10, lsl #8        \r\n"
            "stmia  %0!, { r2, r4, r6, r8 } \r\n"
            "strh   r10, [%0], #2           \r\n"
            "mov    r10, r10, lsr #16           \r\n"
            "strb   r10, [%0], #1           \r\n"
            : "+&r"(*buf)
            : "r"(&DATA_REG)
            : "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10"
        );
        break;
    }
}

static inline void copy_read_sectors_slow(unsigned char** buf)
{
   int cnt = FIFO_LEN;
   /* Copy one chunk of 16 words */
   asm volatile (
      "mov r1, %2              \r\n"
      "1:                                     \r\n"
      "ldrh   r0, [%1]             \r\n"
      "strb   r0, [%0], #1          \r\n"
      "mov    r0, r0, lsr #8          \r\n"
      "strb   r0, [%0], #1          \r\n"
      "subs   r1, r1, #1          \r\n"
      "bgt    1b                          \r\n"
      :"+&r"(*buf)  //out
      :"r"(&DATA_REG), "r"(cnt) //in
      );
}

/* Writes have to be kept slow for now */
static inline void copy_write_sectors(const unsigned char** buf)
{
    int cnt = FIFO_LEN;
    unsigned t;

    do
    {
        t  = *(*buf)++;
        t |= *(*buf)++ << 8;
        DATA_REG = t;
    } while (--cnt > 0); /* tail loop is faster */
}

static int sd_select_bank(unsigned char bank)
{
    unsigned char card_data[512];
    const unsigned char* write_buf;
    int i, ret;

    memset(card_data, 0, 512);
    ret = sd_wait_for_state(TRAN, EC_TRAN_SEL_BANK);
    if (ret < 0)
        return ret;

    BLOCK_SIZE_REG = 512;
    BLOCK_COUNT_REG = 1;

    ret = sd_command(35, 0, NULL, 0x1c0d); /* CMD35 is vendor specific */
    if (ret < 0)
    {
        return ret;
    }

    SD_STATE_REG = PRG;

    card_data[0] = bank;

    /* Write the card data */
    write_buf = card_data;
    for (i = 0; i < SD_BLOCK_SIZE/2; i += FIFO_LEN)
    {
        /* Wait for the FIFO to empty */
        if (sd_poll_status(FIFO_EMPTY, 10000))
        {
            copy_write_sectors(&write_buf); /* Copy one chunk of 16 words */
            continue;
        }

       return -EC_FIFO_SEL_BANK_EMPTY;
    }

    if (!sd_poll_status(DATA_DONE, 10000))
    {
        return -EC_FIFO_SEL_BANK_DONE;
    }
    currcard->current_bank = bank;

    return 0;
}

static void sd_card_mux(int card_no)
{
/* Set the current card mux */
#ifdef SANSA_E200
    if (card_no == 0)
    {
        GPO32_VAL |= 0x4;

        GPIOA_ENABLE     &= ~0x7a;
        GPIOA_OUTPUT_EN  &= ~0x7a;
        GPIOD_ENABLE     |=  0x1f;
        GPIOD_OUTPUT_VAL |=  0x1f;
        GPIOD_OUTPUT_EN  |=  0x1f;

        outl((inl(0x70000014) & ~(0x3ffff)) | 0x255aa, 0x70000014);
    }
    else
    {
        GPO32_VAL &= ~0x4;

        GPIOD_ENABLE     &= ~0x1f;
        GPIOD_OUTPUT_EN  &= ~0x1f;
        GPIOA_ENABLE     |=  0x7a;
        GPIOA_OUTPUT_VAL |=  0x7a;
        GPIOA_OUTPUT_EN  |=  0x7a;

        outl(inl(0x70000014) & ~(0x3ffff), 0x70000014);
    }
#else /* SANSA_C200 */
    if (card_no == 0)
    {
        GPO32_VAL |= 0x4;

        GPIOD_ENABLE     &= ~0x1f;
        GPIOD_OUTPUT_EN  &= ~0x1f;
        GPIOA_ENABLE     |=  0x7a;
        GPIOA_OUTPUT_VAL |=  0x7a;
        GPIOA_OUTPUT_EN  |=  0x7a;

        outl(inl(0x70000014) & ~(0x3ffff), 0x70000014);
    }
    else
    {
        GPO32_VAL &= ~0x4;

        GPIOA_ENABLE     &= ~0x7a;
        GPIOA_OUTPUT_EN  &= ~0x7a;
        GPIOD_ENABLE     |=  0x1f;
        GPIOD_OUTPUT_VAL |=  0x1f;
        GPIOD_OUTPUT_EN  |=  0x1f;

        outl((inl(0x70000014) & ~(0x3ffff)) | 0x255aa, 0x70000014);
    }
#endif
}

bool card_detect_target(void)
{
#ifdef SANSA_E200
   return (GPIOA_INPUT_VAL & 0x80) == 0; /* low active */
#elif defined SANSA_C200
   return (GPIOL_INPUT_VAL & 0x08) != 0; /* high active */
#endif
}

static void sd_init_device(int card_no)
{
/* SD Protocol registers */
#ifdef HAVE_HOTSWAP
    unsigned int response = 0;
#endif
    unsigned int  i;
    unsigned int  c_size;
    unsigned long c_mult;
    unsigned char carddata[512];
    unsigned char *dataptr;
    int ret;

/* Enable and initialise controller */
    REG_1 = 6;

/* Initialise card data as blank */
    memset(currcard, 0, sizeof(*currcard));

/* Switch card mux to card to initialize */
    sd_card_mux(card_no);

/* Init NAND */
    REG_11 |=  (1 << 15);
    REG_12 |=  (1 << 15);
    REG_12 &= ~(3 << 12);
    REG_12 |=  (1 << 13);
    REG_11 &= ~(3 << 12);
    REG_11 |=  (1 << 13);

    DEV_EN |= DEV_ATA; /* Enable controller */
    DEV_RS |= DEV_ATA; /* Reset controller */
    DEV_RS &=~DEV_ATA; /* Clear Reset */

    SD_STATE_REG = TRAN;

    REG_5 = 0xf;

    ret = sd_command(GO_IDLE_STATE, 0, NULL, 256);
    if (ret < 0)
        goto card_init_error;

    check_time[EC_POWER_UP] = USEC_TIMER;

#ifdef HAVE_HOTSWAP
    /* Check for SDHC:
       - non-SDHC cards simply ignore SEND_IF_COND (CMD8) and we get error -219,
         which we can just ignore and assume we're dealing with standard SD.
       - SDHC cards echo back the argument into the response. This is how we
         tell if the card is SDHC.
     */
    ret = sd_command(SEND_IF_COND,0x1aa, &response,7);
    if ( (ret < 0) && (ret!=-219) )
            goto card_init_error;
#endif

    while ((currcard->ocr & (1 << 31)) == 0) /* until card is powered up */
    {
        ret = sd_command(APP_CMD, currcard->rca, NULL, 1);
        if (ret < 0)
            goto card_init_error;

#ifdef HAVE_HOTSWAP
        if(response == 0x1aa)
        {
            /* SDHC */
            ret = sd_command(SD_APP_OP_COND, (1<<30)|0x100000,
                             &currcard->ocr, 3);
        }
        else
#endif /* HAVE_HOTSWAP */
        {
            /* SD Standard */
            ret = sd_command(SD_APP_OP_COND, 0x100000, &currcard->ocr, 3);
        }

        if (ret < 0)
            goto card_init_error;

        if (!sd_check_timeout(5000000, EC_POWER_UP))
        {
            ret = -EC_POWER_UP;
            goto card_init_error;
        }
    }

    ret = sd_command(ALL_SEND_CID, 0, currcard->cid, 2);
    if (ret < 0)
        goto card_init_error;

    ret = sd_command(SEND_RELATIVE_ADDR, 0, &currcard->rca, 1);
    if (ret < 0)
        goto card_init_error;

    ret = sd_command(SEND_CSD, currcard->rca, currcard->csd, 2);
    if (ret < 0)
        goto card_init_error;


    /* These calculations come from the Sandisk SD card product manual */
    if( (currcard->csd[3]>>30) == 0)
    {
        /* CSD version 1.0 */
        c_size = ((currcard->csd[2] & 0x3ff) << 2) + (currcard->csd[1]>>30) + 1;
        c_mult = 4 << ((currcard->csd[1] >> 15) & 7);
        currcard->max_read_bl_len = 1 << ((currcard->csd[2] >> 16) & 15);
        currcard->block_size = SD_BLOCK_SIZE;     /* Always use 512 byte blocks */
        currcard->numblocks = c_size * c_mult * (currcard->max_read_bl_len/512);
        currcard->capacity = (uint64_t)currcard->numblocks * (uint64_t)currcard->block_size;
    }
#ifdef HAVE_HOTSWAP
    else if( (currcard->csd[3]>>30) == 1)
    {
        /* CSD version 2.0 */
        c_size = ((currcard->csd[2] & 0x3f) << 16) + (currcard->csd[1]>>16) + 1;
        currcard->max_read_bl_len = 1 << ((currcard->csd[2] >> 16) & 0xf);
        currcard->block_size = SD_BLOCK_SIZE;     /* Always use 512 byte blocks */
        currcard->numblocks = c_size;
        currcard->capacity = currcard->numblocks * currcard->block_size;
    }
#endif /* HAVE_HOTSWAP */
    
    REG_1 = 0;

    ret = sd_command(SELECT_CARD, currcard->rca, NULL, 129);
    if (ret < 0)
        goto card_init_error;

    ret = sd_command(APP_CMD, currcard->rca, NULL, 1);
    if (ret < 0)
        goto card_init_error;

    ret = sd_command(SET_BUS_WIDTH, currcard->rca | 2, NULL, 1); /* 4 bit */
    if (ret < 0)
        goto card_init_error;

    ret = sd_command(SET_BLOCKLEN, currcard->block_size, NULL, 1);
    if (ret < 0)
        goto card_init_error;

    BLOCK_SIZE_REG = currcard->block_size;

    /* If this card is >4GB & not SDHC, then we need to enable bank switching */
    if( (currcard->numblocks >= BLOCKS_PER_BANK) &&
        ((currcard->ocr & (1<<30)) == 0) )
    {
        SD_STATE_REG = TRAN;
        BLOCK_COUNT_REG = 1;

        ret = sd_command(SWITCH_FUNC, 0x80ffffef, NULL, 0x1c05);
        if (ret < 0)
            goto card_init_error;

        /* Read 512 bytes from the card.
        The first 512 bits contain the status information
        TODO: Do something useful with this! */
        dataptr = carddata;
        for (i = 0; i < SD_BLOCK_SIZE/2; i += FIFO_LEN)
        {
            /* Wait for the FIFO to be full */
            if (sd_poll_status(FIFO_FULL, 100000))
            {
                copy_read_sectors_slow(&dataptr);
                continue;
            }

            ret = -EC_FIFO_ENA_BANK_EMPTY;
            goto card_init_error;
        }
    }
    
/*    hd_sizes[card_no] = currcard->capacity;
    hd_blocksizes[card_no] = currcard->block_size;
    hd[card_no].nr_sects = currcard->numblocks;
    
    hd_hardsectsizes = 
       hd_maxsect =    
       //    hardsect_size[MAJOR_NR] = hd_hardsectsizes;
       //    max_sectors[MAJOR_NR] = hd_maxsect;
*/
    currcard->initialized = 1;
    return;

    /* Card failed to initialize so disable it */
card_init_error:
    currcard->initialized = ret;
}

/* lock must already be aquired */
static void sd_select_device(int card_no)
{
    currcard = &card_info[card_no];

    if (card_no == 0)
    {
        /* Main card always gets a chance */
        sd_status[0].retry = 0;
    }

    if (currcard->initialized > 0)
    {
        /* This card is already initialized - switch to it */
        sd_card_mux(card_no);
        return;
    }

    if (currcard->initialized == 0)
    {
        /* Card needs (re)init */
        sd_init_device(card_no);
    }
}


int sd_read_sectors(int drive, unsigned long start, int incount,
                     void* inbuf)
{
    int ret;
    unsigned char *buf, *buf_end;
    int bank;

    /* TODO: Add DMA support. */

    spin_lock(&sd_spin);
ata_read_retry:
    if (drive != 0 && !card_detect_target())
    {
        /* no external sd-card inserted */
        ret = -EC_NOCARD;
        goto ata_read_error;
    }

    sd_select_device(drive);

    if (currcard->initialized < 0)
    {
        ret = currcard->initialized;
        goto ata_read_error;
    }

//    last_disk_activity = current_tick;

    /* Only switch banks with non-SDHC cards */
    if((currcard->ocr & (1<<30))==0)
    {
        bank = start / BLOCKS_PER_BANK;

        if (currcard->current_bank != bank)
        {
            ret = sd_select_bank(bank);
            if (ret < 0)
                goto ata_read_error;
        }
    
        start -= bank * BLOCKS_PER_BANK;
    }

    ret = sd_wait_for_state(TRAN, EC_TRAN_READ_ENTRY);
    if (ret < 0)
        goto ata_read_error;

    BLOCK_COUNT_REG = incount;

#ifdef HAVE_HOTSWAP
    if(currcard->ocr & (1<<30) )
    {
        /* SDHC */
        ret = sd_command(READ_MULTIPLE_BLOCK, start, NULL, 0x1c25);
    }
    else
#endif
    {
        ret = sd_command(READ_MULTIPLE_BLOCK, start * SD_BLOCK_SIZE, NULL, 0x1c25);
    }
    if (ret < 0)
        goto ata_read_error;

    /* TODO: Don't assume BLOCK_SIZE == SECTOR_SIZE */

    buf_end = (unsigned char *)inbuf + incount * currcard->block_size;
    for (buf = inbuf; buf < buf_end;)
    {
        /* Wait for the FIFO to be full */
        if (sd_poll_status(FIFO_FULL, 0x80000))
        {
            copy_read_sectors_fast(&buf); /* Copy one chunk of 16 words */
            /* TODO: Switch bank if necessary */
            continue;
        }

        ret = -EC_FIFO_READ_FULL;
        goto ata_read_error;
    }

//    last_disk_activity = current_tick;

    ret = sd_command(STOP_TRANSMISSION, 0, NULL, 1);
    if (ret < 0)
        goto ata_read_error;

    ret = sd_wait_for_state(TRAN, EC_TRAN_READ_EXIT);
    if (ret < 0)
        goto ata_read_error;

    while (1)
    {
        spin_unlock(&sd_spin);

        return ret;

ata_read_error:
        if (sd_status[drive].retry < sd_status[drive].retry_max
            && ret != -EC_NOCARD)
        {
            sd_status[drive].retry++;
            currcard->initialized = 0;
            goto ata_read_retry;
        }
    }
}

int sd_write_sectors(int drive, unsigned long start, int count,
                      const void* outbuf)
{
/* Write support is not finished yet */
/* TODO: The standard suggests using ACMD23 prior to writing multiple blocks
   to improve performance */
    int ret;
    const unsigned char *buf, *buf_end;
    int bank;

    spin_lock(&sd_spin);

ata_write_retry:
    if (drive != 0 && !card_detect_target())
    {
        /* no external sd-card inserted */
        ret = -EC_NOCARD;
        goto ata_write_error;
    }

    sd_select_device(drive);

    if (currcard->initialized < 0)
    {
        ret = currcard->initialized;
        goto ata_write_error;
    }

    /* Only switch banks with non-SDHC cards */
    if((currcard->ocr & (1<<30))==0)
    {
        bank = start / BLOCKS_PER_BANK;

        if (currcard->current_bank != bank)
        {
            ret = sd_select_bank(bank);
            if (ret < 0)
                goto ata_write_error;
        }
    
        start -= bank * BLOCKS_PER_BANK;
    }

    check_time[EC_WRITE_TIMEOUT] = USEC_TIMER;

    ret = sd_wait_for_state(TRAN, EC_TRAN_WRITE_ENTRY);
    if (ret < 0)
        goto ata_write_error;

    BLOCK_COUNT_REG = count;

#ifdef HAVE_HOTSWAP
    if(currcard->ocr & (1<<30) )
    {
        /* SDHC */
        ret = sd_command(WRITE_MULTIPLE_BLOCK, start, NULL, 0x1c2d);
    }
    else
#endif
    {
        ret = sd_command(WRITE_MULTIPLE_BLOCK, start*SD_BLOCK_SIZE, NULL, 0x1c2d);
    }
    if (ret < 0)
        goto ata_write_error;

    buf_end = outbuf + count * currcard->block_size - 2*FIFO_LEN;

    for (buf = outbuf; buf <= buf_end;)
    {
        if (buf == buf_end)
        {
            /* Set SD_STATE_REG to PRG for the last buffer fill */
            SD_STATE_REG = PRG;
        }

        udelay(2); /* needed here (loop is too fast :-) */

        /* Wait for the FIFO to empty */
        if (sd_poll_status(FIFO_EMPTY, 0x80000))
        {
            copy_write_sectors(&buf); /* Copy one chunk of 16 words */
            /* TODO: Switch bank if necessary */
            continue;
        }

        ret = -EC_FIFO_WR_EMPTY;
        goto ata_write_error;
    }

//    last_disk_activity = current_tick;

    if (!sd_poll_status(DATA_DONE, 0x80000))
    {
        ret = -EC_FIFO_WR_DONE;
        goto ata_write_error;
    }

    ret = sd_command(STOP_TRANSMISSION, 0, NULL, 1);
    if (ret < 0)
        goto ata_write_error;

    ret = sd_wait_for_state(TRAN, EC_TRAN_WRITE_EXIT);
    if (ret < 0)
        goto ata_write_error;

    while (1)
    {
        spin_unlock(&sd_spin);

        return ret;

ata_write_error:
        if (sd_status[drive].retry < sd_status[drive].retry_max
            && ret != -EC_NOCARD)
        {
            sd_status[drive].retry++;
            currcard->initialized = 0;
            goto ata_write_retry;
        }
    }
}
#if 0
static void sd_thread(void) __attribute__((noreturn));
static void sd_thread(void)
{
    struct queue_event ev;
    bool idle_notified = false;
    
    while (1)
    {
        queue_wait_w_tmo(&sd_queue, &ev, HZ);

        switch ( ev.id ) 
        {
#ifdef HAVE_HOTSWAP
        case SD_HOTSWAP:
        {
            int action = SDA_NONE;

            /* Lock to keep us from messing with this variable while an init
               may be in progress */
            spin_lock(&sd_spin);
            card_info[1].initialized = 0;
            sd_status[1].retry = 0;

            /* Either unmount because the card was pulled or unmount and
               remount if already mounted since multiple messages may be
               generated for the same event - like someone inserting a new
               card before anything detects the old one pulled :) */
            if (disk_unmount(1) != 0)  /* release "by force" */
                action |= SDA_UNMOUNTED;

            if (ev.data != 0 && disk_mount(1) != 0) /* mount SD-CARD */
                action |= SDA_MOUNTED;

            if (action & SDA_UNMOUNTED)
                queue_broadcast(SYS_HOTSWAP_EXTRACTED, 0);

            if (action & SDA_MOUNTED)
                queue_broadcast(SYS_HOTSWAP_INSERTED, 0);

            if (action != SDA_NONE)
                queue_broadcast(SYS_FS_CHANGED, 0);

            spin_unlock(&sd_spin);
            break;
            } /* SD_HOTSWAP */
#endif /* HAVE_HOTSWAP */
        case SYS_TIMEOUT:
            if (TIME_BEFORE(current_tick, last_disk_activity+(3*HZ)))
            {
                idle_notified = false;
            }
            else
            {
                /* never let a timer wrap confuse us */
                next_yield = USEC_TIMER;

                if (!idle_notified)
                {
                    call_ata_idle_notifys(false);
                    idle_notified = true;
                }
            }
            break;
        case SYS_USB_CONNECTED:
            usb_acknowledge(SYS_USB_CONNECTED_ACK);
            /* Wait until the USB cable is extracted again */
            usb_wait_for_disconnect(&sd_queue);
            break;
        }
    }
}
#endif
void sd_enable(bool on)
{
    if(on)
    {
        DEV_EN |= DEV_ATA; /* Enable controller */
    }
    else
    {
        DEV_EN &= ~DEV_ATA; /* Disable controller */
    }
}


int sd_HwInit(void)
{
    int ret = 0;

    /* NOTE: This init isn't dual core safe */
    if (!initialized)
    {
        initialized = true;

        spin_lock_init(&sd_spin);// IF_COP(, SPINLOCK_TASK_SWITCH));

        spin_lock(&sd_spin);

        sd_enable(true);
        /* init controller */
        outl(inl(0x70000088) & ~(0x4), 0x70000088);
        outl(inl(0x7000008c) & ~(0x4), 0x7000008c);
        GPO32_ENABLE |= 0x4;

        GPIOG_ENABLE     |= (0x3 << 5);
        GPIOG_OUTPUT_EN  |= (0x3 << 5);
        GPIOG_OUTPUT_VAL |= (0x3 << 5);

#ifdef HAVE_HOTSWAP
        /* enable card detection port - mask interrupt first */
#ifdef SANSA_E200
        GPIOA_INT_EN     &= ~0x80;

        GPIOA_OUTPUT_EN  &= ~0x80;
        GPIOA_ENABLE     |=  0x80;
#elif defined SANSA_C200
        GPIOL_INT_EN     &= ~0x08;

        GPIOL_OUTPUT_EN  &= ~0x08;
        GPIOL_ENABLE     |=  0x08;
#endif
#endif
        sd_select_device(0);

        if (currcard->initialized < 0)
            ret = currcard->initialized;

 //       queue_init(&sd_queue, true);
 //       create_thread(sd_thread, sd_stack, sizeof(sd_stack), 0,
//            sd_thread_name IF_PRIO(, PRIORITY_SYSTEM) IF_COP(, CPU));

        /* enable interupt for the mSD card */
        mdelay(100);
#ifdef HAVE_HOTSWAP
#ifdef SANSA_E200
        CPU_INT_EN = HI_MASK;
        CPU_HI_INT_EN = GPIO0_MASK;

        GPIOA_INT_LEV = (GPIOA_INT_LEV & ~0x80) | (~GPIOA_INPUT_VAL & 0x80);

        GPIOA_INT_CLR = 0x80;
        GPIOA_INT_EN |= 0x80;
#elif defined SANSA_C200
        CPU_INT_EN = HI_MASK;
        CPU_HI_INT_EN = GPIO2_MASK;

        GPIOL_INT_LEV = (GPIOL_INT_LEV & ~0x08) | (~GPIOL_INPUT_VAL & 0x08);

        GPIOL_INT_CLR = 0x08;
        GPIOL_INT_EN |= 0x08;
#endif
#endif
        spin_unlock(&sd_spin);
    }

    return ret;
}
#if 0
/* move the sd-card info to mmc struct */
tCardInfo *card_get_info_target(int card_no)
{
    int i, temp;
    static tCardInfo card;
    static const char mantissa[] = {  /* *10 */
        0,  10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80 };
    static const int exponent[] = {  /* use varies */
      1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000 };

    card.initialized  = card_info[card_no].initialized;
    card.ocr          = card_info[card_no].ocr;
    for(i=0; i<4; i++)  card.csd[i] = card_info[card_no].csd[3-i];
    for(i=0; i<4; i++)  card.cid[i] = card_info[card_no].cid[3-i];
    card.numblocks    = card_info[card_no].numblocks;
    card.blocksize    = card_info[card_no].block_size;
    card.size         = card_info[card_no].capacity < 0xffffffff ?
                        card_info[card_no].capacity : 0xffffffff;
    card.block_exp    = card_info[card_no].block_exp;
    temp              = card_extract_bits(card.csd, 29, 3);
    card.speed        = mantissa[card_extract_bits(card.csd, 25, 4)]
                      * exponent[temp > 2 ? 7 : temp + 4];
    card.nsac         = 100 * card_extract_bits(card.csd, 16, 8);
    temp              = card_extract_bits(card.csd, 13, 3);
    card.tsac         = mantissa[card_extract_bits(card.csd, 9, 4)]
                      * exponent[temp] / 10;
    card.cid[0]       = htobe32(card.cid[0]); /* ascii chars here */
    card.cid[1]       = htobe32(card.cid[1]); /* ascii chars here */
    temp = *((char*)card.cid+13); /* adjust year<=>month, 1997 <=> 2000 */
    *((char*)card.cid+13) = (unsigned char)((temp >> 4) | (temp << 4)) + 3;

    return &card;
}
#endif

#ifdef HAVE_HOTSWAP
static bool sd1_oneshot_callback(struct timeout *tmo)
{
    /* Take final state only - insert/remove is bouncy */
    queue_remove_from_head(&sd_queue, SD_HOTSWAP);
    queue_post(&sd_queue, SD_HOTSWAP, tmo->data);
    return false;
}

/* called on insertion/removal interrupt */
void microsd_int(void)
{
    static struct timeout sd1_oneshot;

#ifdef SANSA_E200
    int detect = GPIOA_INPUT_VAL & 0x80;

    GPIOA_INT_LEV = (GPIOA_INT_LEV & ~0x80) | (detect ^ 0x80);
    GPIOA_INT_CLR = 0x80;

    timeout_register(&sd1_oneshot, sd1_oneshot_callback,
                     detect ? 1 : HZ/2, detect == 0);
#elif defined SANSA_C200
    int detect = GPIOL_INPUT_VAL & 0x08;

    GPIOL_INT_LEV = (GPIOL_INT_LEV & ~0x08) | (detect ^ 0x08);
    GPIOL_INT_CLR = 0x08;

    timeout_register(&sd1_oneshot, sd1_oneshot_callback,
                     detect ? HZ/2 : 1, detect != 0);
#endif

}
#endif /* HAVE_HOTSWAP */

static void sd_request(request_queue_t *q)
{
   unsigned int sd_address;
   unsigned char *buffer_address;
   unsigned int dev;
   int nr_sectors;
   int cmd;
   int code;
   
   (void)q;
   while (1)
   {
      code = 1; // Default is success
      INIT_REQUEST;
      sd_address = (CURRENT->sector + hd[MINOR(CURRENT->rq_dev)].start_sect);// * hd_hardsectsizes[0];
      buffer_address = CURRENT->buffer;
      nr_sectors = CURRENT->current_nr_sectors;
      cmd = CURRENT->cmd;

      
      dev = MINOR(CURRENT->rq_dev);
      if( CURRENT->sector >= hd[dev].nr_sects || ((CURRENT->sector+CURRENT->nr_sectors) > hd[dev].nr_sects))
//      if (((CURRENT->sector + CURRENT->current_nr_sectors + hd[MINOR(CURRENT->rq_dev)].start_sect) > hd[0].nr_sects))
      {
         code = 0;
      }
      else if (cmd == READ)
      {
         spin_unlock_irq(&io_request_lock);
         if(sd_read_sectors(0, sd_address, nr_sectors, buffer_address) != 0)
            code = 0;
         spin_lock_irq(&io_request_lock);
      }
      else if (cmd == WRITE)
      {
         spin_unlock_irq(&io_request_lock);
         if(sd_write_sectors(0, sd_address, nr_sectors, buffer_address) != 0)
            code = 0;
         spin_lock_irq(&io_request_lock);
      }
      else
      {
         code = 0;
      }
      end_request(code);
   }
}

extern struct gendisk hd_gendisk;
static int sd_revalidate(kdev_t dev)
{
   int target, max_p, start, i;
//   if (mmc_media_detect == 0) return -ENODEV;
   
   target = DEVICE_NR(dev);
   
   max_p = hd_gendisk.max_p;
   start = target << 6;
   for (i = max_p - 1; i >= 0; i--) {
      int minor = start + i;
      invalidate_device(MKDEV(MAJOR_NR, minor), 1);
      hd_gendisk.part[minor].start_sect = 0;
      hd_gendisk.part[minor].nr_sects = 0;
   }
   
   grok_partitions(&hd_gendisk, target, 1 << 6,
      hd_sizes[0]);
   
   return 0;
}


static int sd_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
   if (!inode || !inode->i_rdev)
      return -EINVAL;
   
   switch(cmd) {
   case BLKGETSIZE:
      return put_user(hd[MINOR(inode->i_rdev)].nr_sects, (unsigned long *)arg);
   case BLKGETSIZE64:
      return put_user((u64)hd[MINOR(inode->i_rdev)].
         nr_sects, (u64 *) arg);
   case BLKRRPART:
      if (!capable(CAP_SYS_ADMIN))
         return -EACCES;
      
      return sd_revalidate(inode->i_rdev);
   case HDIO_GETGEO:
      {
         struct hd_geometry *loc, g;
         loc = (struct hd_geometry *) arg;
         if (!loc)
            return -EINVAL;
         g.heads = 4;
         g.sectors = 16;
         g.cylinders = hd[0].nr_sects / (4 * 16);
         g.start = hd[MINOR(inode->i_rdev)].start_sect;
         return copy_to_user(loc, &g, sizeof(g)) ? -EFAULT : 0;
      }
   default:
      return blk_ioctl(inode->i_rdev, cmd, arg);
      return -1;
   }
}

static int sd_open(struct inode *inode, struct file *filp)
{
#if defined(MODULE)
   MOD_INC_USE_COUNT;
#endif
   return 0;
}

static int sd_release(struct inode *inode, struct file *filp)
{
#if defined(MODULE)
   MOD_DEC_USE_COUNT;
#endif
   return 0;
}

static struct block_device_operations sd_bdops = 
{
   open: sd_open,
   release: sd_release,
   ioctl: sd_ioctl
};

static struct gendisk hd_gendisk = 
{
//   major:		MAJOR_NR,
   major_name:	DEVICE_NAME,
   minor_shift:	6,
   max_p:		1 << 6,
   part:		hd,
   sizes:		hd_sizes,
   fops:		&sd_bdops,
};


static void sd_initDisk()
{
   int i;
   memset(hd_sizes, 0, sizeof(hd_sizes));
   
   hd_sizes[0] = (int)(currcard->capacity/1024);
  
   for(i=0; i<(1<<6); i++) {
      hd_blocksizes[i] = SD_BLOCK_SIZE;
      hd_hardsectsizes[i] = SD_SECTOR_SIZE;
      hd_maxsect[i] = currcard->max_read_bl_len;
   }

   blk_size[MAJOR_NR] = hd_sizes;
   blksize_size[MAJOR_NR] = hd_blocksizes;
   hardsect_size[MAJOR_NR] = hd_hardsectsizes;
   max_sectors[MAJOR_NR] = hd_maxsect;
   
//   blksize_size[MAJOR_NR][DEVICE_NR(0)] = SD_BLOCK_SIZE;
//   hardsect_size[MAJOR_NR][DEVICE_NR(0)] = SD_SECTOR_SIZE;
//   max_sectors[MAJOR_NR][DEVICE_NR(0)] = currcard->max_read_bl_len;
//   blk_size[MAJOR_NR][DEVICE_NR(0)] = (int)(currcard->capacity/1024);
   
   memset(hd, 0, sizeof(hd));
   hd[0].nr_sects = currcard->numblocks;
   
   hd_gendisk.nr_real = 1;

/*
printk("hd_sizes:%u\n",hd_sizes[0]);
printk("hd_maxsect:%u\n",hd_maxsect[0]);
printk("hd_hardsectsizes:%u\n",hd_hardsectsizes[0]);
printk("hd_blocksizes:%u\n",hd_blocksizes[0]);
printk("numblocks:%u\n",currcard->numblocks);
printk("block_size:%u\n",currcard->block_size);
mdelay(5000);
*/


   register_disk(&hd_gendisk, MKDEV(MAJOR_NR,0), 1<<6,
                 &sd_bdops, hd_sizes[0]); //*2  
   //hd_maxsect[0]/**2*/);

}


static void sd_exit(void)
{
   blk_size[MAJOR_NR] = NULL;
   blksize_size[MAJOR_NR] = NULL;
   hardsect_size[MAJOR_NR] = NULL;
   max_sectors[MAJOR_NR] = NULL;
   hd[0].nr_sects = 0;
}

int __init sd_driver_init(void)
{
	int rc;
   printk("Sansa SD-Flash driver Copyright (C) 2006-2008 Daniel Ankers, Sebastian Duell\n");   

   rc = register_blkdev(MAJOR_NR, DEVICE_NAME, &sd_bdops);
	if (rc < 0)
	{
		printk(KERN_WARNING "sd: can't get major %d\n", MAJOR_NR);
		return rc;
	}
   nr_major = rc;
   hd_gendisk.major = MAJOR_NR;

   blk_init_queue(BLK_DEFAULT_QUEUE(MAJOR_NR), sd_request);
   
	read_ahead[MAJOR_NR] = 8;
   add_gendisk(&hd_gendisk);
   sd_HwInit();
   sd_initDisk();

	return 0;
}

void __exit sd_driver_exit(void)
{
	int i;

	for (i = 0; i < (1 << 6); i++)
		fsync_dev(MKDEV(MAJOR_NR, i));

	blk_cleanup_queue(BLK_DEFAULT_QUEUE(MAJOR_NR));
	del_gendisk(&hd_gendisk);
   unregister_blkdev(MAJOR_NR, DEVICE_NAME);
	sd_exit();
}

#ifdef MODULE
MODULE_AUTHOR("Sebastian Duell <Sebastian.Duell@ipodlinux.org>");
MODULE_DESCRIPTION("Sansa e200 Flash driver");
MODULE_LICENSE("GPL");
#endif

module_exit(sd_driver_exit);

