	.file	"HardTickSimp.c"
	.version	"01.01"
gcc2_compiled.:
.text
	.align 4
.globl HardClock
	.type	 HardClock,@function
HardClock:
	pushq %rbp
	movq %rsp,%rbp
#APP
	rdtsc
#NO_APP
	leave
	ret
.Lfe1:
	.size	 HardClock,.Lfe1-HardClock
	.ident	"GCC: (GNU) egcs-2.91.66 19990314/Linux (egcs-1.1.2 release)"
