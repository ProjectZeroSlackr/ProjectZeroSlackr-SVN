#
# Makefile for DarcNES
#

# $Id: Makefile,v 1.82 2001/02/27 04:17:37 nyef Exp $

#TARGET?=Linux_svgalib
TARGET?=Linux_X
#TARGET?=DJGPP
#TARGET?=Linux_GTK

C_ONLY=-DC_ONLY # -DCROSS_COMPILING

MACH_TYPES=-DMACH_TYPES=\"types-i386.h\"

#BYTE_ORDER=-DMSB_FIRST
BYTE_ORDER=-DLSB_FIRST

#DEBUG=-g

OPTFLAGS=-O3  -fomit-frame-pointer -fstrength-reduce -funroll-loops -fexpensive-optimizations -finline-functions

BASE_CFLAGS=-Wall $(DEBUG) $(OPTFLAGS) $(C_ONLY) $(BYTE_ORDER) $(MACH_TYPES)
BASE_SFLAGS=-Wall $(DEBUG) $(OPTFLAGS)
BASE_LDFLAGS=-Wl,--warn-common

#
# Flags for Linux_svgalib
#
ifeq ($(TARGET),Linux_svgalib)
CONFFLAGS=-DPCE_CD_SUPPORT -DSOUND
SYSTEMSRCS=video_svga.c ui_svga.c snd_unix.c cd_unix.c
BINFILE=sdarcnes
LIBS=-lvgagl -lvga
endif

#
# Flags for Linux_X
#
ifeq ($(TARGET),Linux_X)
CONFFLAGS=-DSOUND
LDFLAGS+=-L../ttk/libs/SDL
CFLAGS+=-I/usr/include/SDL -elf2flt
SFLAGS+=-I/usr/X11R6/include
SYSTEMSRCS=video_SDL.c ui_SDL.c snd_unix.c cop.c tiledraw.S include.S ncpu.S
BINFILE=iDarcNES
LIBS=-L../hotdog/ipod -lhotdog -Wl,-elf2flt
endif

#
# Flags for Linux_GTK
#
ifeq ($(TARGET),Linux_GTK)
GTK_CONFIG?=gtk-config
CONFFLAGS=-DPCE_CD_SUPPORT -DSOUND
CFLAGS+=`$(GTK_CONFIG) --cflags`
SYSTEMSRCS=ui_gtk.c snd_unix.c cd_unix.c
BINFILE=darcnes
LIBS=`$(GTK_CONFIG) --libs`
endif

#
# Flags for DJGPP
#
ifeq ($(TARGET),DJGPP)
CONFFLAGS=-DSOUND
CFLAGS+=-DASM_UNDERBARS
SFLAGS+=-DASM_UNDERBARS
SYSTEMSRCS=video_allegro.c ui_allegro.c snd_allegro.c
BINFILE=darcnes.exe
LIBS=-lalleg
endif

#
# finalize FLAGS
#
CFLAGS+=$(BASE_CFLAGS) $(CONFFLAGS)
LCFLAGS=$(BASE_CFLAGS) $(CONFFLAGS)
SFLAGS+=$(BASE_SFLAGS)
LDFLAGS+=$(BASE_LDFLAGS)

#
# fixup command variables
#
LOCAL_CC?=gcc
LOCAL_CXX?=g++
LOCAL_AS?=gcc
LOCAL_LD?=gcc

#LOCAL_CC?=arm-elf-gcc
#LOCAL_CXX?=arm-elf-g++
#LOCAL_AS?=arm-elf-gcc
#LOCAL_LD?=arm-elf-gcc
CC=$(LOCAL_CC)
CXX=$(LOCAL_CC)
AS=$(LOCAL_AS)
LD=$(LOCAL_LD)

ifndef CROSS_COMPILING
TARGET_CC=arm-elf-gcc
TARGET_CXX=arm-elf-g++
TARGET_AS=arm-elf-gcc
TARGET_LD=arm-elf-gcc
else
TARGET_CC=gcc
TARGET_CXX=g++
TARGET_AS=gcc
TARGET_LD=gcc
endif

#
# file lists
#
NES_SRCS=mappers.c nes_ppu.c nes.c nes_psg.c fds.c
PCE_SRCS=pce.c pce_vdp.c pce_cd.c
SMS_SRCS=sms.c sms9918.c sms_psg.c
CV_SRCS=coleco.c tms9918.c sms_psg.c
SG1K_SRCS=sg1000.c tms9918.c sms_psg.c
GEN_SRCS=genesis.c gen_vdp.c
A2_SRCS=apple2.c apple2_vdp.c apple2_disk.c
MSX_SRCS=msx.c tms9918.c

GAMESRCS := $(NES_SRCS) $(PCE_SRCS) $(SMS_SRCS) $(CV_SRCS) $(SG1K_SRCS) \
	$(A2_SRCS) $(MSX_SRCS) #$(GEN_SRCS)
GAMESRCS := $(sort $(GAMESRCS))

ifdef C_ONLY
ASMSRCS=
CSRCS=blit_c.c
else
ASMSRCS=blitters.S
CSRCS=
endif

MZ80_SRCS=mz80.c
CPU6502_SRCS=cpu6502.c cpu6502opc.c
CPU6280_SRCS=cpu6280.c cpu6280opc.c
EMU68K_SRCS=emu68k.c torr_cpu.c

CPUSRCS := $(MZ80_SRCS) $(CPU6502_SRCS) $(CPU6280_SRCS) #$(EMU68K_SRCS)
CPUSRCS := $(sort $(CPUSRCS))

COMMONSRCS=tool.c cal.c event.c palette8.c 

SRCFILES=$(COMMONSRCS) $(GAMESRCS) $(ASMSRCS) $(CSRCS) $(SYSTEMSRCS) $(CPUSRCS)

# .c files that should not be auto-depended
NODEPSRCS=palette8.c

# .c files not in SRCFILES that should be auto-depended
DEPSRCS=

# extra files to delete while cleaning up
RMFILES=

# non-*.[chS] files to include in distribution
#MISC_FILES=Makefile notes ppu_notes readme sound_notes timing.txt
MISC_FILES=Makefile readme files.txt tests/*[0-9]

# object file list
OBJFILES := \
    $(patsubst %.c,%.o,$(filter %.c,$(SRCFILES))) \
    $(patsubst %.cpp,%.o,$(filter %.cpp,$(SRCFILES))) \
    $(patsubst %.S,%.o,$(filter %.S,$(SRCFILES)))

# make rules

all:	default

default: $(BINFILE)

$(BINFILE): $(OBJFILES)
	$(TARGET_LD) $(LDFLAGS) -o $(BINFILE) $(OBJFILES) $(LIBS)

# stupid GNU make limitations require specifying .S dependancies explicitly
blitters.o: blitters.S machdep.h

DEPSRCS += test_68k.c test_6280.c test_6502.c

#.PHONY: tests
#tests: run_test_68k run_test_6280 run_test_6502


# special handling for generic palette code

# FIXME: how to add MACH_TYPES to dependancies on following line?
palette8.o: palette.c palette.h types.h
	$(TARGET_CC) -o $@ $(CFLAGS) -DPALETTE_DEPTH=8 -c $<

palette16.o: palette.c palette.h types.h
	$(TARGET_CC) -o $@ $(CFLAGS) -DPALETTE_DEPTH=16 -c $<

palette32.o: palette.c palette.h types.h
	$(TARGET_CC) -o $@ $(CFLAGS) -DPALETTE_DEPTH=32 -c $<

# special handling for 68000 core

#.PHONY: run_test_68k
#run_test_68k: test_68k
#	cat tests/68k_test_*[0-9] | ./test_68k
#
#RMFILES += test_68k
#test_68k: test_68k.o emu68k.o
#	$(LD) $(LDFLAGS) -o test_68k test_68k.o emu68k.o


# special handling for 6280 core

#.PHONY: run_test_6280
#run_test_6280: test_6280
#	cat tests/6280_test_*[0-9] | ./test_6280
#
#RMFILES += test_6280
#test_6280: test_6280.o cpu6280.o cpu6280opc.o
#	$(LD) $(LDFLAGS) -o test_6280 test_6280.o cpu6280.o cpu6280opc.o
#
#NODEPSRCS += cpu6280opc.c
#RMFILES += cpu6280opc.c
#cpu6280opc.o: cpu6280opc.c cpu6280int.h
#
#cpu6280opc.c: build_cpu6280
#	./build_cpu6280 cpu6280opc.c
#
#DEPSRCS += build_cpu6280.c
#
#build_cpu6280.o: build_cpu6280.c
#	$(CC) -c $(LCFLAGS) $<
#
#RMFILES += build_cpu6280
#build_cpu6280: build_cpu6280.o
#	$(CC) $(LCFLAGS) -o build_cpu6280 build_cpu6280.o


# special handling for 6502 core

#.PHONY: run_test_6502
#run_test_6502: test_6502
#	cat tests/6502_test_*[0-9] | ./test_6502
#
#RMFILES += test_6502
#test_6502: test_6502.o cpu6502.o cpu6502opc.o
#	$(LD) $(LDFLAGS) -o test_6502 test_6502.o cpu6502.o cpu6502opc.o
#
#NODEPSRCS += cpu6502opc.c
#RMFILES += cpu6502opc.c
#cpu6502opc.o: cpu6502opc.c cpu6502int.h
#
#cpu6502opc.c: build_cpu6502
#	./build_cpu6502 cpu6502opc.c
#
#DEPSRCS += build_cpu6502.c
#
#build_cpu6502.o: build_cpu6502.c
#	$(CC) -c $(LCFLAGS) $<
#
#RMFILES += build_cpu6502
#build_cpu6502: build_cpu6502.o
#	$(CC) $(LCFLAGS) -o build_cpu6502 build_cpu6502.o


.PHONY: clean
clean:
	rm -f *.o *.d *~ "#"*"#" $(BINFILE) $(RMFILES) iDarcNES.gdb

.PHONY: distr
distr: clean
	(cd ..; tar cvzf darcnes/darcnes.tgz $(addprefix darcnes/,*.[chS] $(MISC_FILES)))
	grep -h \$$Id Makefile *.[chS] | grep -v darcnes.ids -> darcnes.ids

.c.o:	
	$(TARGET_CC) -c $(CFLAGS) $<

.c.s:	
	$(TARGET_CC) -S $(CFLAGS) $<

.cpp.o:	
	$(TARGET_CXX) -c $(CFLAGS) $<

.cpp.s:	
	$(TARGET_CXX) -S $(CFLAGS) $<

.S.o:
	$(TARGET_AS) -c $(SFLAGS) $<

.s.o:
	$(TARGET_AS) -c $(SFLAGS) $(addsuffix .S,$(basename $<))

#
# Dependancy data
#
DEPSRCS += $(SRCFILES)
DEPFILES := $(patsubst %.c,%.d,$(filter %.c,$(filter-out $(NODEPSRCS),$(DEPSRCS))))
DEPFILES += $(patsubst %.cpp,%.d,$(filter %.cpp,$(filter-out $(NODEPSRCS),$(DEPSRCS))))

# include depend files unles we are making clean
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distr)
#include $(DEPFILES)
endif
endif

#RMFILES+=fixdep
#NODEPSRCS+=fixdep.c
#fixdep: fixdep.c
#	$(CC) -o $@ $(LCFLAGS) $<
#
#%.d: %.cpp fixdep
#	$(TARGET_CXX) $(CFLAGS) -MM $< | ./fixdep > $@
#
#%.d: %.c fixdep
#	$(TARGET_CC) $(CFLAGS) -MM $< | ./fixdep > $@

#
# $Log: Makefile,v $
# Revision 1.82  2001/02/27 04:17:37  nyef
# removed 68k core and genesis driver sources from the compile lists
#
# Revision 1.81  2000/11/29 05:44:16  nyef
# removed M6502_SRCS from CPUSRCS
# removed the definition of M6502_SRCS
#
# Revision 1.80  2000/11/25 15:03:18  nyef
# added new MSX_SRCS group to the GAMESRCS list
#
# Revision 1.79  2000/10/30 22:18:04  nyef
# enabled warnings for multiple definitions of the same common section
#
# Revision 1.78  2000/08/04 23:34:42  nyef
# added support for the generic palette system
# added the 8-bit generic palette code to the common compile lists
#
# Revision 1.77  2000/08/04 21:26:03  nyef
# fixed to use $(CC) instead of $(TARGET_CC) for making build_cpu6{502,280}
#
# Revision 1.76  2000/07/09 20:49:23  nyef
# tweaked definition of LDFLAGS for the Linux_X version to allow for overrides
#
# Revision 1.75  2000/06/28 01:34:42  nyef
# added fds.c to the NES compile list
#
# Revision 1.74  2000/06/28 00:36:54  nyef
# fixed to work with GNU make 3.77 on BeOS (hopefully other platforms too)
#
# Revision 1.73  2000/06/25 15:23:47  nyef
# fixed default GTK_CONFIG to be gtk-config
#
# Revision 1.72  2000/06/03 23:01:04  nyef
# tweaked GTK target to allow specifying a different version of gtk_config
#
# Revision 1.71  2000/05/07 00:01:02  nyef
# Cleaned up a lot of things
# Added support for compiling and auto-depending C++ files
# Moved sound control out from #define SOUND in {sms,nes}_psg.c
# Added support for cross compilation
#
# Revision 1.70  2000/05/01 00:35:23  nyef
# added build process for cpu6502 core
# added cpu6502.c and cpu6502opc.c to the compile lists
#
# Revision 1.69  2000/04/15 02:14:06  nyef
# fixed to not rebuild the depend files when making clean
#
# Revision 1.68  2000/04/05 02:23:04  nyef
# added fixdep to the "files to remove when cleaning up" list
#
# Revision 1.67  2000/04/05 02:14:11  nyef
# fixed to not clobber CFLAGS inherited from the environment
#
# Revision 1.66  2000/03/25 18:18:45  nyef
# fixed a couple mistakes with the "clean" target
#
# Revision 1.65  2000/03/25 06:33:01  nyef
# general cleanup
# rebuilt auto-depend system (GNU make sucks sometimes)
# fixed autodep on CPU test and generation code
#
# Revision 1.64  2000/03/20 03:37:26  nyef
# made automatic dependancy generation fail to suck
# broke automatic dependancy generation for test drivers
# cleaned up the massive duplication involved in maintaining separate lists
#     of source and object files
# cleaned up system-dependant file lists
#
# Revision 1.63  2000/03/18 17:54:42  nyef
# fixed automatic dependancy problem with test driver code
#
# Revision 1.62  2000/03/11 02:30:17  nyef
# added apple2_disk.[co] to the compile lists
#
# Revision 1.61  2000/03/10 01:49:14  nyef
# added event.[co] to the compile lists
#
# Revision 1.60  2000/03/06 00:38:51  nyef
# added keyboard support to X version
#
# Revision 1.59  2000/02/14 02:36:58  nyef
# changed to use new unified cd_unix.c interface
#
# Revision 1.58  2000/02/12 19:04:24  nyef
# added PCE CD support
#
# Revision 1.57  2000/02/06 22:30:19  nyef
# re-added -g debug option
#
# Revision 1.56  2000/02/01 04:12:01  nyef
# added test data files to distribution
#
# Revision 1.55  2000/01/29 03:09:50  nyef
# changed dist target to put files in the .tgz file in a darcnes/ directory
#
# Revision 1.54  2000/01/24 05:07:24  nyef
# removed m6280.[co] from the compile lists
# added cpu6280.[co] and cpu6280opc.o to the compile lists
#
# Revision 1.53  2000/01/21 03:21:25  nyef
# fixed depends for cpu6280opc.o
#
# Revision 1.52  2000/01/17 01:24:59  nyef
# added test code for cpu6280 core
# added build process for cpu6280 core
#
# Revision 1.51  2000/01/09 18:23:46  nyef
# added a target to run the unit tests automatically
#
# Revision 1.50  2000/01/01 04:13:06  nyef
# added apple2.c and apple2_vdp.c to the compile lists
#
# Revision 1.49  1999/12/24 16:53:04  nyef
# added include directory for XWindows on FreeBSD
#
# Revision 1.48  1999/12/12 18:20:31  nyef
# added a target for test_68k
#
# Revision 1.47  1999/12/07 02:01:51  nyef
# added the MACH_TYPES silliness (too bad it's the best solution I found)
#
# Revision 1.46  1999/12/04 04:57:41  nyef
# removed debug.c, dt6502.c, dt65c02.c and dt6280.c from the compile lists
#
# Revision 1.45  1999/12/04 04:05:59  nyef
# removed all ASM CPU cores from the compile lists
#
# Revision 1.44  1999/11/27 20:08:10  nyef
# added sg1000.[co] to the compile lists
#
# Revision 1.43  1999/11/23 01:41:59  nyef
# added tiledraw.c to the compile lists
#
# Revision 1.42  1999/11/14 01:30:25  nyef
# added a .c.s rule to make it easier to check the compiler's behavior
#
# Revision 1.41  1999/10/31 02:38:24  nyef
# added snd_allegro.c and snd_unix.c to the compile lists
#
# Revision 1.40  1999/08/23 01:44:37  nyef
# removed -g debug option
#
# Revision 1.39  1999/08/15 02:27:20  nyef
# added C_ONLY support
# added partial support for new "Torrigion" CPU core
# added BYTE_ORDER support
#
# Revision 1.38  1999/08/07 16:20:03  nyef
# added -DLSB_FIRST to CFLAGS in all system types
#
# Revision 1.37  1999/08/02 00:21:36  nyef
# added m6280.[co] to the compile lists
#
# Revision 1.36  1999/06/14 19:16:56  nyef
# removed (unused) GRAPHICS options
# added OPTFLAGS (optimization), with -fomit-frame-pointer
#
# Revision 1.35  1999/06/08 01:49:56  nyef
# added coleco.[co] to the compile lists
# added tms9918.[co] to the compile lists
#
# Revision 1.34  1999/04/28 16:49:22  nyef
# added sms_psg.[co] to the compile lists
#
# Revision 1.33  1999/04/17 23:04:29  nyef
# added genesis.[co], gen_vdp.[co], and emu68k.[co] to compile lists.
#
# Revision 1.32  1999/02/21 23:32:52  nyef
# switched from using blitters.c to blitter.S
#
# Revision 1.31  1999/02/15 03:40:50  nyef
# added patch from Michael Vance for GTK support
#
# Revision 1.30  1999/02/14 18:34:35  nyef
# removed io_nes.[So] from the compile lists
#
# Revision 1.29  1999/01/22 03:04:16  nyef
# added files.txt to the MISC_FILES list.
#
# Revision 1.28  1999/01/18 22:16:58  nyef
# added changes required for SVGALib support
#
# Revision 1.27  1999/01/17 02:13:13  nyef
# added pce_vdp.[co] to the compile lists.
#
# Revision 1.26  1999/01/13 04:15:41  nyef
# removed io_pce.[So] from the compile lists.
#
# Revision 1.25  1999/01/11 02:24:02  nyef
# removed nes_blit.[co] from compile lists.
# added blitters.[co] to compile lists.
#
# Revision 1.24  1999/01/08 02:45:46  nyef
# removed zarzon code from compile lists.
#
# Revision 1.23  1999/01/07 03:16:55  nyef
# added sms9918.[co] to the compile lists.
#
# Revision 1.22  1999/01/03 02:29:51  nyef
# added maratz80.[co] to the compile lists.
#
# Revision 1.21  1999/01/01 07:01:38  nyef
# added sms.c, emuz80.S and opcz80.S to compile lists.
#
# Revision 1.20  1998/12/31 19:30:45  nyef
# added some admin stuff to make distr.
#
# Revision 1.19  1998/12/24 04:33:46  nyef
# split all of the game-specific files out of the common compile lists.
# added zarzon.[co] to the game compile lists.
#
# Revision 1.18  1998/12/20 04:03:37  nyef
# changed header comment and all binfiles to say "darcnes".
#
# Revision 1.17  1998/12/20 04:01:54  nyef
# removed a few files from MISC_FILES.
#
# Revision 1.16  1998/12/12 21:38:50  nyef
# added m6502.c to compile lists
#
# Revision 1.15  1998/11/29 18:29:20  nyef
# added cal.c to the compile lists.
#
# Revision 1.14  1998/10/21 02:38:58  nyef
# added targets for 'realclean' and 'distr'
#
# Revision 1.13  1998/10/11 21:37:02  nyef
# added {emu,opc}6502a.[So] the the common compile lists.
#
# Revision 1.12  1998/08/22 00:25:46  nyef
# added nes_psg.[co] to the common compile lists.
#
# Revision 1.11  1998/08/02 16:52:59  nyef
# added ui_allegro.[co] to DJGPP{SRC,OBJ}S.
#
# Revision 1.10  1998/08/01 22:31:02  nyef
# added ui_x.[co] to the LINUX_X{SRC,OBJ}S lines.
#
# Revision 1.9  1998/08/01 01:15:10  nyef
# added a bunch of libs to the Linux_X configuration for GUI support.
#
# Revision 1.8  1998/07/31 23:52:47  nyef
# added .s.o rule for DJGPP support.
#
# Revision 1.7  1998/07/29 01:39:42  nyef
# added ASM_UNDERBARS to [CS]FLAGS for DJGPP.
# removed commented out commands on [CS]FLAGS for Linux/Svgalib.
# added -lalleg to LIBS line for DJGPP.
#
# Revision 1.6  1998/07/28 23:48:18  nyef
# changed all references to '.depend' to '_depend' for DOS compatability.
#
# Revision 1.5  1998/07/25 16:58:04  nyef
# added the blit compiler to the common compile lists.
#
# Revision 1.4  1998/07/14 02:08:59  nyef
# added opc6280.S to the compile lists
#
# Revision 1.3  1998/07/14 01:50:30  nyef
# added dt6280.c to the file lists.
#
# Revision 1.2  1998/07/12 20:51:17  nyef
# re-added pce-specific files to compile lists
#
# Revision 1.1  1998/07/11 22:16:01  nyef
# Initial revision
#
#
