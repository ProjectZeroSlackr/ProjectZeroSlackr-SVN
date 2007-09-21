/*
  Little John GP32
  File : unes.h
  Authors : Yoyo.
  Version : 0.2
  Last update : 7th July 2003
*/


#ifndef _UNES_H_
#define _UNES_H_

#define FRAME_IRQ
//#define __testing__

#define __asmcpu__
//#define __fastkey__
//#define __fastkey2__
//#define __fastkey3__

/*#define debug_start_frame 30000
#define debug_scanline 30000
#define __debug_op__
#define __debug_io__*/


#include "datatypes.h"




#ifdef __asmcpu__
extern void cpu_pending_irq(void);
extern void cpu_irq(void);
extern void cpu_nmi(void);
extern int cpu_getcycles(bool b);
extern int cpu_exec(uint32 total_cycles);
extern void cpu_burn(uint32 burn_cycles);
extern void cpu_release(void);
extern void init_sram(uint8 *sram);
extern void reset_cpu(void);
extern void init_cpu(uint8 *nesram);
extern void set_cpu_bank_full(uint8 *bank0,uint8 *bank1,uint8 *bank2,uint8 *bank3);
extern void set_cpu_bank0(uint8 *bank0);
extern void set_cpu_bank1(uint8 *bank1);
extern void set_cpu_bank2(uint8 *bank2);
extern void set_cpu_bank3(uint8 *bank3);
extern void reload_fast_pc(void);

extern void get_context(uint32* nesregs);
extern void set_context(uint32* nesregs);
#endif

#ifdef __asmcpu__
#define doIRQ() \
 	cpu_irq();
#define PendingIRQ() \
	cpu_pending_irq();
#else
#define doIRQ() \
        nes6502_irq();
#define PendingIRQ() \
	nes6502_pending_irq();
#endif


extern void XlatNESBufferPAL (unsigned char *src, unsigned char *dest, uint8 *nesppupal);
extern void XlatNESBufferNTSC (unsigned char *src, unsigned char *dest, uint8 *nesppupal);
extern void XlatNESBufferLine (unsigned char *src, unsigned char *dest,uint8 *nesppupal, uint32 scanl);
extern void XlatNESBufferLineZ (unsigned char *src, unsigned char *dest,uint8 *nesppupal, uint32 scanl);
extern void XlatNESBufferLineZ16 (unsigned char *src, unsigned char *dest,uint8 *nesppupal, uint32 scanl);
extern void XlatNESBufferLineZ16B (unsigned char *src, unsigned char *dest,uint8 *nesppupal, uint32 scanl);
extern void XlatNESBufferLine16 (unsigned char *src, unsigned char *dest,uint8 *nesppupal, uint32 scanl);

extern void XlatNESBufferLineLazy (unsigned char *src, unsigned char *dest,uint8 *nesppupal, uint32 startscanl);
extern void XlatNESBufferLineZLazy (unsigned char *src, unsigned char *dest,uint8 *nesppupal, uint32 startscanl);

#include "nes6502.h"

#define AUTO_FSKIP 10

#define NBBUFFERS 16

// Palette structure
typedef struct __pal {
	int r;
	int g;
	int b;
} pal;

// .NES Rom file header structure
typedef struct 
{ 
	char id[4]; //should be a 'NES'+#0x1A  here
        char prg_rom_pages_nb;char chr_rom_pages_nb;
	char rom_ctrl1,rom_ctrl2;
	char dummy;  //should be a 0x00 here
} NES_file_header;

typedef struct {
	

	byte JoyPad1_BitIndex;
	byte JoyPad2_BitIndex;

	
	byte vrom;
	byte mirroring;
	
        byte DrawCframe;
	int  cart_size;		
	
	int currentcpucycle;
	int cpucycle;
	int deccycle;
	int curdeccycle;
	
	uint8 frame_irq_enabled;
	uint8 frame_irq_disenabled;
	
	
	char debugmode;
	char emupause;
	char fps;
	
	
	int MasterVolume;
	long snd_frequency;
	char InterPolateSnd;
	long audio_buffer_pos;
	Uint8 enablesound,Sound16bits,SoundStereo;
	int buffsize;		
		
	byte *Vbuffer;	
	byte SaveRAM;
	byte *chr_beg;
	byte *prg_beg;
		
	long CurrentCycle;	
	
	Uint8 *sndbuffer;
	Uint8 *sndbuffer_pos;
	Uint8 *sndbuffer_end;	
	
	byte   morethan8spr;
	uint8  DrawAllLines;
	uint8  scaleMode;
	uint8  vsync;
		
	uint8  padmode;
	int32  zapperX,zapperY;  
	uint8  autofireA;
	uint8  autofireB;
	
	uint32  crc;
	uint32  crc_all;
	
	char  country[8];
  	char  publisher[128];
  	char  date[16];
  	
  	uint8 ppu_mirror0;
  	uint8 ppu_mirror1;
  	uint8 ppu_mirror2;
  	uint8 ppu_mirror3;
  	
  	uint8 sramEnabled;
  	
  	uint8 nsfmode;
  	
  	uint8 sndfreq;
  	uint8 sndfilter;
  	
  	uint8 lazy_mode;
  	uint8 lazy_mode_avail;
} NesVar;



typedef struct { 			
	byte *CPUMemory; //64k
	byte *CPUPageIndex[8];  //8k pages pointers
	byte *NESRAM;	
	byte *NESSRAM;
	uint32 sramsize;
	byte *mapper_extram;
	uint32 mapper_extramsize;
		
	uint32 vramsize;
	
	
	byte *PPUPageIndex[12]; //[16];  //1k pages pointers
	bool PPUPageIndexProtect[12]; //[16];  //1k pages pointers
	byte *PPU_patterntables;	
	byte *PPU_palette;
	byte *PPU_nametables;	
	NES_file_header ROM_Header;
        uint32 Mapper_used;	

	byte *SPRMemory;
	byte *rom;		

	long romsize;
	int  trainer;
	NesVar var;	
} VirtualNES;


#define CONFIG_DATA_SIZE 512
typedef struct {	
	uint8 version_major;
	uint8 version_minor;
	uint8 enablesound;
	uint8 soundQuality;
	
	uint8 soundFilter;	
	uint8 fskiprate;
	uint8 fskiprate_adj_ratio;
	uint8 morethan8spr;
	
	uint8 showFPS;
	uint8 gamma;
	uint8 DrawAllLines;
	uint8 vsync;
	
	uint8 scaleMode;	
	uint8 autoFireA;
	uint8 autoFireB;	
	uint8 dum2;
	char  lastRom[256];
	char  dummy[512-256-16];
} config_data;

long JoyPad1_State(void);
long JoyPad2_State(void);

/*****************************************************
  Send an error msg
*****************************************************/
void SendErrorMsg(char *errormsg);

/*****************************************************
  Open, check and return memorycopy of a ROM file
*****************************************************/
char Load_ROM(char *RomName);
char Open_ROM(void);
char Init_NES(char *RomName);

/*****************************************************
  Close the ROM
*****************************************************/
char Close_ROM(int b);



int InitSound(void);
void InitDisplay(void);
void InitGUI(void);
Uint8 nes_main (int runcounter); //(int argc,char *argv[])
void GP_PauseAudio(Uint8 pause);
void gpwait(int l);

byte emulate_one_frame(void);

#endif

