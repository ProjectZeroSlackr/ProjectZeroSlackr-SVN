/*
 * cal.c
 *
 * CPU Abstraction Layer implimentation.
 */

/* $Id: cal.c,v 1.30 2001/02/27 04:14:00 nyef Exp $ */

#include <stdlib.h>
#include "cal.h"
#include "ui.h"

cal_cpu cal_create(cal_cpuinit_t initfunc)
{
    cal_cpu retval;

    retval = calloc(1, sizeof(struct cal_cpu));

    if (retval) {
	initfunc(&retval);
    } else {
	deb_printf("Insufficient memory to create CPU.\n");
    }

    return retval;
}

void cal_event_delay_callback(void *data, int cycles)
{
    register cal_cpu cpu = data;

    cpu->runfor(cpu, cycles);
    cpu->run(cpu);
}

/*
 * $Log: cal.c,v $
 * Revision 1.30  2001/02/27 04:14:00  nyef
 * added cal_event_delay_callback to support the new event.c interface
 *
 * Revision 1.29  2001/02/26 03:03:12  nyef
 * changed the interface for cal_create()
 *
 * Revision 1.28  2001/02/25 22:10:29  nyef
 * removed some now-useless includes
 *
 * Revision 1.27  2001/02/25 22:05:20  nyef
 * moved junk68k CAL interface code out to emu68k.c
 *
 * Revision 1.26  2001/02/25 21:47:33  nyef
 * moved mz80 CAL interface code out to mz80.c
 *
 * Revision 1.25  2000/11/29 05:45:37  nyef
 * removed all marat6502 support
 *
 * Revision 1.24  2000/08/26 01:07:13  nyef
 * removed all marat6280 support
 *
 * Revision 1.23  2000/05/01 00:36:54  nyef
 * added cpu6502 support
 *
 * Revision 1.22  2000/04/23 20:15:06  nyef
 * made junk68k (emu68k) core use the torr68k code for IRQ generation
 *
 * Revision 1.21  2000/01/24 05:04:58  nyef
 * added CPU6280 support
 *
 * Revision 1.20  2000/01/14 04:41:35  nyef
 * changed junk68k (emu68k) code to use cycle counting
 *
 * Revision 1.19  1999/12/04 04:59:44  nyef
 * cleared out all code relating to the ASM CPU cores
 *
 * Revision 1.18  1999/11/21 04:39:51  nyef
 * added implementation of ranged mmu for marat 6502 core
 *
 * Revision 1.17  1999/11/21 04:26:29  nyef
 * changed cal_create() to use loop and collection idiom
 *
 * Revision 1.16  1999/11/08 01:31:42  nyef
 * added implementation of ranged mmu for marat z80 core
 *
 * Revision 1.15  1999/08/29 13:05:56  nyef
 * updated to new IRQ interface
 *
 * Revision 1.14  1999/08/15 02:28:36  nyef
 * added TORR68K support
 *
 * Revision 1.13  1999/08/02 00:16:30  nyef
 * added marat6280 support
 * added conditional compilation of junk68k support
 *
 * Revision 1.12  1999/06/06 17:50:22  nyef
 * changed maratz80 memory access interface to be faster
 *
 * Revision 1.11  1999/04/17 20:06:10  nyef
 * added 68k support
 *
 * Revision 1.10  1999/01/24 15:58:33  nyef
 * changed all malloc(x) to calloc(1, x).
 *
 * Revision 1.9  1999/01/17 02:10:27  nyef
 * fixed support for irqs in the emu6280 core
 *
 * Revision 1.8  1999/01/13 03:52:23  nyef
 * continued preliminary work on 6280 support.
 *
 * Revision 1.7  1999/01/03 02:25:32  nyef
 * added maratz80 support.
 *
 * Revision 1.6  1999/01/02 06:37:43  nyef
 * fixed stupid mistake with z80 setiou.
 *
 * Revision 1.5  1999/01/02 06:35:40  nyef
 * added io interface for z80 support.
 *
 * Revision 1.4  1999/01/01 07:00:59  nyef
 * added emuz80 support
 *
 * Revision 1.3  1998/12/12 21:34:37  nyef
 * brought remaining cpu interfaces up to spec
 *
 * Revision 1.2  1998/12/12 20:34:22  nyef
 * brought marat6502 interface up to spec.
 *
 * Revision 1.1  1998/11/29 18:28:44  nyef
 * Initial revision
 *
 */
