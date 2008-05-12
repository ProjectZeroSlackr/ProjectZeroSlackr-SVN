#include "cop.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

extern void  flip_screen_();

int cop_execute(void (*function)())
{
	outl((unsigned int)function, COP_HANDLER);
	outl(0x0, 0x60007004);
	return 1;
}

void cop_lcd_test()
{
	while (inl(COP_STATUS) & COP_RUNNING)
	{
		while ((inl(COP_STATUS) & COP_LINE_REQ) == 0);
		flip_screen_();
		outl(inl(COP_STATUS) &~ COP_LINE_REQ, COP_STATUS); // clear frameready
	}	
}

void cop_begin()
{
	outl(inl(COP_STATUS) &~ COP_LINE_REQ2, COP_STATUS); // clear frameready
	cop_execute(cop_lcd_test);	
	outl(COP_RUNNING, COP_STATUS);
}

void cop_end()
{
	outl(inl(COP_STATUS) &~ COP_RUNNING, COP_STATUS); // set to 0
	outl(inl(COP_STATUS) | COP_LINE_REQ, COP_STATUS);
}
