	.file	"blit_c.c"
	.text
	.align	2
	.global	blit_2_8
	.type	blit_2_8, %function
blit_2_8:
	@ args = 8, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	stmfd	sp!, {r4, lr}
	mov	r4, r0, asl #1
	mov	lr, r1, lsr #1
	and	ip, lr, #85
	and	r1, r1, #85
	and	lr, r4, #170
	and	r0, r0, #170
	and	r3, r3, #255
	orr	ip, ip, r0
	orr	lr, lr, r1
	and	r0, r2, #255
	ldr	r4, [sp, #8]
	ldr	r1, [sp, #12]
	cmp	r3, #7
	ldrls	pc, [pc, r3, asl #2]
	b	.L1
	.p2align 2
.L26:
	.word	.L3
	.word	.L6
	.word	.L9
	.word	.L12
	.word	.L15
	.word	.L18
	.word	.L21
	.word	.L24
.L3:
	tst	ip, #192
	movne	r3, ip, lsr #6
	andne	r3, r3, #3
	ldrneb	r2, [r3, r4]	@ zero_extendqisi2
	strneb	r2, [r1, #0]
	sub	r2, r0, #1
	ands	r0, r2, #255
	add	r1, r1, #1
	ldmeqfd	sp!, {r4, pc}
.L6:
	tst	lr, #192
	ldrneb	r3, [r4, lr, lsr #6]	@ zero_extendqisi2
	strneb	r3, [r1, #0]
	sub	r3, r0, #1
	ands	r0, r3, #255
	add	r1, r1, #1
	ldmeqfd	sp!, {r4, pc}
.L9:
	tst	ip, #48
	movne	r3, ip, lsr #4
	andne	r3, r3, #3
	ldrneb	r2, [r3, r4]	@ zero_extendqisi2
	strneb	r2, [r1, #0]
	sub	r2, r0, #1
	ands	r0, r2, #255
	add	r1, r1, #1
	ldmeqfd	sp!, {r4, pc}
.L12:
	tst	lr, #48
	movne	r3, lr, lsr #4
	andne	r3, r3, #3
	ldrneb	r2, [r3, r4]	@ zero_extendqisi2
	sub	r3, r0, #1
	strneb	r2, [r1, #0]
	ands	r0, r3, #255
	add	r1, r1, #1
	ldmeqfd	sp!, {r4, pc}
.L15:
	tst	ip, #12
	movne	r3, ip, lsr #2
	andne	r3, r3, #3
	ldrneb	r2, [r3, r4]	@ zero_extendqisi2
	strneb	r2, [r1, #0]
	sub	r2, r0, #1
	ands	r0, r2, #255
	add	r1, r1, #1
	ldmeqfd	sp!, {r4, pc}
.L18:
	tst	lr, #12
	movne	r3, lr, lsr #2
	andne	r3, r3, #3
	ldrneb	r2, [r3, r4]	@ zero_extendqisi2
	sub	r3, r0, #1
	strneb	r2, [r1, #0]
	ands	r0, r3, #255
	add	r1, r1, #1
	ldmeqfd	sp!, {r4, pc}
.L21:
	ands	r2, ip, #3
	ldrneb	r2, [r2, r4]	@ zero_extendqisi2
	strneb	r2, [r1, #0]
	cmp	r0, #1
	add	r1, r1, #1
	ldmeqfd	sp!, {r4, pc}
.L24:
	ands	r3, lr, #3
	ldmeqfd	sp!, {r4, pc}
	ldrb	ip, [r3, r4]	@ zero_extendqisi2
	strb	ip, [r1, #0]
.L1:
	ldmfd	sp!, {r4, pc}
	.size	blit_2_8, .-blit_2_8
	.align	2
	.global	blit_2_8_rev
	.type	blit_2_8_rev, %function
blit_2_8_rev:
	@ args = 8, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	stmfd	sp!, {r4, lr}
	mov	r4, r0, asl #1
	mov	lr, r1, lsr #1
	and	ip, lr, #85
	and	r1, r1, #85
	and	lr, r4, #170
	and	r0, r0, #170
	and	r3, r3, #255
	orr	ip, ip, r0
	orr	lr, lr, r1
	and	r0, r2, #255
	ldr	r4, [sp, #8]
	ldr	r1, [sp, #12]
	cmp	r3, #7
	ldrls	pc, [pc, r3, asl #2]
	b	.L28
	.p2align 2
.L53:
	.word	.L30
	.word	.L33
	.word	.L36
	.word	.L39
	.word	.L42
	.word	.L45
	.word	.L48
	.word	.L51
.L30:
	ands	r3, lr, #3
	ldrneb	r3, [r3, r4]	@ zero_extendqisi2
	sub	r2, r0, #1
	strneb	r3, [r1, #0]
	ands	r0, r2, #255
	add	r1, r1, #1
	ldmeqfd	sp!, {r4, pc}
.L33:
	ands	r3, ip, #3
	ldrneb	r3, [r3, r4]	@ zero_extendqisi2
	sub	r2, r0, #1
	strneb	r3, [r1, #0]
	ands	r0, r2, #255
	add	r1, r1, #1
	ldmeqfd	sp!, {r4, pc}
.L36:
	tst	lr, #12
	movne	r3, lr, lsr #2
	andne	r3, r3, #3
	ldrneb	r2, [r3, r4]	@ zero_extendqisi2
	strneb	r2, [r1, #0]
	sub	r2, r0, #1
	ands	r0, r2, #255
	add	r1, r1, #1
	ldmeqfd	sp!, {r4, pc}
.L39:
	tst	ip, #12
	movne	r3, ip, lsr #2
	andne	r3, r3, #3
	ldrneb	r2, [r3, r4]	@ zero_extendqisi2
	sub	r3, r0, #1
	strneb	r2, [r1, #0]
	ands	r0, r3, #255
	add	r1, r1, #1
	ldmeqfd	sp!, {r4, pc}
.L42:
	tst	lr, #48
	movne	r3, lr, lsr #4
	andne	r3, r3, #3
	ldrneb	r2, [r3, r4]	@ zero_extendqisi2
	strneb	r2, [r1, #0]
	sub	r2, r0, #1
	ands	r0, r2, #255
	add	r1, r1, #1
	ldmeqfd	sp!, {r4, pc}
.L45:
	tst	ip, #48
	movne	r3, ip, lsr #4
	andne	r3, r3, #3
	ldrneb	r2, [r3, r4]	@ zero_extendqisi2
	sub	r3, r0, #1
	strneb	r2, [r1, #0]
	ands	r0, r3, #255
	add	r1, r1, #1
	ldmeqfd	sp!, {r4, pc}
.L48:
	tst	lr, #192
	ldrneb	r3, [r4, lr, lsr #6]	@ zero_extendqisi2
	strneb	r3, [r1, #0]
	cmp	r0, #1
	add	r1, r1, #1
	ldmeqfd	sp!, {r4, pc}
.L51:
	tst	ip, #192
	ldmeqfd	sp!, {r4, pc}
	mov	r2, ip, lsr #6
	and	r0, r2, #3
	ldrb	ip, [r0, r4]	@ zero_extendqisi2
	strb	ip, [r1, #0]
.L28:
	ldmfd	sp!, {r4, pc}
	.size	blit_2_8_rev, .-blit_2_8_rev
	.align	2
	.global	blit_4_8
	.type	blit_4_8, %function
blit_4_8:
	@ args = 16, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	stmfd	sp!, {r4, r5, r6, r7, lr}
	ldrb	ip, [sp, #24]	@ zero_extendqisi2
	ldrb	r4, [sp, #20]	@ zero_extendqisi2
	and	r7, r0, #255
	and	r6, r1, #255
	and	r5, r2, #255
	and	lr, r3, #255
	mov	r0, r7, asl ip
	mov	r1, r6, asl ip
	mov	r2, r5, asl ip
	mov	r3, lr, asl ip
	cmp	r4, #0
	and	r5, r0, #255
	and	lr, r1, #255
	and	r0, r2, #255
	and	ip, r3, #255
	ldr	r7, [sp, #28]
	ldr	r6, [sp, #32]
	ldmlefd	sp!, {r4, r5, r6, r7, pc}
.L58:
	and	r1, lr, #128
	and	r2, r5, #128
	mov	r3, r1, asr #5
	orr	r3, r3, r2, lsr #4
	and	r1, r0, #128
	orr	r2, r3, r1, lsr #6
	orr	r3, r2, ip, lsr #7
	ldrb	r2, [r3, r7]	@ zero_extendqisi2
	mov	r1, r5, asl #1
	mov	r3, lr, asl #1
	mov	r0, r0, asl #1
	mov	ip, ip, asl #1
	subs	r4, r4, #1
	strb	r2, [r6], #1
	and	r5, r1, #255
	and	lr, r3, #255
	and	r0, r0, #255
	and	ip, ip, #255
	bne	.L58
	ldmfd	sp!, {r4, r5, r6, r7, pc}
	.size	blit_4_8, .-blit_4_8
	.align	2
	.global	blit_4_8_rev
	.type	blit_4_8_rev, %function
blit_4_8_rev:
	@ args = 16, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	stmfd	sp!, {r4, r5, r6, r7, r8, lr}
	ldrb	ip, [sp, #28]	@ zero_extendqisi2
	ldrb	r8, [sp, #24]	@ zero_extendqisi2
	and	r6, r0, #255
	and	r5, r1, #255
	and	r4, r2, #255
	and	r7, r3, #255
	mov	r0, r6, asr ip
	mov	r3, r7, asr ip
	mov	r1, r5, asr ip
	mov	r2, r4, asr ip
	cmp	r8, #0
	and	r5, r0, #255
	and	r4, r1, #255
	and	r0, r2, #255
	and	lr, r3, #255
	ldr	r7, [sp, #32]
	ldr	r6, [sp, #36]
	ldmlefd	sp!, {r4, r5, r6, r7, r8, pc}
	mov	ip, r8
.L67:
	and	r3, r4, #1
	and	r2, r5, #1
	mov	r1, r3, asl #2
	orr	r3, r1, r2, asl #3
	and	r2, r0, #1
	orr	r3, r3, r2, asl #1
	and	r1, lr, #1
	orr	r3, r3, r1
	ldrb	r2, [r3, r7]	@ zero_extendqisi2
	subs	ip, ip, #1
	strb	r2, [r6], #1
	mov	r5, r5, lsr #1
	mov	r4, r4, lsr #1
	mov	r0, r0, lsr #1
	mov	lr, lr, lsr #1
	bne	.L67
	ldmfd	sp!, {r4, r5, r6, r7, r8, pc}
	.size	blit_4_8_rev, .-blit_4_8_rev
	.align	2
	.global	blit_4_8_czt
	.type	blit_4_8_czt, %function
blit_4_8_czt:
	@ args = 16, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	stmfd	sp!, {r4, r5, r6, r7, r8, lr}
	ldrb	ip, [sp, #28]	@ zero_extendqisi2
	ldrb	r4, [sp, #24]	@ zero_extendqisi2
	and	r7, r0, #255
	and	r6, r1, #255
	and	r5, r2, #255
	and	r8, r3, #255
	mov	r0, r7, asl ip
	mov	r3, r8, asl ip
	mov	r1, r6, asl ip
	mov	r2, r5, asl ip
	cmp	r4, #0
	and	r6, r0, #255
	and	r5, r1, #255
	and	r0, r2, #255
	and	lr, r3, #255
	ldr	r8, [sp, #32]
	ldr	r7, [sp, #36]
	ldmlefd	sp!, {r4, r5, r6, r7, r8, pc}
.L77:
	and	ip, r5, #128
	and	r2, r6, #128
	mov	r3, ip, asr #5
	orr	ip, r3, r2, lsr #4
	and	r1, r0, #128
	orr	r3, ip, r1, lsr #6
	orrs	r3, r3, lr, lsr #7
	ldrneb	r3, [r3, r8]	@ zero_extendqisi2
	mov	r2, r6, asl #1
	strneb	r3, [r7, #0]
	mov	r1, r5, asl #1
	mov	r0, r0, asl #1
	mov	ip, lr, asl #1
	subs	r4, r4, #1
	and	r6, r2, #255
	and	r5, r1, #255
	and	r0, r0, #255
	and	lr, ip, #255
	add	r7, r7, #1
	bne	.L77
	ldmfd	sp!, {r4, r5, r6, r7, r8, pc}
	.size	blit_4_8_czt, .-blit_4_8_czt
	.align	2
	.global	blit_4_8_czt_rev
	.type	blit_4_8_czt_rev, %function
blit_4_8_czt_rev:
	@ args = 16, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	stmfd	sp!, {r4, r5, r6, r7, r8, lr}
	ldrb	ip, [sp, #28]	@ zero_extendqisi2
	ldrb	r7, [sp, #24]	@ zero_extendqisi2
	and	r6, r0, #255
	and	r5, r1, #255
	and	r4, r2, #255
	and	r8, r3, #255
	mov	r3, r8, asr ip
	mov	r0, r6, asr ip
	mov	r1, r5, asr ip
	mov	r2, r4, asr ip
	cmp	r7, #0
	and	r6, r0, #255
	and	r4, r1, #255
	and	r0, r2, #255
	and	lr, r3, #255
	ldr	r8, [sp, #32]
	ldr	r5, [sp, #36]
	ldmlefd	sp!, {r4, r5, r6, r7, r8, pc}
	mov	ip, r7
.L87:
	and	r1, r4, #1
	and	r2, r6, #1
	mov	r3, r1, asl #2
	orr	r3, r3, r2, asl #3
	and	r1, r0, #1
	and	r2, lr, #1
	orr	r3, r3, r1, asl #1
	orrs	r3, r3, r2
	ldrneb	r3, [r3, r8]	@ zero_extendqisi2
	strneb	r3, [r5, #0]
	subs	ip, ip, #1
	mov	r6, r6, lsr #1
	mov	r4, r4, lsr #1
	mov	r0, r0, lsr #1
	mov	lr, lr, lsr #1
	add	r5, r5, #1
	bne	.L87
	ldmfd	sp!, {r4, r5, r6, r7, r8, pc}
	.size	blit_4_8_czt_rev, .-blit_4_8_czt_rev
	.ident	"GCC: (GNU) 3.4.3"
