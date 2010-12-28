	.file	"HardClock.c"
	.pred.safe_across_calls p1-p5,p16-p63
.text
	.align 16
	.global HardClock#
	.proc HardClock#
HardClock:
	.prologue
	.body
	mov r8 = ar44
# edited by A. Seznec
	br.ret.sptk.many b0
	.endp HardClock#
	.ident	"GCC: (GNU) 2.96 20000731 (Red Hat Linux 7.1 2.96-101)"
