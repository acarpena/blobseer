	.file	"HardTick.c"
gcc2_compiled.:
___gnu_compiled_c:
	.comm	_Interrupt, 16	 # 4
	.comm	_OldHARDTICK, 16	 # 4
	.comm	_HARDtick, 16	 # 4
.text
	.align 4
.globl _HardClock
	.def	_HardClock;	.scl	2;	.type	32;	.endef
_HardClock:
	pushl %ebp
	movl %esp,%ebp
#APP
	rdtsc
#NO_APP
	leave
	ret
#.Lfe4:
#	.size	 HARDTICK,.Lfe4-HARDTICK
	.align 4

	




