#ifndef __COP_H_
#define __COP_H_
/* borrowed from iboy */
int cop_execute(void (*function)());
void cop_begin(void (*function)());
void cop_set(void (*function)());
void cop_end();


#define DMA_BUFFER		0x40000040



#define COP_HANDLER 	0x4001501C
#define COP_STATUS 	0x40015020


/*
#define COP_HANDLER     0x40016FF0
#define COP_STATUS      0x40016FF4
*/

#define COP_RUNNING	(1 << 0)
#define COP_LINE_REQ	(1 << 1)
#define COP_LINE_REQ2	(2 << 1)



#endif
