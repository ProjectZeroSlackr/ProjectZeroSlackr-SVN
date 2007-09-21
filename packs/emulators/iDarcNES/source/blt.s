	.file	"blt.c"
	.text
	.align	2
	.global	stretchblt
	.type	stretchblt, %function
stretchblt:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	stmfd	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
	ldr	r2, .L21
	ldr	r3, [r2, #0]
	cmp	r3, #0
	sub	sp, sp, #16
	mov	r6, r0
	mov	sl, r1
	mov	r9, #0
	bls	.L13
.L20:
	ldr	r4, .L21+4
	ldr	r0, .L21+8
	ldr	r5, [r4, #0]
	ldr	r1, [r0, r9, asl #2]
	cmp	r5, #0
	mov	r8, r1, asl #8
	mov	r5, #0
	bls	.L15
	add	r7, r8, sl
	mov	r4, r9, asl #2
	stmia	sp, {r4, r7}	@ phole stm
.L10:
	ldr	r7, .L21+12
	ldr	r1, [r7, r5, asl #2]
	add	lr, r8, r1
	add	r4, lr, sl
	add	ip, r4, #256
	add	r0, r1, r8
	ldr	r2, [sp, #4]
	ldr	r3, .L21+12
	ldr	r7, .L21+16
	ldr	r4, .L21+8
	add	lr, r0, sl
	str	ip, [sp, #8]
	ldr	r0, [sp, #0]
	ldrb	ip, [r2, r1]	@ zero_extendqisi2
	add	r2, r3, r5, asl #2
	ldr	r3, [r7, r9, asl #2]
	add	r7, r0, r4
	ldr	r4, .L21+4
	ldr	r0, [r4, #0]
	ldr	r4, .L21+20
	str	r0, [sp, #12]
	ldr	r0, [r2, #4]
	add	r3, r3, r5
	ldr	r2, [r4, ip, asl #2]
	cmp	r0, r1
	mov	r4, r3, asl #1
	add	r5, r5, #1
	strh	r2, [r4, r6]	@ movhi 
	beq	.L8
	ldrb	r1, [lr, #1]	@ zero_extendqisi2
	ldr	r3, .L21+20
	ldr	r2, [r3, r1, asl #2]
	ldrh	r0, [r4, r6]
	and	r1, r2, #2016
	and	ip, r0, #2016
	mov	r3, r1, lsr #6
	and	r1, r2, #63488
	add	r3, r3, ip, lsr #6
	and	r2, r2, #31
	mov	ip, r1, lsr #12
	add	r1, ip, r0, lsr #12
	and	lr, r0, #31
	mov	ip, r3, asl #5
	mov	r0, r2, lsr #1
	add	r2, r0, lr, lsr #1
	orr	r3, ip, r1, asl #11
	orr	r1, r3, r2
	strh	r1, [r4, r6]	@ movhi 
.L8:
	ldr	r0, .L21+8
	ldr	r2, [r7, #4]
	ldr	ip, [r0, r9, asl #2]
	cmp	r2, ip
	beq	.L7
	ldr	ip, [sp, #8]
	ldr	r3, .L21+20
	ldrb	r0, [ip, #0]	@ zero_extendqisi2
	ldr	r2, [r3, r0, asl #2]
	ldrh	ip, [r4, r6]
	and	r1, r2, #2016
	and	r0, ip, #2016
	mov	r3, r1, lsr #6
	and	r1, r2, #63488
	add	r3, r3, r0, lsr #6
	mov	r1, r1, lsr #12
	and	r0, r2, #31
	add	r1, r1, ip, lsr #12
	mov	r2, r0, lsr #1
	and	ip, ip, #31
	mov	r0, r3, asl #5
	add	r2, r2, ip, lsr #1
	orr	ip, r0, r1, asl #11
	orr	r3, ip, r2
	strh	r3, [r4, r6]	@ movhi 
.L7:
	ldr	r4, [sp, #12]
	cmp	r4, r5
	bhi	.L10
.L15:
	ldr	r1, .L21
	ldr	r2, [r1, #0]
	add	r9, r9, #1
	cmp	r2, r9
	bhi	.L20
.L13:
	add	sp, sp, #16
	ldmfd	sp!, {r4, r5, r6, r7, r8, r9, sl, pc}
.L22:
	.align	2
.L21:
	.word	H
	.word	W
	.word	scaley
	.word	scalex
	.word	screeny
	.word	vid_pre_xlat2
	.size	stretchblt, .-stretchblt
	.align	2
	.global	stretchblt2
	.type	stretchblt2, %function
stretchblt2:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	stmfd	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
	ldr	r9, .L38
	ldr	r3, [r9, #0]
	cmp	r3, #0
	mov	r6, r0
	mov	r8, r1
	mov	r7, #0
	ldmlsfd	sp!, {r4, r5, r6, r7, r8, r9, sl, pc}
	ldr	r0, .L38+4
	ldr	sl, .L38+8
	ldr	r4, [r0, #0]
.L31:
	ldr	r1, [sl, r7, asl #2]
	cmp	r4, #0
	mov	r2, r1, asl #8
	mov	lr, #0
	bls	.L35
	ldr	r5, .L38+12
	ldr	r3, [r5, r7, asl #2]
	ldr	r0, .L38+16
	ldr	r5, .L38+20
	add	r1, r2, r8
	mov	ip, r3, asl #1
.L30:
	ldr	r3, [r5, lr, asl #2]
	ldrb	r2, [r1, r3]	@ zero_extendqisi2
	add	lr, lr, #1
	ldr	r3, [r0, r2, asl #2]
	cmp	r4, lr
	strh	r3, [ip, r6]	@ movhi 
	add	ip, ip, #2
	bhi	.L30
.L35:
	ldr	ip, [r9, #0]
	add	r7, r7, #1
	cmp	ip, r7
	bhi	.L31
	ldmfd	sp!, {r4, r5, r6, r7, r8, r9, sl, pc}
.L39:
	.align	2
.L38:
	.word	H
	.word	W
	.word	scaley
	.word	screeny
	.word	vid_pre_xlat2
	.word	scalex
	.size	stretchblt2, .-stretchblt2
	.ident	"GCC: (GNU) 3.4.3"
