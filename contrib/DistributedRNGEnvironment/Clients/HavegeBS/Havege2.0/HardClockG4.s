.macro mul
	mul$0 $1,$2,$3
.endmacro
.macro div
	div$0 $1,$2,$3
.endmacro
.text
	.align 2
.globl _HardClock
_HardClock:
	mftb r3
	blr
