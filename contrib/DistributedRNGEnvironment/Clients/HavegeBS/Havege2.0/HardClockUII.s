
	.section	".text",#alloc,#execinstr
	.section	".bss",#alloc,#write
	.common	HARDTICK,4,4
	.section	".text",#alloc,#execinstr
       	.global HardClock
        HardClock:
	retl	
	rd %tick, %o0
		.type	HardClock,2
		.size	HardClock,(.-HardClock)

