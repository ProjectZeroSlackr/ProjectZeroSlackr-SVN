/*
 * cal.h
 *
 * CPU Abstraction Layer interface.
 */

/* $Id: cal.h,v 1.18 2001/02/27 04:19:18 nyef Exp $ */

#ifndef __CAL_H__
#define __CAL_H__

typedef struct cal_cpu *cal_cpu;

void cal_maratz80_init(cal_cpu *cpu);
void cal_junk68k_init(cal_cpu *cpu);
void cal_torr68k_init(cal_cpu *cpu);
void cal_cpu6280_init(cal_cpu *cpu);
void cal_cpu6502_init(cal_cpu *cpu);

#define CPUT_MARATZ80  cal_maratz80_init
#define CPUT_JUNK68K   cal_junk68k_init
#define CPUT_TORR68K   cal_torr68k_init
#define CPUT_CPU6280   cal_cpu6280_init
#define CPUT_CPU6502   cal_cpu6502_init

typedef unsigned char (*memread8_t)(cal_cpu, unsigned long);
typedef void (*memwrite8_t)(cal_cpu, unsigned long, unsigned char);
typedef unsigned short (*memread16_t)(cal_cpu, unsigned long);
typedef void (*memwrite16_t)(cal_cpu, unsigned long, unsigned short);
typedef unsigned long (*memread32_t)(cal_cpu, unsigned long);
typedef void (*memwrite32_t)(cal_cpu, unsigned long, unsigned long);

typedef struct {
    unsigned long start;
    unsigned long end;
    unsigned char *data;
    unsigned short flags;
    unsigned short tag;
} ranged_mmu;

typedef void (*cpureset_t)(cal_cpu);
typedef void (*cpurun_t)(cal_cpu);
typedef void (*cpurunfor_t)(cal_cpu, int);
typedef void (*cpuburnfor_t)(cal_cpu, int);
typedef void (*cpuirq_t)(cal_cpu, int);
typedef void (*cpunmi_t)(cal_cpu);
typedef void (*cpusetzpage_t)(cal_cpu, void *);
typedef int (*cputimeleft_t)(cal_cpu);
typedef void (*cpusetmmu0_t)(cal_cpu, ranged_mmu *);
typedef void (*cpusetmmu8_t)(cal_cpu, int, int, memread8_t *, memwrite8_t *);
typedef void (*cpusetmmu16_t)(cal_cpu, int, int, memread16_t *, memwrite16_t *);
typedef void (*cpusetmmu32_t)(cal_cpu, int, int, memread32_t *, memwrite32_t *);
typedef void (*cpusetiou_t)(cal_cpu, memread8_t, memwrite8_t);

struct cal_cpu {
    void *userdata;
    cpureset_t reset;
    cpurun_t run;
    cpurunfor_t runfor;
    cpuburnfor_t burn;
    cpuirq_t irq;
    cpunmi_t nmi;
    cpusetzpage_t setzpage;
    cputimeleft_t timeleft;
    cpusetmmu0_t setmmu0;
    cpusetmmu8_t setmmu8;
    cpusetmmu16_t setmmu16;
    cpusetmmu32_t setmmu32;
    cpusetiou_t setiou;
    union {
	struct M6502 *d_marat6502;
	struct emu6502_context *d_emu6502;
	struct emu6502_context *d_emu6502a;
	struct emu6502_context *d_emu65c02;
	struct emu6280_context *d_emu6280;
	struct Z80 *d_maratz80;
	struct emuz80_context *d_emuz80;
	struct emu68k_context *d_junk68k;
	struct M6280 *d_marat6280;
	struct cpu6280_context *d_cpu6280;
	struct cpu6502_context *d_cpu6502;
    } data;
};

void cal_event_delay_callback(void *, int);

typedef void (*cal_cpuinit_t)(cal_cpu *cpu);
cal_cpu cal_create(cal_cpuinit_t initfunc);

#endif /* __CAL_H__ */

/*
 * $Log: cal.h,v $
 * Revision 1.18  2001/02/27 04:19:18  nyef
 * removed useless include of cpudefs.h
 *
 * Revision 1.17  2001/02/27 04:14:09  nyef
 * added cal_event_delay_callback to support the new event.c interface
 *
 * Revision 1.16  2001/02/26 03:04:10  nyef
 * changed the interface for cal_create()
 * added some glue to allow system drivers to operate with the new cal_create()
 * interface without modification
 *
 * Revision 1.15  2000/05/01 00:36:40  nyef
 * added cpu6502 support
 *
 * Revision 1.14  2000/01/17 00:59:43  nyef
 * changed to always define cpu data pointers, even if the cpu isn't enabled
 *
 * Revision 1.13  2000/01/17 00:58:06  nyef
 * added cpu6280 support
 *
 * Revision 1.12  1999/11/08 01:31:11  nyef
 * added preliminary definition and interface for ranged mmu
 *
 * Revision 1.11  1999/08/29 13:06:00  nyef
 * updated to new IRQ interface
 *
 * Revision 1.10  1999/08/15 02:28:21  nyef
 * added TORR68K support
 *
 * Revision 1.9  1999/08/02 00:17:03  nyef
 * added marat6280 support
 * added conditional compilation of junk68k support
 *
 * Revision 1.8  1999/04/17 20:05:57  nyef
 * changed interface to allow 68k support.
 * added 68k support.
 *
 * Revision 1.7  1999/01/26 03:06:08  nyef
 * expanded "address bus" to 32 bits.
 *
 * Revision 1.6  1999/01/09 19:47:47  nyef
 * added 'void *userdata' to struct cal_cpu.
 *
 * Revision 1.5  1999/01/03 02:25:40  nyef
 * added maratz80 support.
 *
 * Revision 1.4  1999/01/02 06:35:46  nyef
 * added io interface for z80 support.
 *
 * Revision 1.3  1999/01/01 07:00:46  nyef
 * added emuz80 support
 *
 * Revision 1.2  1998/12/12 20:35:13  nyef
 * fixed marat6502 support.
 *
 * Revision 1.1  1998/11/29 18:28:34  nyef
 * Initial revision
 *
 */
