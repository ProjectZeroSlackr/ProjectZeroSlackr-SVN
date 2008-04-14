	.text
	.align	2
	@@.type	tiledraw_8, %function
	
__ASM_functions_list:
	.long	tiledraw_8
	.long	0
	.global _tiledraw_setup
_tiledraw_setup:
	str	r4, [sp, #-4]!
	@@ Setup stuff for code copy.
	ldr	r0, =__ASM_functions_start
	ldr	r1, =0x40015040
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
	.global	tiledraw_8
tiledraw_8:
	stmfd	sp!, {r4, r5, r6, lr}
	ldr	ip, [r1], #4
	ldr	r5, [sp, #16]
	cmp	ip, #0
	ldr	lr, [r2], #4
	mov	r6, r3
	rsbeq	r3, r5, r0
	mov	r4, r2
	addeq	r0, r3, #8
	beq	.L3
	add	r2, ip, r5
	cmp	r5, #7
	ldrls	pc, [pc, r5, asl #2]
	b	.L3
	.p2align 2
.L49:
	.word	.L5
	.word	.L14
	.word	.L22
	.word	.L29
	.word	.L35
	.word	.L40
	.word	.L44
	.word	.L47
.L44:
	ldrb	r3, [r2, #0]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, lr]	@ zero_extendqisi2
	strneb	r3, [r0, #0]
	ldrb	ip, [r2, #1]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, lr]	@ zero_extendqisi2
	strneb	ip, [r0, #1]
	add	r0, r0, #2
.L3:
	mov	r2, r6
	b	.L107
.L109:
	ldr	ip, [r1], #4
	cmp	ip, #0
	ldr	lr, [r4], #4
	beq	.L62
	ldrb	r3, [ip, #0]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, lr]	@ zero_extendqisi2
	strneb	r3, [r0, #0]
	ldrb	r3, [ip, #1]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, lr]	@ zero_extendqisi2
	strneb	r3, [r0, #1]
	ldrb	r3, [ip, #2]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, lr]	@ zero_extendqisi2
	strneb	r3, [r0, #2]
	ldrb	r3, [ip, #3]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, lr]	@ zero_extendqisi2
	strneb	r3, [r0, #3]
	ldrb	r3, [ip, #4]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, lr]	@ zero_extendqisi2
	strneb	r3, [r0, #4]
	ldrb	r3, [ip, #5]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, lr]	@ zero_extendqisi2
	strneb	r3, [r0, #5]
	ldrb	r3, [ip, #6]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, lr]	@ zero_extendqisi2
	strneb	r3, [r0, #6]
	ldrb	r3, [ip, #7]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, lr]	@ zero_extendqisi2
	strneb	r3, [r0, #7]
.L62:
	add	r0, r0, #8
	sub	r2, r2, #1
.L107:
	cmp	r2, #1
	bgt	.L109
	cmp	r5, #0
	ldmeqfd	sp!, {r4, r5, r6, pc}
	ldr	r1, [r1, #0]
	cmp	r1, #0
	ldr	r2, [r4, #0]
	ldmeqfd	sp!, {r4, r5, r6, pc}
	sub	ip, r5, #1
	cmp	ip, #6
	ldrls	pc, [pc, ip, asl #2]
	b	.L1
	.p2align 2
.L102:
	.word	.L100
	.word	.L97
	.word	.L93
	.word	.L88
	.word	.L82
	.word	.L75
	.word	.L67
.L47:
	ldrb	ip, [r2, #0]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, lr]	@ zero_extendqisi2
	strneb	ip, [r0, #0]
	add	r0, r0, #1
	b	.L3
.L5:
	ldrb	ip, [r2, #0]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, lr]	@ zero_extendqisi2
	strneb	ip, [r0, #0]
	ldrb	r3, [r2, #1]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, lr]	@ zero_extendqisi2
	strneb	r3, [r0, #1]
	ldrb	ip, [r2, #2]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, lr]	@ zero_extendqisi2
	strneb	ip, [r0, #2]
	ldrb	r3, [r2, #3]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, lr]	@ zero_extendqisi2
	strneb	r3, [r0, #3]
	ldrb	ip, [r2, #4]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, lr]	@ zero_extendqisi2
	strneb	ip, [r0, #4]
	ldrb	r3, [r2, #5]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, lr]	@ zero_extendqisi2
	strneb	r3, [r0, #5]
	ldrb	ip, [r2, #6]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, lr]	@ zero_extendqisi2
	strneb	ip, [r0, #6]
	ldrb	r3, [r2, #7]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, lr]	@ zero_extendqisi2
	strneb	r3, [r0, #7]
	add	r0, r0, #8
	b	.L3
.L14:
	ldrb	r3, [r2, #0]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, lr]	@ zero_extendqisi2
	strneb	r3, [r0, #0]
	ldrb	ip, [r2, #1]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, lr]	@ zero_extendqisi2
	strneb	ip, [r0, #1]
	ldrb	r3, [r2, #2]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, lr]	@ zero_extendqisi2
	strneb	r3, [r0, #2]
	ldrb	ip, [r2, #3]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, lr]	@ zero_extendqisi2
	strneb	ip, [r0, #3]
	ldrb	r3, [r2, #4]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, lr]	@ zero_extendqisi2
	strneb	r3, [r0, #4]
	ldrb	ip, [r2, #5]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, lr]	@ zero_extendqisi2
	strneb	ip, [r0, #5]
	ldrb	r3, [r2, #6]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, lr]	@ zero_extendqisi2
	strneb	r3, [r0, #6]
	add	r0, r0, #7
	b	.L3
.L22:
	ldrb	r3, [r2, #0]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, lr]	@ zero_extendqisi2
	strneb	r3, [r0, #0]
	ldrb	ip, [r2, #1]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, lr]	@ zero_extendqisi2
	strneb	ip, [r0, #1]
	ldrb	r3, [r2, #2]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, lr]	@ zero_extendqisi2
	strneb	r3, [r0, #2]
	ldrb	ip, [r2, #3]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, lr]	@ zero_extendqisi2
	strneb	ip, [r0, #3]
	ldrb	r3, [r2, #4]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, lr]	@ zero_extendqisi2
	strneb	r3, [r0, #4]
	ldrb	ip, [r2, #5]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, lr]	@ zero_extendqisi2
	strneb	ip, [r0, #5]
	add	r0, r0, #6
	b	.L3
.L29:
	ldrb	ip, [r2, #0]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, lr]	@ zero_extendqisi2
	strneb	ip, [r0, #0]
	ldrb	r3, [r2, #1]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, lr]	@ zero_extendqisi2
	strneb	r3, [r0, #1]
	ldrb	ip, [r2, #2]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, lr]	@ zero_extendqisi2
	strneb	ip, [r0, #2]
	ldrb	r3, [r2, #3]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, lr]	@ zero_extendqisi2
	strneb	r3, [r0, #3]
	ldrb	ip, [r2, #4]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, lr]	@ zero_extendqisi2
	strneb	ip, [r0, #4]
	add	r0, r0, #5
	b	.L3
.L35:
	ldrb	ip, [r2, #0]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, lr]	@ zero_extendqisi2
	strneb	ip, [r0, #0]
	ldrb	r3, [r2, #1]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, lr]	@ zero_extendqisi2
	strneb	r3, [r0, #1]
	ldrb	ip, [r2, #2]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, lr]	@ zero_extendqisi2
	strneb	ip, [r0, #2]
	ldrb	r3, [r2, #3]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, lr]	@ zero_extendqisi2
	strneb	r3, [r0, #3]
	add	r0, r0, #4
	b	.L3
.L40:
	ldrb	r3, [r2, #0]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, lr]	@ zero_extendqisi2
	strneb	r3, [r0, #0]
	ldrb	ip, [r2, #1]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, lr]	@ zero_extendqisi2
	strneb	ip, [r0, #1]
	ldrb	r3, [r2, #2]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, lr]	@ zero_extendqisi2
	strneb	r3, [r0, #2]
	add	r0, r0, #3
	b	.L3
.L100:
	ldrb	r1, [r1, #0]	@ zero_extendqisi2
	cmp	r1, #0
	ldmeqfd	sp!, {r4, r5, r6, pc}
	ldrb	ip, [r1, r2]	@ zero_extendqisi2
	strb	ip, [r0, #0]
.L1:
	ldmfd	sp!, {r4, r5, r6, pc}
.L67:
	ldrb	ip, [r1, #0]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, r2]	@ zero_extendqisi2
	strneb	ip, [r0, #0]
	ldrb	r3, [r1, #1]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, r2]	@ zero_extendqisi2
	strneb	r3, [r0, #1]
	ldrb	ip, [r1, #2]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, r2]	@ zero_extendqisi2
	strneb	ip, [r0, #2]
	ldrb	r3, [r1, #3]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, r2]	@ zero_extendqisi2
	strneb	r3, [r0, #3]
	ldrb	ip, [r1, #4]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, r2]	@ zero_extendqisi2
	strneb	ip, [r0, #4]
	ldrb	r3, [r1, #5]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, r2]	@ zero_extendqisi2
	strneb	r3, [r0, #5]
	ldrb	r1, [r1, #6]	@ zero_extendqisi2
	cmp	r1, #0
	ldrneb	r3, [r1, r2]	@ zero_extendqisi2
	strneb	r3, [r0, #6]
	ldmfd	sp!, {r4, r5, r6, pc}
.L97:
	ldrb	r3, [r1, #0]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, r2]	@ zero_extendqisi2
	strneb	r3, [r0, #0]
	ldrb	r1, [r1, #1]	@ zero_extendqisi2
	cmp	r1, #0
	ldrneb	r3, [r1, r2]	@ zero_extendqisi2
	strneb	r3, [r0, #1]
	ldmfd	sp!, {r4, r5, r6, pc}
.L93:
	ldrb	ip, [r1, #0]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, r2]	@ zero_extendqisi2
	strneb	ip, [r0, #0]
	ldrb	r3, [r1, #1]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, r2]	@ zero_extendqisi2
	strneb	r3, [r0, #1]
	ldrb	r1, [r1, #2]	@ zero_extendqisi2
	cmp	r1, #0
	ldrneb	r3, [r1, r2]	@ zero_extendqisi2
	strneb	r3, [r0, #2]
	ldmfd	sp!, {r4, r5, r6, pc}
.L88:
	ldrb	ip, [r1, #0]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, r2]	@ zero_extendqisi2
	strneb	ip, [r0, #0]
	ldrb	r3, [r1, #1]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, r2]	@ zero_extendqisi2
	strneb	r3, [r0, #1]
	ldrb	ip, [r1, #2]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, r2]	@ zero_extendqisi2
	strneb	ip, [r0, #2]
	ldrb	r1, [r1, #3]	@ zero_extendqisi2
	cmp	r1, #0
	ldrneb	r3, [r1, r2]	@ zero_extendqisi2
	strneb	r3, [r0, #3]
	ldmfd	sp!, {r4, r5, r6, pc}
.L82:
	ldrb	r3, [r1, #0]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, r2]	@ zero_extendqisi2
	strneb	r3, [r0, #0]
	ldrb	ip, [r1, #1]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, r2]	@ zero_extendqisi2
	strneb	ip, [r0, #1]
	ldrb	r3, [r1, #2]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, r2]	@ zero_extendqisi2
	strneb	r3, [r0, #2]
	ldrb	ip, [r1, #3]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, r2]	@ zero_extendqisi2
	strneb	ip, [r0, #3]
	ldrb	r1, [r1, #4]	@ zero_extendqisi2
	cmp	r1, #0
	ldrneb	r3, [r1, r2]	@ zero_extendqisi2
	strneb	r3, [r0, #4]
	ldmfd	sp!, {r4, r5, r6, pc}
.L75:
	ldrb	r3, [r1, #0]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, r2]	@ zero_extendqisi2
	strneb	r3, [r0, #0]
	ldrb	ip, [r1, #1]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, r2]	@ zero_extendqisi2
	strneb	ip, [r0, #1]
	ldrb	r3, [r1, #2]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, r2]	@ zero_extendqisi2
	strneb	r3, [r0, #2]
	ldrb	ip, [r1, #3]	@ zero_extendqisi2
	cmp	ip, #0
	ldrneb	ip, [ip, r2]	@ zero_extendqisi2
	strneb	ip, [r0, #3]
	ldrb	r3, [r1, #4]	@ zero_extendqisi2
	cmp	r3, #0
	ldrneb	r3, [r3, r2]	@ zero_extendqisi2
	strneb	r3, [r0, #4]
	ldrb	r1, [r1, #5]	@ zero_extendqisi2
	cmp	r1, #0
	ldrneb	r3, [r1, r2]	@ zero_extendqisi2
	strneb	r3, [r0, #5]
	ldmfd	sp!, {r4, r5, r6, pc}
__ASM_functions_end:
	.size	tiledraw_8, .-tiledraw_8
	.ident	"GCC: (GNU) 3.4.3"
