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
extern void  flip_screen_();
void cop_lcd_test()
{
	while (inl(COP_STATUS) & COP_RUNNING)
	{
		while ((inl(COP_STATUS) & COP_LINE_REQ)==0); // wait for 1		
		flip_screen_();
//		while ((inl(COP_STATUS) & COP_LINE_REQ)!=0) // wait for 0
			outl(inl(COP_STATUS) &~ COP_LINE_REQ, COP_STATUS); // clear frameready
	}
		
}

#define CACHE_CTL        (*(volatile unsigned long *)(0x6000c000))

#define CACHE_DISABLE    0
void cop_begin()
{
	outl(inl(COP_STATUS) &~ COP_LINE_REQ2, COP_STATUS); // clear frameready
	cop_execute(cop_lcd_test);	
	outl(COP_RUNNING, COP_STATUS);

}
void cop_set(void (*function)())
{
}

void cop_end()
{
	outl(inl(COP_STATUS) &~ COP_RUNNING, COP_STATUS); // set to 0
	outl(inl(COP_STATUS) | COP_LINE_REQ, COP_STATUS);
}
