#include "cop.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>

#define outl(a, b) (*(volatile unsigned int *)(b) = (a))
#define inl(a) (*(volatile unsigned int *)(a))

int cop_execute(void (*function)())
{
		outl((unsigned int)function, COP_HANDLER);
		outl(0x0, 0x60007004);
		return 1;	
}
extern void  video_display_buffer_();
//void (*myfunction)();
void cop_lcd_test()
{
	while (inl(COP_STATUS) & COP_RUNNING)
	{
		while ((inl(COP_STATUS) & COP_LINE_REQ)==0); // wait for 1		
		//outl(1,0xf000f024);
/*		if ((inl(COP_STATUS) & COP_LINE_REQ2)!=0)
		{
			myfunction();		
			outl(inl(COP_STATUS) &~ COP_LINE_REQ2, COP_STATUS); // clear frameready
		}
		else*/
		video_display_buffer_();
		//while ((inl(COP_STATUS) & COP_LINE_REQ)==1)  // wait for 1		
		outl(inl(COP_STATUS) &~ COP_LINE_REQ, COP_STATUS); // clear frameready
	}
		
}

#define CACHE_CTL        (*(volatile unsigned long *)(0x6000c000))

#define CACHE_DISABLE    0
void cop_begin(void (*function)())
{
	//CACHE_CTL=0;
//	myfunction=function;
	outl(inl(COP_STATUS) &~ COP_LINE_REQ2, COP_STATUS); // clear frameready
	cop_execute(cop_lcd_test);	
	outl(COP_RUNNING, COP_STATUS);

}
void cop_set(void (*function)())
{
//	myfunction=function;
}
/*
int init_snd()
{	
    int tmp,result;
    sound_fd = open("/dev/dsp", O_WRONLY);
    tmp = AFMT_S16_LE;
    result = ioctl(sound_fd, SNDCTL_DSP_SETFMT, &tmp);
    tmp = 0;
    result = ioctl(sound_fd, SNDCTL_DSP_CHANNELS, &tmp);
    int sound_rate = 44100;
    result = ioctl(sound_fd, SNDCTL_DSP_SPEED, &sound_rate);
}
*/

void cop_end()
{
	outl(inl(COP_STATUS) &~ COP_RUNNING, COP_STATUS); // set to 0
	outl(inl(COP_STATUS) | COP_LINE_REQ, COP_STATUS);
}
