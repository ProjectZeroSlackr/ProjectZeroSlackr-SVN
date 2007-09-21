/*
 * blitters.S
 *
 * miscellaneous blitters
 */

/* $Id: blitters.S,v 1.1 1999/02/21 23:32:34 nyef Exp $ */

#include "machdep.h"

	.globl SYMNAME(blit_2_8)
SYMLABEL(blit_2_8)
	pushl  %esi
	pushl  %edi
	pushl  %ebx
	pushl  %ecx
	movb   0x14(%esp),%bl
	movb   0x18(%esp),%bh
	movb   0x20(%esp),%cl
	shlb   %cl,%bl
	shlb   %cl,%bh
	movl   0x24(%esp),%esi
	movl   0x28(%esp),%edi
	movb   0x1c(%esp),%cl
0:	xorl   %eax,%eax
	rclb   %bl
	rcll   %eax
	rclb   %bh
	rcll   %eax
	orb    %al,%al
	je     1f
	movb   (%eax,%esi),%al
	movb   %al,(%edi)
1:	incl   %edi
	decb   %cl
	jne    0b
	popl   %ecx
	popl   %ebx
	popl   %edi
	popl   %esi
	ret

	.globl SYMNAME(blit_2_8_rev)
SYMLABEL(blit_2_8_rev)
	pushl  %esi
	pushl  %edi
	pushl  %ebx
	pushl  %ecx
	movb   0x14(%esp,1),%bl
	movb   0x18(%esp,1),%bh
	movb   0x20(%esp,1),%cl
	shrb   %cl,%bl
	shrb   %cl,%bh
	movl   0x24(%esp,1),%esi
	movl   0x28(%esp,1),%edi
	movb   0x1c(%esp,1),%cl
0:	xorl   %eax,%eax
	rcrb   %bl
	rcll   %eax
	rcrb   %bh
	rcll   %eax
	orb    %al,%al
	je     1f
	movb   (%eax,%esi,1),%al
	movb   %al,(%edi)
1:	incl   %edi
	decb   %cl
	jne    0b
	popl   %ecx
	popl   %ebx
	popl   %edi
	popl   %esi
	ret

	.globl SYMNAME(blit_4_8)
SYMLABEL(blit_4_8)
	pushl  %esi
	pushl  %edi
	pushl  %ebx
	pushl  %ecx
	pushl  %edx
	movb   0x18(%esp,1),%bl
	movb   0x1c(%esp,1),%bh
	movb   0x20(%esp,1),%dl
	movb   0x24(%esp,1),%dh
	movb   0x2c(%esp,1),%cl
	shlb   %cl,%bl
	shlb   %cl,%bh
	shlb   %cl,%dl
	shlb   %cl,%dh
	movl   0x30(%esp,1),%esi
	movl   0x34(%esp,1),%edi
	movb   0x28(%esp,1),%cl
0:	xorl   %eax,%eax
	rclb   %bl
	rcll   %eax
	rclb   %bh
	rcll   %eax
	rclb   %dl
	rcll   %eax
	rclb   %dh
	rcll   %eax
	movb   (%eax,%esi,1),%al
	movb   %al,(%edi)
	incl   %edi
	decb   %cl
	jne    0b
	popl   %edx
	popl   %ecx
	popl   %ebx
	popl   %edi
	popl   %esi
	ret

	.globl SYMNAME(blit_4_8_czt)
SYMLABEL(blit_4_8_czt)
	pushl  %esi
	pushl  %edi
	pushl  %ebx
	pushl  %ecx
	pushl  %edx
	movb   0x18(%esp,1),%bl
	movb   0x1c(%esp,1),%bh
	movb   0x20(%esp,1),%dl
	movb   0x24(%esp,1),%dh
	movb   0x2c(%esp,1),%cl
	shlb   %cl,%bl
	shlb   %cl,%bh
	shlb   %cl,%dl
	shlb   %cl,%dh
	movl   0x30(%esp,1),%esi
	movl   0x34(%esp,1),%edi
	movb   0x28(%esp,1),%cl
0:	xorl   %eax,%eax
	rclb   %bl
	rcll   %eax
	rclb   %bh
	rcll   %eax
	rclb   %dl
	rcll   %eax
	rclb   %dh
	rcll   %eax
	orb    %al, %al
	jz     1f
	movb   (%eax,%esi,1),%al
	movb   %al,(%edi)
1:	incl   %edi
	decb   %cl
	jne    0b
	popl   %edx
	popl   %ecx
	popl   %ebx
	popl   %edi
	popl   %esi
	ret

	.globl SYMNAME(blit_4_8_rev)
SYMLABEL(blit_4_8_rev)
	pushl  %esi
	pushl  %edi
	pushl  %ebx
	pushl  %ecx
	pushl  %edx
	movb   0x18(%esp,1),%bl
	movb   0x1c(%esp,1),%bh
	movb   0x20(%esp,1),%dl
	movb   0x24(%esp,1),%dh
	movb   0x2c(%esp,1),%cl
	shrb   %cl,%bl
	shrb   %cl,%bh
	shrb   %cl,%dl
	shrb   %cl,%dh
	movl   0x30(%esp,1),%esi
	movl   0x34(%esp,1),%edi
	movb   0x28(%esp,1),%cl
0:	xorl   %eax,%eax
	rcrb   %bl
	rcll   %eax
	rcrb   %bh
	rcll   %eax
	rcrb   %dl
	rcll   %eax
	rcrb   %dh
	rcll   %eax
	movb   (%eax,%esi,1),%al
	movb   %al,(%edi)
	incl   %edi
	decb   %cl
	jne    0b
	popl   %edx
	popl   %ecx
	popl   %ebx
	popl   %edi
	popl   %esi
	ret

	.globl SYMNAME(blit_4_8_czt_rev)
SYMLABEL(blit_4_8_czt_rev)
	pushl  %esi
	pushl  %edi
	pushl  %ebx
	pushl  %ecx
	pushl  %edx
	movb   0x18(%esp,1),%bl
	movb   0x1c(%esp,1),%bh
	movb   0x20(%esp,1),%dl
	movb   0x24(%esp,1),%dh
	movb   0x2c(%esp,1),%cl
	shrb   %cl,%bl
	shrb   %cl,%bh
	shrb   %cl,%dl
	shrb   %cl,%dh
	movl   0x30(%esp,1),%esi
	movl   0x34(%esp,1),%edi
	movb   0x28(%esp,1),%cl
0:	xorl   %eax,%eax
	rcrb   %bl
	rcll   %eax
	rcrb   %bh
	rcll   %eax
	rcrb   %dl
	rcll   %eax
	rcrb   %dh
	rcll   %eax
	orb    %al, %al
	jz     1f
	movb   (%eax,%esi,1),%al
	movb   %al,(%edi)
1:	incl   %edi
	decb   %cl
	jne    0b
	popl   %edx
	popl   %ecx
	popl   %ebx
	popl   %edi
	popl   %esi
	ret

/*
 * $Log: blitters.S,v $
 * Revision 1.1  1999/02/21 23:32:34  nyef
 * Initial revision
 *
 */
