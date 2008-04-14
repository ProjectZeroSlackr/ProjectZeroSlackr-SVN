/** Z80: portable Z80 emulator *******************************/
/**                                                         **/
/**                           Z80.h                         **/
/**                                                         **/
/** This file contains declarations relevant to emulation   **/
/** of Z80 CPU.                                             **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1994,1995,1996,1997       **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/   
/**     changes to this file.                               **/
/*************************************************************/
#ifndef Z80_H
#define Z80_H

/*
 * this file has been modified for use with the cal.
 */

/* $Id: mz80.h,v 1.5 1999/11/08 01:34:09 nyef Exp $ */

#include "cal.h"
#include "mtypes.h"

                               /* Compilation options:       */
/* #define DEBUG */            /* Compile debugging version  */
/* #define LSB_FIRST */        /* Compile for low-endian CPU */
/* #define MSB_FIRST */        /* Compile for hi-endian CPU  */

                               /* LoopZ80() may return:      */
#define INT_IRQ     0x0038     /* Standard RST 38h interrupt */
#define INT_NMI     0x0066     /* Non-maskable interrupt     */
#define INT_NONE    0xFFFF     /* No interrupt required      */
#define INT_QUIT    0xFFFE     /* Exit the emulation         */

                               /* Bits in Z80 F register:    */
#define S_FLAG      0x80       /* 1: Result negative         */
#define Z_FLAG      0x40       /* 1: Result is zero          */
#define H_FLAG      0x10       /* 1: Halfcarry/Halfborrow    */
#define P_FLAG      0x04       /* 1: Result is even          */
#define V_FLAG      0x04       /* 1: Overflow occured        */
#define N_FLAG      0x02       /* 1: Subtraction occured     */
#define C_FLAG      0x01       /* 1: Carry/Borrow occured    */

/** Simple Datatypes *****************************************/
/** NOTICE: sizeof(byte)=1 and sizeof(word)=2               **/
/*************************************************************/
/* typedef unsigned char byte; */
/* typedef unsigned short word; */
/* typedef signed char offset; */

/** Structured Datatypes *************************************/
/** NOTICE: #define LSB_FIRST for machines where least      **/
/**         signifcant byte goes first.                     **/
/*************************************************************/
/* typedef union */
/* { */
/*   word W; */
/* #ifdef LSB_FIRST */
/*   struct { byte l,h; } B; */
/* #else */
/*   struct { byte h,l; } B; */
/* #endif */
/* } pair; */

typedef struct Z80
{
  pair AF,BC,DE,HL,IX,IY,PC,SP;       /* Main registers      */
  pair AF1,BC1,DE1,HL1;               /* Shadow registers    */
  byte IFF,I;                         /* Interrupt registers */

  int IPeriod,ICount; /* Set IPeriod to number of CPU cycles */
                      /* between calls to LoopZ80()          */
  int IBackup;        /* Private, don't touch                */
  word IRequest;      /* Set to address of pending IRQ       */
  cal_cpu User;         /* Arbitrary user data (ID,RAM*,etc.)  */
  byte TrapBadOps;    /* Set to 1 to warn of illegal opcodes */
  word Trap;          /* Set Trap to address to trace from   */
  byte Trace;         /* Set Trace=1 to start tracing        */
    memread8_t io_read;
    memwrite8_t io_write;
    memread8_t readfunc;
    memwrite8_t writefunc;
    ranged_mmu *mmu;
    unsigned char *cur_ip;
    unsigned short ip_left;
} Z80;

/** ResetZ80() ***********************************************/
/** This function can be used to reset the registers before **/
/** starting execution with RunZ80(). It sets registers to  **/
/** their initial values.                                   **/
/*************************************************************/
void ResetZ80(register Z80 *R);

/** ExecZ80() ************************************************/
/** This function will execute a single Z80 opcode. It will **/
/** then return next PC, and current register values in R.  **/
/*************************************************************/
word ExecZ80(register Z80 *R);

/** IntZ80() *************************************************/
/** This function will generate interrupt of given vector.  **/
/*************************************************************/
void IntZ80(register Z80 *R,register word Vector);

/** RunZ80() *************************************************/
/** This function will run Z80 code until an LoopZ80() call **/
/** returns INT_QUIT. It will return the PC at which        **/
/** emulation stopped, and current register values in R.    **/
/*************************************************************/
word RunZ80(register Z80 *R);

/** RdZ80()/WrZ80() ******************************************/
/** These functions are called when access to RAM occurs.   **/
/** They allow to control memory access.                    **/
/************************************ TO BE WRITTEN BY USER **/
/* void WrZ80(register word Addr,register byte Value); */
/* byte RdZ80(register word Addr); */

/** InZ80()/OutZ80() *****************************************/
/** Z80 emulation calls these functions to read/write from  **/
/** I/O ports. There can be 65536 I/O ports, but only first **/
/** 256 are usually used                                    **/
/************************************ TO BE WRITTEN BY USER **/
/* void OutZ80(register word Port,register byte Value); */
/* byte InZ80(register word Port); */
#define OutZ80(port, value) R->io_write(R->User, (port), (value))
#define InZ80(port) R->io_read(R->User, (port))

/** PatchZ80() ***********************************************/
/** Z80 emulation calls this function when it encounters a  **/
/** special patch command (ED FE) provided for user needs.  **/
/** For example, it can be called to emulate BIOS calls,    **/
/** such as disk and tape access. Replace it with an empty  **/
/** macro for no patching.                                  **/
/************************************ TO BE WRITTEN BY USER **/
/* void PatchZ80(register Z80 *R); */

/** DebugZ80() ***********************************************/
/** This function should exist if DEBUG is #defined. When   **/
/** Trace!=0, it is called after each command executed by   **/
/** the CPU, and given the Z80 registers. Emulation exits   **/
/** if DebugZ80() returns 0.                                **/
/*************************************************************/
/* byte DebugZ80(register Z80 *R); */

/** LoopZ80() ************************************************/
/** Z80 emulation calls this function periodically to check **/
/** if the system hardware requires any interrupts. This    **/
/** function must return an address of the interrupt vector **/
/** (0x0038, 0x0066, etc.) or INT_NONE for no interrupt.    **/
/** Return INT_QUIT to exit the emulation loop.             **/
/************************************ TO BE WRITTEN BY USER **/
/* word LoopZ80(register Z80 *R); */

#endif /* Z80_H */

/*
 * $Log: mz80.h,v $
 * Revision 1.5  1999/11/08 01:34:09  nyef
 * added preliminary implementation of ranged mmu
 *
 * Revision 1.4  1999/08/07 16:19:14  nyef
 * removed definition of LSB_FIRST
 *
 * Revision 1.3  1999/06/06 17:50:09  nyef
 * changed memory access interface to be faster
 *
 * Revision 1.2  1999/04/17 20:08:17  nyef
 * fixed for new version of the CAL.
 *
 * Revision 1.1  1999/01/03 02:24:50  nyef
 * Initial revision
 *
 */
