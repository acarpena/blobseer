
	.section	".text",#alloc,#execinstr
	.file	"HardTickSimp.c"

	.section	".bss",#alloc,#write
	.common	HARDTICK,4,4

	.section	".text",#alloc,#execinstr
/* 000000	   0 */		.align	4
!
! SUBROUTINE HardTick
!
! OFFSET    SOURCE LINE	LABEL	INSTRUCTION

                       	.global HardClock
                       HardClock:
! FILE HardTickSimp.c

!    1		      !int HARDTICK;
!    2		      !int HardTick()
!    3		      !{
!    4		      !HARDTICK= HARDTICK -5;




/* 0x000c	     */		retl	! Result =  %o0
rd %tick, %o0
/* 0x0014	   0 */		.type	HardClock,2
/* 0x0014	     */		.size	HardClock,(.-HardClock)

! Begin Disassembling Stabs
	.xstabs	".stab.index","Xa ; O ; V=3.1 ; R=WorkShop Compilers 5.0 99/12/04 C 5.0 patch 107289-05",60,0,0,0	! (/tmp/acompAAAfcaacV:1)
	.xstabs	".stab.index","/udd/seznec/HIPSOR/UltraSparc; /soft/WorkShop/bin/../SC5.0/bin/cc -xO5 -fast -S  HardTickSimp.c -W0,-xp",52,0,0,0	! (/tmp/acompAAAfcaacV:2)
! End Disassembling Stabs

! Begin Disassembling Ident
	.ident	"cg: WorkShop Compilers 5.0 00/03/28 Compiler Common 5.0 Patch 107357-09"	! (NO SOURCE LINE)
	.ident	"acomp: WorkShop Compilers 5.0 99/12/04 C 5.0 patch 107289-05"	! (/tmp/acompAAAfcaacV:4)
! End Disassembling Ident
