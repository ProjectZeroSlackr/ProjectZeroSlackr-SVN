/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id: lcd-as-memframe.S 15254 2007-10-21 23:12:17Z kkurbjun $
 *
 * Copyright (C) 2007 by Michael Sevakis
 *
 * ARM code for memory framebuffer LCDs
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/

//#include "config.h"
//#include "cpu.h"

#define LCD_WIDTH 176


/****************************************************************************
 * void lcd_copy_buffer_rect(fb_data *dst, fb_data *src, int width,
 *                           int height);
 */
     .section    .text, "ax", %progbits
     .align      2
     .global     lcd_copy_buffer_rect
     .type       lcd_copy_buffer_rect, %function
                                        @ r0 = dst
                                        @ r1 = src
                                        @ r2 = width
                                        @ r3 = height
lcd_copy_buffer_rect:                   @
    stmfd   sp!, { r4-r12, lr }         @ save non-scratch regs
    mov     r5, r2                      @ r5 = cached width
    rsb     r4, r2, #LCD_WIDTH          @ r4 = LCD_WIDTH - width
10: @ copy line                         @
    subs    r2, r5, #1                  @ r2 = width - 1
    beq     40f @ finish line           @ one halfword? skip to trailing copy
    tst     r0, #2                      @ word aligned?
    beq     20f @ rem copy              @ yes? skip to word copy
    ldrh    r6, [r1], #2                @ copy leading halfword
    subs    r2, r2, #1                  @
    strh    r6, [r0], #2                @
    ble     40f @ finish line           @ next line if lt or finish
                                        @ trailing halfword if eq
20: @ rem copy                          @
    add     r14, r2, #1                 @ get remaining width mod 16 after word
                                        @ align (rw)
    and     r14, r14, #0xe              @ r14 = 0 (16), 2, 4, 6, 8, 10, 12, 14
    add     pc, pc, r14, lsl #3         @ branch to 32-byte align
    nop                                 @ 
    b       30f                         @ rw % 16 = 0 or 1? use octword loop
    nop                                 @
    nop                                 @
    nop                                 @
    ldr     r6, [r1], #4                @ rw % 16 = 2 or 3
    subs    r2, r2, #2                  @
    str     r6, [r0], #4                @
    b       25f @ copy up done          @
    ldmia   r1!, { r6-r7 }              @ rw % 16 = 4 or 5
    subs    r2, r2, #4                  @
    stmia   r0!, { r6-r7 }              @
    b       25f @ copy up done          @
    ldmia   r1!, { r6-r8 }              @ rw % 16 = 6 or 7
    subs    r2, r2, #6                  @
    stmia   r0!, { r6-r8 }              @
    b       25f @ copy up done          @
    ldmia   r1!, { r6-r9 }              @ rw % 16 = 8 or 9
    subs    r2, r2, #8                  @
    stmia   r0!, { r6-r9 }              @
    b       25f @ copy up done          @
    ldmia   r1!, { r6-r10 }             @ rw % 16 = 10 or 11
    subs    r2, r2, #10                 @
    stmia   r0!, { r6-r10 }             @
    b       25f @ copy up done          @
    ldmia   r1!, { r6-r11 }             @ rw % 16 = 12 or 13
    subs    r2, r2, #12                 @
    stmia   r0!, { r6-r11 }             @
    b       25f @ copy up done          @
    ldmia   r1!, { r6-r12 }             @ rw % 16 = 14 or 15
    subs    r2, r2, #14                 @
    stmia   r0!, { r6-r12 }             @
25: @ copy up done                      @
    ble     40f @ finish line           @ no 32-byte segments remaining?
30: @ octword loop                      @ copy 16 pixels per loop
    ldmia   r1!, { r6-r12, r14 }        @
    subs    r2, r2, #16                 @
    stmia   r0!, { r6-r12, r14 }        @
    bgt     30b @ octword loop          @ 
40: @ finish line                       @
    ldreqh  r6, [r1], #2                @ finish last halfword if eq ...
    add     r1, r1, r4, lsl #1          @
    streqh  r6, [r0], #2                @ ...
    add     r0, r0, r4, lsl #1          @
    subs    r3, r3, #1                  @ next line
    bgt     10b @ copy line             @
    ldmfd   sp!, { r4-r12, pc }         @ restore regs and return
    .ltorg                              @ dump constant pool
    .size   lcd_copy_buffer_rect, .-lcd_copy_buffer_rect

