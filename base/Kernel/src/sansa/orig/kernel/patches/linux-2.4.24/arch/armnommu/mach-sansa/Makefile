#
# Makefile for the linux kernel.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).

USE_STANDARD_AS_RULE := true

O_TARGET		:= sansa.o

# Object file lists.

obj-y += arch.o cop.o dma.o hardware.o irq.o time.o lcd-as-memframe.o sd.o battery.o rtc.o


obj-$(CONFIG_AUDIO_SANSA)	+= audio.o
obj-$(CONFIG_FB_SANSA)		+= fb.o
obj-$(CONFIG_KB_SANSA)		+= keyboard.o

export-objs		:= audio.o fb.o keyboard.o hardware.o 

include $(TOPDIR)/Rules.make
