#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <linux/kd.h>
#include <sys/signal.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <stdio.h>


#include "../../defs.h"
#include "../../input.h"
#include "../../hw.h"
#ifdef IPOD_COLOR
#include "../color_lcd/lcd.h"
#include "../color_lcd/lcdll.h"
#endif
#ifdef IPOD_4G
#include "../4g_lcd/lcd.h"
#include "../4g_lcd/lcdll.h"
#endif
#ifdef IPOD_MINI
#include "../4g_lcd/lcd.h"
#include "../4g_lcd/lcdll.h"
#endif

#include "../ipod/nix.h"
#include "../config.h"
#include "kb.h"

// the following arrays define the default keymapping

unsigned char push_map[5] = { 
	PAD_A,						//x01 = IPOD_CENTER
	PAD_B,						//x02 = IPOD_RIGHT
	PAD_B,						//x04 = IPOD_LEFT
	PAD_B,						//x08 = IPOD_DOWN
	PAD_B,						//x10 = IPOD_UP
};			 

unsigned char push_combo[32];

unsigned char touch_map[8] = {
	PAD_UP,
	PAD_A,
	PAD_RIGHT,
	PAD_START,
	PAD_DOWN,
	PAD_SELECT,
	PAD_LEFT,
	PAD_B
};


void kb_init()
{
	int i, bit, h;

#ifndef PORTABLE	
	int fd = open("/dev/console", O_NONBLOCK);
	if (fd < 0)
	{
		printf("attention: console still active!\n");
		sleep(20);
		/* not _deadly_ if we can't set graphics mode */
		return;
	}
	ioctl(fd, KDSETMODE, KD_GRAPHICS);
	close(fd);
#endif	
	
	// build a map for all possible combinations
	for(i=0; i<32; i++) {
		push_combo[i] = 0;
		h = 1;
		for(bit=0;bit<5;bit++,h<<=1)
			if(i&h) push_combo[i] |= push_map[bit];
	}	
	
}


void kb_close()
{
#ifdef KB_PORTABLE
	reset_input_mode();
#endif
		
#ifndef PORTABLE
	int fd = open("/dev/console", O_NONBLOCK);
	ioctl(fd, KDSETMODE, KD_TEXT);
	close(fd);
#endif
}


int kb_gethold()
{
	return !(inl(0x6000d030)&0x20); 
}



// FIXME: Do we really need my kb_getch?
#ifndef KB_PORTABLE
int kb_getch() {
	int in, button, st;
	int cur, cnt=5;
	static int last=-1;

begin:
	sys_sleep(15 * 1000);
	
	
	if(!(inl(0x6000d030)&0x20)) return 'h';
	in = inl(0x7000C140);
	
	st = ((in & 0xff000000)>>24);
	if(st != 0xc0 && st != 0x80) goto begin;
	
	button = (in & 0x00001F00) >> 8;

	switch(button) {
		case IPOD_UP:	  cur = 'm'; break;
		case IPOD_DOWN:	  cur = 'd'; break;
		case IPOD_LEFT:	  cur = 'l'; break;
		case IPOD_RIGHT:  cur = 'r'; break;
		case IPOD_CENTER: cur = '\n'; break;
		default: 
			last = -1;
			goto begin;
	}
	
	if(last == cur && cnt--) goto begin;
	
	last = cur;
	return cur;
}	
#endif
	

void enter_menu();


void kb_poll()
{
	int in, st;
	unsigned char touch, button;
	

	pad_release(0xFF); // release ALL


	in = inl(0x7000C140);
	if(in == 0) enter_menu();	// FIXME: hold - is there a better way?

	st = (in & 0xff000000) >> 24;
	if(st == 0xc0)
	{ 	// touched
		touch = (in & 0x007F0000 ) >> 16;
		touch+=6;
		touch/=12;
		if(touch>7) touch=0;
		pad_press(touch_map[touch]);
	}
	else if(st != 0x80) return; 
	// hold trashes the kb to 55555... or aaaaa...
	// until the next kb event happens so we have to return

	
/*	if(y_flip && (button & IPOD_DOWN)) {
		lcd_yflip();
		usleep(200000);
	}*/

	button = (in & 0x00001F00) >> 8;
	if(button) pad_press(push_combo[button]);	
}
