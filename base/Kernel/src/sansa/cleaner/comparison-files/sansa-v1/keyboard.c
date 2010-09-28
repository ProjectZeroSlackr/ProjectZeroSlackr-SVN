/*
 * keyboard.c - keyboard driver for iPod
 *
 * taken from Rockbox and modified 
 *
 */

#include <linux/module.h>
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kbd_ll.h>
#include <linux/mm.h>
#include <linux/kbd_kern.h>
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/miscdevice.h>
#include <linux/poll.h>
#include <asm/io.h>
#include <asm/arch/irqs.h>
#include <asm/keyboard.h>
#include <asm/hardware.h>
#include <asm/uaccess.h>

#include "pp5020.h"


#define TIME_AFTER(a,b)		((long)(b) - (long)(a) < 0)
#define TIME_BEFORE(a,b)	TIME_AFTER(b,a)
#define current_tick		(signed)(USEC_TIMER/10000)

#define BUTTON_SCROLL_BACK	0x00000080
#define BUTTON_SCROLL_FWD	0x00000100

#define BUTTON_NONE		0
#define BUTTON_REL		0x02000000
#define BUTTON_REPEAT		0x04000000
#define BUTTON_TOUCHPAD		0x08000000

#define WHEEL_REPEAT_INTERVAL	300000
#define WHEEL_FAST_ON_INTERVAL	20000
#define WHEEL_FAST_OFF_INTERVAL	60000
#define WHEELCLICKS_PER_ROTATION 96

/* Clickwheel */
static unsigned	int	old_wheel_value		= 0;
static unsigned	int	wheel_repeat		= 0;
static unsigned	int	wheel_click_count	= 0;
static unsigned	int	wheel_delta		= 0;
static		int	wheel_fast_mode		= 0;
static unsigned	long	last_wheel_usec		= 0;
static unsigned	long	wheel_velocity		= 0;
static		long	last_wheel_post		= 0;
static		long	next_backlight_on	= 0;
/* Buttons */
static		int	hold_button		= 0;
static unsigned	int	int_btn			= 0x7f;

/* we use the keycodes and translation is 1 to 1 */
#define R_SC		KEY_R
#define L_SC		KEY_L

#define UP_SC		KEY_M
#define LEFT_SC		KEY_W
#define RIGHT_SC	KEY_F
#define DOWN_SC		KEY_D

#define HOLD_SC		KEY_H
#define REC_SC		KEY_C
#define POWER_SC	KEY_P
#define ACTION_SC	KEY_ENTER

/* need to pass something becuase we use a shared irq */
#define KEYBOARD_DEV_ID	(void *)0x4b455942

static unsigned ipod_hw_ver;

#define EV_SCROLL	0x1000
#define EV_TAP		0x2000
#define EV_PRESS	0x4000
#define EV_RELEASE	0x8000
#define EV_TOUCH	0x0100
#define EV_LIFT		0x0200
#define EV_MASK		0xff00

#define BTN_ACTION	0x0001
#define BTN_NEXT	0x0002
#define BTN_PREVIOUS	0x0004
#define BTN_PLAY	0x0008
#define BTN_MENU	0x0010
#define BTN_HOLD	0x0020
#define BTN_REC		0x0040
#define BTN_POWER	0x0080
#define BTN_MASK	0x00ff

#define SCROLL_LEFT	0x0080
#define SCROLL_RIGHT	0x0000
#define SCROLL(dist)	((dist) < 0? (-(dist) & 0x7f) | SCROLL_LEFT : (dist) & 0x7f)
#define SCROLL_MASK	0x007f

/*
 * We really want to restrict it to less-than-0x60,
 * but that's not a power of 2 so 0x5f doesn't work as mask.
 * We'll settle for 0x7f, since the val should never get
 * above 0x5f in hardware anyway.
 */
#define TAP(loc)	((loc) & 0x7f)
#define TOUCH(loc)	((loc) & 0x7f)
#define LIFT(loc)	((loc) & 0x7f)
#define TAP_MASK	0x007f
#define TOUCH_MASK	0x007f
#define LIFT_MASK	0x007f

static volatile int ikb_reading;
static volatile int ikb_opened;
static volatile unsigned short ikb_events[32];
static volatile unsigned ikb_ev_head, ikb_ev_tail;

static volatile unsigned ikb_pressed_at; /* jiffy value when user touched wheel, 0 if not touching. */
static volatile unsigned ikb_first_loc; /* location where user first touched wheel */
static volatile int ikb_current_scroll; /* current scroll distance */
static volatile unsigned ikb_buttons_pressed, ikb_buttons_pressed_new; /* mask of BTN_* values */

static DECLARE_WAIT_QUEUE_HEAD (ikb_read_wait);

static void ikb_push_event (unsigned ev) 
{
	if (!ikb_opened) return;
	
	if ((ikb_ev_head+1 == ikb_ev_tail) || (ikb_ev_head == 31 && ikb_ev_tail == 0))
		printk (KERN_ERR "dropping event %08x\n", ev);
	
	ikb_events[ikb_ev_head++] = ev;
	ikb_ev_head &= 31;
	wake_up_interruptible (&ikb_read_wait);
}

/* Turn the counter into a scroll event. */
static void ikb_make_scroll_event (void)
{
	if (ikb_current_scroll) {
		ikb_push_event (EV_SCROLL | SCROLL(ikb_current_scroll));
		ikb_current_scroll = 0;
	}
}

static void ikb_scroll (int dir) 
{
	ikb_current_scroll += dir;

	while (dir > 0) {
		handle_scancode (R_SC, 1);
		handle_scancode (R_SC, 0);
		dir--;
	}

	while (dir < 0) {
		handle_scancode (L_SC, 1);
		handle_scancode (L_SC, 0);
		dir++;
	}

	if (ikb_reading)
		ikb_make_scroll_event();
}

static void handle_scroll_wheel(int new_scroll, int was_hold, int reverse)
{
	static int prev_scroll = -1;
	static int scroll_state[4][4] = {
		{0, 1, -1, 0},
		{-1, 0, 0, 1},
		{1, 0, 0, -1},
		{0, -1, 1, 0}
	};

	if ( prev_scroll == -1 ) {
		prev_scroll = new_scroll;
	}
	else if (!was_hold) {
		switch (scroll_state[prev_scroll][new_scroll]) {
		case 1:
			if (reverse) {
				/* 'r' keypress */
				ikb_scroll (1);
			}
			else {
				/* 'l' keypress */
				ikb_scroll (-1);
			}
			break;
		case -1:
			if (reverse) {
				/* 'l' keypress */
				ikb_scroll (-1);
			}
			else {
				/* 'r' keypress */
				ikb_scroll (1);
			}
			break;
		default:
			/* only happens if we get out of sync */
			break;
		}
	}

	prev_scroll = new_scroll;
}

static void ikb_handle_button (int button, int press) 
{
	int sc;

	if (press)
		ikb_buttons_pressed_new |= button;
	else
		ikb_buttons_pressed_new &= ~button;
	
	/* Send the code to the TTY driver too */
	switch (button) {
	case BTN_ACTION:
		sc = ACTION_SC;
		break;
	case BTN_PREVIOUS:
		sc = DOWN_SC;
		break;
	case BTN_NEXT:
		sc = UP_SC;
		break;
	case BTN_MENU:
		sc = LEFT_SC;
		break;
	case BTN_PLAY:
		sc = RIGHT_SC;
		break;
	case BTN_HOLD:
		sc = HOLD_SC;
		break;
	case BTN_REC:
		sc = REC_SC;
		break;
	case BTN_POWER:
		sc = POWER_SC;
		break;
	default:
		sc = 0;
		break;
	}

	if (sc)
		handle_scancode (sc, press);
}

static void ikb_start_buttons (void)
{
	ikb_buttons_pressed_new = ikb_buttons_pressed;
}

static void ikb_finish_buttons (void)
{
	/* Pressed: */
	if (ikb_buttons_pressed_new & ~ikb_buttons_pressed) {
		ikb_push_event (EV_PRESS | (ikb_buttons_pressed_new & ~ikb_buttons_pressed));
	}
	/* Released: */
	if (ikb_buttons_pressed & ~ikb_buttons_pressed_new) {
		ikb_push_event (EV_RELEASE | (ikb_buttons_pressed & ~ikb_buttons_pressed_new));
	}
	
	ikb_buttons_pressed = ikb_buttons_pressed_new;
}

static ssize_t ikb_read (struct file *file, char *buf, size_t nbytes, loff_t *ppos) 
{
	DECLARE_WAITQUEUE (wait, current);
	ssize_t retval = 0, count = 0;
	
	if (nbytes == 0) return 0;

	ikb_make_scroll_event();
	ikb_reading = 1;

	add_wait_queue (&ikb_read_wait, &wait);
	while (nbytes > 0) {
		int ev;

		set_current_state (TASK_INTERRUPTIBLE);

		if (ikb_ev_head == ikb_ev_tail) {
			if (file->f_flags & O_NONBLOCK) {
				retval = -EAGAIN;
				break;
			}
			if (signal_pending (current)) {
				retval = -ERESTARTSYS;
				break;
			}
			schedule();
			continue;
		}
		
		for (ev = ikb_ev_tail; ev != ikb_ev_head && nbytes >= 2; ikb_ev_tail++, ikb_ev_tail &= 31, ev = ikb_ev_tail) {
			put_user (ikb_events[ev], (unsigned short *)buf);
			count += 2;
			buf += 2;
			nbytes -= 2;
		}

		break; /* only read as much as we have */
	}

	ikb_reading = 0;
	current->state = TASK_RUNNING;
	remove_wait_queue (&ikb_read_wait, &wait);

	return (count? count : retval);
}

static unsigned int ikb_poll (struct file *file, poll_table *wait) 
{
	unsigned int mask = 0;
	poll_wait (file, &ikb_read_wait, wait);
	if (ikb_ev_head != ikb_ev_tail)
		mask |= POLLIN | POLLRDNORM;
	return mask;
}

/*
 * We allow multiple opens, even though multiple readers will compete for events,
 * since usually one reader is in wait() for the other to complete.
 * (It's no worse than a bog-standard TTY device.)
 */
static int ikb_open (struct inode *inode, struct file *file) 
{
	MOD_INC_USE_COUNT;
	ikb_opened++;
	return 0;
}

static int ikb_release (struct inode *inode, struct file *file) 
{
	ikb_opened--;
	MOD_DEC_USE_COUNT;
	return 0;
}

static struct file_operations ikb_fops = {
	read:		ikb_read,
	poll:		ikb_poll,
	open:		ikb_open,
	release:	ikb_release,
};
static struct miscdevice ikb_misc = { MISC_DYNAMIC_MINOR, "wheel", &ikb_fops };

void clickwheel_int(void)
{
	/* Read wheel 
	 * Bits 6 and 7 of GPIOH change as follows:
	 * Clockwise rotation	01 -> 00 -> 10 -> 11
	 * Counter-clockwise	11 -> 10 -> 00 -> 01
	 *
	 * This is equivalent to wheel_value of:
	 * Clockwise rotation	0x40 -> 0x00 -> 0x80 -> 0xc0
	 * Counter-clockwise	0xc0 -> 0x80 -> 0x00 -> 0x40
	 */
	static const unsigned char wheel_tbl[2][4] =
	{
		/* 0x00  0x40  0x80  0xc0 */ /* Wheel value		*/
		{ 0x40, 0xc0, 0x00, 0x80 }, /* Clockwise rotation */
		{ 0x80, 0x00, 0xc0, 0x40 }, /* Counter-clockwise  */ 
	};

	unsigned int wheel_value;

	wheel_value = GPIOH_INPUT_VAL & 0xc0;
	GPIOH_INT_LEV = (GPIOH_INT_LEV & ~0xc0) | (wheel_value ^ 0xc0);
	GPIOH_INT_CLR = GPIOH_INT_STAT & 0xc0;

	if (!hold_button)
	{
		unsigned int btn = BUTTON_NONE;
		if (old_wheel_value == wheel_tbl[0][wheel_value >> 6])
		{
			btn = BUTTON_SCROLL_FWD;
			ikb_scroll (1);
		}
		else if (old_wheel_value == wheel_tbl[1][wheel_value >> 6])
		{
			btn = BUTTON_SCROLL_BACK;
			ikb_scroll (-1);
		}
	}

	old_wheel_value = wheel_value;
}

/* device buttons */
void button_int()
{	
	int newHold;
	unsigned char state;
	unsigned int newBtn;

	state = GPIOF_INPUT_VAL & 0xff;

	newHold = (state & 0x80) != 0;
	if(newHold != hold_button)
	{
		if(newHold)
		  ikb_handle_button (BTN_HOLD, 1);
		else
		  ikb_handle_button (BTN_HOLD, 0);

		hold_button = newHold;
	}


	if (!hold_button)
	{
		newBtn = int_btn ^ (state & 0x7f);
		if ((newBtn & 0x01) != 0)
			ikb_handle_button (BTN_REC, (state & 0x01) == 0);
		if ((newBtn & 0x02) != 0)
			ikb_handle_button (BTN_PLAY, (state & 0x02) == 0);
		if ((newBtn & 0x04) != 0)
			ikb_handle_button (BTN_NEXT, (state & 0x04) == 0);
		if ((newBtn & 0x08) != 0)
			ikb_handle_button (BTN_PREVIOUS, (state & 0x08) == 0);
		if ((newBtn & 0x10) != 0)
			ikb_handle_button (BTN_ACTION, (state & 0x10) == 0);
		if ((newBtn & 0x20) != 0)
			ikb_handle_button (BTN_MENU, (state & 0x20) == 0);
		if ((newBtn & 0x40) != 0)
			ikb_handle_button (BTN_POWER, (state & 0x40) == 0);
		int_btn = (state & 0x7f);
	}
	GPIOF_INT_LEV = (GPIOF_INT_LEV & ~0xff) | (state ^ 0xff);
	GPIOF_INT_CLR = GPIOF_INT_STAT;
}

void keyboard_int(int irq, void *dev_id, struct pt_regs *regs)
{

	ikb_start_buttons();

	if(regs != NULL)
		kbd_pt_regs = regs;

	if (GPIOF_INT_STAT & 0xff)
		button_int();
	if (GPIOH_INT_STAT & 0xc0)
		clickwheel_int();

	tasklet_schedule(&keyboard_tasklet);  
	ikb_finish_buttons();
}

void __init ipodkb_init_hw(void)
{
	/* Enable all buttons */
	GPIOF_OUTPUT_EN &= ~0xff;
	GPIOF_ENABLE |= 0xff;

	/* Scrollwheel light - enable control through GPIOG pin 7 and set timeout */
	GPIOG_OUTPUT_EN |= 0x80;
	GPIOG_ENABLE = 0x80;

	/* Mask these before performing init ... because init has possibly
		occurred before */
	GPIOF_INT_EN &= ~0xff;
	GPIOH_INT_EN &= ~0xc0;

	/* Get current tick before enabling button interrupts */
	last_wheel_usec = USEC_TIMER;
	last_wheel_post = last_wheel_usec;

	GPIOH_ENABLE |= 0xc0;
	GPIOH_OUTPUT_EN &= ~0xc0;

	/* Read initial buttons */
	button_int();
	GPIOF_INT_CLR = 0xff;

	/* Read initial wheel value (bit 6-7 of GPIOH) */
	old_wheel_value = GPIOH_INPUT_VAL & 0xc0;
	GPIOH_INT_LEV = (GPIOH_INT_LEV & ~0xc0) | (old_wheel_value ^ 0xc0);
	GPIOH_INT_CLR = 0xc0;

	if (request_irq(PP5020_GPIO1_IRQ, keyboard_int, SA_SHIRQ, "keyboard", KEYBOARD_DEV_ID)) {
		printk("ipoddkb: IRQ %d failed\n", PP5020_GPIO1_IRQ);
	}

	/* Enable button interrupts */
	GPIOF_INT_EN |= 0xff;
	GPIOH_INT_EN |= 0xc0;

	misc_register (&ikb_misc);
}

