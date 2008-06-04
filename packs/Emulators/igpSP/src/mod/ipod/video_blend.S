@ Copy of GP2X build's video_blend.S but
@ expand_blend doesn't seem to work. ~Keripo

.align 2

.global expand_normal

@ The following function isn't complete (only works on run multiples of 8),
@ but unfortunately I don't see much potential for actually being able to
@ use it..

#define expand_pixel_pair(reg, temp)                                         ;\
  and temp, r3, reg, lsr #15                                                 ;\
  ldrh temp, [r2, temp]                                                      ;\
                                                                             ;\
  and reg, r3, reg, lsl #1                                                   ;\
  ldrh reg, [r2, reg]                                                        ;\
                                                                             ;\
  orr reg, reg, temp, lsl #16                                                ;\


@ Input:
@   r0 = screen_ptr
@   r1 = start
@   r2 = end

1:
  .word palette_ram_converted
  .word 0x3FE

expand_normal:
  stmdb sp!, { r4, r5, r6, r7, r14 }

  add r0, r0, r1, lsl #1          @ screen_ptr += start
  sub r1, r2, r1                  @ r1 = end - start
  ldr r2, 1b                      @ r2 = palette_ram_converted
  ldr r3, 1b + 4                  @ r3 = 0x3FE

2:
  ldmia r0, { r4, r5, r6, r7 }

  expand_pixel_pair(r4, r14)
  expand_pixel_pair(r5, r14)
  expand_pixel_pair(r6, r14)
  expand_pixel_pair(r7, r14)

  stmia r0!, { r4, r5, r6, r7 }

  subs r1, r1, #8
  bne 2b

  ldmia sp!, { r4, r5, r6, r7, pc }

