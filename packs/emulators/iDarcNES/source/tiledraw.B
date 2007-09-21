	.section text
	
__ASM_functions_list:
	.long	tiledraw_8
	.long	0
	.global _tiledraw_setup
_tiledraw_setup:
	str	r4, [sp, #-4]!
	@@ Setup stuff for code copy.
	ldr	r0, =__ASM_functions_start
	ldr	r1, =0x40015030
	ldr	r2, =__ASM_functions_end
	sub	r3, r1, r0		@ r3 = offset
	@@ Copy ASM funcs to IRAM + 0x30.
1:	ldr	r4, [r0], #4
	str	r4, [r1], #4
	cmp	r0, r2
	blo	1b
	@@ Put stubs in at the old addresses to jump to the new ones.
	adr	r0, __ASM_functions_list
1:	ldr	r1, [r0], #4		@ load a funcptr and inc funcptrptr
	cmp	r1, #0			@ is it the 0 at the end?
	beq	2f			@ if so, break out
	add	r2, r1, r3		@ r2 = address of func in iram
	ldr	r4, =0xe51ff004		@ r3 = encoding of `ldr pc, [pc, #-4]' (load pc from next word)
	str	r4, [r1], #4		@ store instr at func and inc funcptr
	str	r2, [r1], #4		@ store address of where-to-jump at func + 4 and inc funcptr
	b	1b			@ keep looping
2:	@@ Return.
	ldr	r4, [sp], #4
	bx	lr
	.pool

__ASM_functions_start:
	.global tiledraw_8
tiledraw_8:
	@ args = 4, pretend = 0, frame = 28
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp
	stmfd	sp!, {fp, ip, lr, pc}
	sub	fp, ip, #4
	sub	sp, sp, #28
	str	r0, [fp, #-16]
	str	r1, [fp, #-20]
	str	r2, [fp, #-24]
	str	r3, [fp, #-28]
	sub	r2, fp, #24
	ldr	r3, [r2, #0]
	ldr	r1, [r3, #0]
	add	r3, r3, #4
	str	r3, [r2, #0]
	str	r1, [fp, #-36]
	sub	r2, fp, #20
	ldr	r3, [r2, #0]
	ldr	r1, [r3, #0]
	add	r3, r3, #4
	str	r3, [r2, #0]
	str	r1, [fp, #-40]
	ldr	r3, [fp, #-40]
	cmp	r3, #0
	bne	.L2
	ldr	r2, [fp, #-16]
	ldr	r3, [fp, #4]
	rsb	r3, r3, r2
	add	r3, r3, #8
	str	r3, [fp, #-16]
	b	.L3
.L2:
	ldr	r2, [fp, #-40]
	ldr	r3, [fp, #4]
	add	r3, r2, r3
	str	r3, [fp, #-40]
	ldr	r3, [fp, #4]
	cmp	r3, #7
	ldrls	pc, [pc, r3, asl #2]
	b	.L3
	.p2align 2
.L21:
	.word	.L5
	.word	.L7
	.word	.L9
	.word	.L11
	.word	.L13
	.word	.L15
	.word	.L17
	.word	.L19
.L5:
	ldr	r3, [fp, #-40]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L6
	ldr	r1, [fp, #-16]
	ldr	r3, [fp, #-40]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldr	r3, [fp, #-36]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]
	strb	r3, [r1, #0]
.L6:
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-40]
	add	r3, r3, #1
	str	r3, [fp, #-40]
.L7:
	ldr	r3, [fp, #-40]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L8
	ldr	r1, [fp, #-16]
	ldr	r3, [fp, #-40]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldr	r3, [fp, #-36]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]
	strb	r3, [r1, #0]
.L8:
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-40]
	add	r3, r3, #1
	str	r3, [fp, #-40]
.L9:
	ldr	r3, [fp, #-40]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L10
	ldr	r1, [fp, #-16]
	ldr	r3, [fp, #-40]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldr	r3, [fp, #-36]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]
	strb	r3, [r1, #0]
.L10:
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-40]
	add	r3, r3, #1
	str	r3, [fp, #-40]
.L11:
	ldr	r3, [fp, #-40]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L12
	ldr	r1, [fp, #-16]
	ldr	r3, [fp, #-40]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldr	r3, [fp, #-36]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]
	strb	r3, [r1, #0]
.L12:
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-40]
	add	r3, r3, #1
	str	r3, [fp, #-40]
.L13:
	ldr	r3, [fp, #-40]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L14
	ldr	r1, [fp, #-16]
	ldr	r3, [fp, #-40]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldr	r3, [fp, #-36]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]
	strb	r3, [r1, #0]
.L14:
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-40]
	add	r3, r3, #1
	str	r3, [fp, #-40]
.L15:
	ldr	r3, [fp, #-40]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L16
	ldr	r1, [fp, #-16]
	ldr	r3, [fp, #-40]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldr	r3, [fp, #-36]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]
	strb	r3, [r1, #0]
.L16:
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-40]
	add	r3, r3, #1
	str	r3, [fp, #-40]
.L17:
	ldr	r3, [fp, #-40]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L18
	ldr	r1, [fp, #-16]
	ldr	r3, [fp, #-40]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldr	r3, [fp, #-36]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]
	strb	r3, [r1, #0]
.L18:
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-40]
	add	r3, r3, #1
	str	r3, [fp, #-40]
.L19:
	ldr	r3, [fp, #-40]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L20
	ldr	r1, [fp, #-16]
	ldr	r3, [fp, #-40]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldr	r3, [fp, #-36]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]
	strb	r3, [r1, #0]
.L20:
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-40]
	add	r3, r3, #1
	str	r3, [fp, #-40]
.L3:
	ldr	r3, [fp, #-28]
	str	r3, [fp, #-32]
.L22:
	ldr	r3, [fp, #-32]
	cmp	r3, #1
	ble	.L23
	sub	r2, fp, #24
	ldr	r3, [r2, #0]
	ldr	r1, [r3, #0]
	add	r3, r3, #4
	str	r3, [r2, #0]
	str	r1, [fp, #-36]
	sub	r2, fp, #20
	ldr	r3, [r2, #0]
	ldr	r1, [r3, #0]
	add	r3, r3, #4
	str	r3, [r2, #0]
	str	r1, [fp, #-40]
	ldr	r3, [fp, #-40]
	cmp	r3, #0
	bne	.L25
	ldr	r3, [fp, #-16]
	add	r3, r3, #8
	str	r3, [fp, #-16]
	b	.L24
.L25:
	ldr	r3, [fp, #-40]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L27
	ldr	r1, [fp, #-16]
	ldr	r3, [fp, #-40]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldr	r3, [fp, #-36]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]
	strb	r3, [r1, #0]
.L27:
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-40]
	add	r3, r3, #1
	str	r3, [fp, #-40]
	ldr	r3, [fp, #-40]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L28
	ldr	r1, [fp, #-16]
	ldr	r3, [fp, #-40]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldr	r3, [fp, #-36]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]
	strb	r3, [r1, #0]
.L28:
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-40]
	add	r3, r3, #1
	str	r3, [fp, #-40]
	ldr	r3, [fp, #-40]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L29
	ldr	r1, [fp, #-16]
	ldr	r3, [fp, #-40]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldr	r3, [fp, #-36]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]
	strb	r3, [r1, #0]
.L29:
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-40]
	add	r3, r3, #1
	str	r3, [fp, #-40]
	ldr	r3, [fp, #-40]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L30
	ldr	r1, [fp, #-16]
	ldr	r3, [fp, #-40]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldr	r3, [fp, #-36]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]
	strb	r3, [r1, #0]
.L30:
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-40]
	add	r3, r3, #1
	str	r3, [fp, #-40]
	ldr	r3, [fp, #-40]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L31
	ldr	r1, [fp, #-16]
	ldr	r3, [fp, #-40]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldr	r3, [fp, #-36]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]
	strb	r3, [r1, #0]
.L31:
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-40]
	add	r3, r3, #1
	str	r3, [fp, #-40]
	ldr	r3, [fp, #-40]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L32
	ldr	r1, [fp, #-16]
	ldr	r3, [fp, #-40]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldr	r3, [fp, #-36]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]
	strb	r3, [r1, #0]
.L32:
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-40]
	add	r3, r3, #1
	str	r3, [fp, #-40]
	ldr	r3, [fp, #-40]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L33
	ldr	r1, [fp, #-16]
	ldr	r3, [fp, #-40]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldr	r3, [fp, #-36]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]
	strb	r3, [r1, #0]
.L33:
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-40]
	add	r3, r3, #1
	str	r3, [fp, #-40]
	ldr	r3, [fp, #-40]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L34
	ldr	r1, [fp, #-16]
	ldr	r3, [fp, #-40]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldr	r3, [fp, #-36]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]
	strb	r3, [r1, #0]
.L34:
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-40]
	add	r3, r3, #1
	str	r3, [fp, #-40]
.L24:
	ldr	r3, [fp, #-32]
	sub	r3, r3, #1
	str	r3, [fp, #-32]
	b	.L22
.L23:
	ldr	r3, [fp, #4]
	cmp	r3, #0
	beq	.L1
	sub	r2, fp, #24
	ldr	r3, [r2, #0]
	ldr	r1, [r3, #0]
	add	r3, r3, #4
	str	r3, [r2, #0]
	str	r1, [fp, #-36]
	sub	r2, fp, #20
	ldr	r3, [r2, #0]
	ldr	r1, [r3, #0]
	add	r3, r3, #4
	str	r3, [r2, #0]
	str	r1, [fp, #-40]
	ldr	r3, [fp, #-40]
	cmp	r3, #0
	beq	.L1
	ldr	r3, [fp, #4]
	sub	r3, r3, #1
	cmp	r3, #6
	ldrls	pc, [pc, r3, asl #2]
	b	.L1
	.p2align 2
.L52:
	.word	.L50
	.word	.L48
	.word	.L46
	.word	.L44
	.word	.L42
	.word	.L40
	.word	.L38
.L38:
	ldr	r3, [fp, #-40]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L39
	ldr	r1, [fp, #-16]
	ldr	r3, [fp, #-40]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldr	r3, [fp, #-36]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]
	strb	r3, [r1, #0]
.L39:
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-40]
	add	r3, r3, #1
	str	r3, [fp, #-40]
.L40:
	ldr	r3, [fp, #-40]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L41
	ldr	r1, [fp, #-16]
	ldr	r3, [fp, #-40]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldr	r3, [fp, #-36]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]
	strb	r3, [r1, #0]
.L41:
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-40]
	add	r3, r3, #1
	str	r3, [fp, #-40]
.L42:
	ldr	r3, [fp, #-40]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L43
	ldr	r1, [fp, #-16]
	ldr	r3, [fp, #-40]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldr	r3, [fp, #-36]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]
	strb	r3, [r1, #0]
.L43:
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-40]
	add	r3, r3, #1
	str	r3, [fp, #-40]
.L44:
	ldr	r3, [fp, #-40]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L45
	ldr	r1, [fp, #-16]
	ldr	r3, [fp, #-40]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldr	r3, [fp, #-36]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]
	strb	r3, [r1, #0]
.L45:
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-40]
	add	r3, r3, #1
	str	r3, [fp, #-40]
.L46:
	ldr	r3, [fp, #-40]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L47
	ldr	r1, [fp, #-16]
	ldr	r3, [fp, #-40]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldr	r3, [fp, #-36]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]
	strb	r3, [r1, #0]
.L47:
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-40]
	add	r3, r3, #1
	str	r3, [fp, #-40]
.L48:
	ldr	r3, [fp, #-40]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L49
	ldr	r1, [fp, #-16]
	ldr	r3, [fp, #-40]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldr	r3, [fp, #-36]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]
	strb	r3, [r1, #0]
.L49:
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-40]
	add	r3, r3, #1
	str	r3, [fp, #-40]
.L50:
	ldr	r3, [fp, #-40]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L51
	ldr	r1, [fp, #-16]
	ldr	r3, [fp, #-40]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldr	r3, [fp, #-36]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]
	strb	r3, [r1, #0]
.L51:
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-40]
	add	r3, r3, #1
	str	r3, [fp, #-40]
.L1:
	sub	sp, fp, #12
	ldmfd	sp, {fp, sp, pc}
__ASM_functions_end:
	.size	tiledraw_8, .-tiledraw_8
	.ident	"GCC: (GNU) 3.4.3"
