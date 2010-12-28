# See Copyright in file Copyright.h
# Advertisement: changing the compiler and the compiler options  
# would necessitate to recalibrate the unrolling factors in the main loop (LoopBody.h)

#Arch= UII or UIII or PIII or ATHLON or P4 or ITANIUM or G4
include CCOPT${Arch}
all${Arch}: HAVEGEND${Arch} HAVEGECRYPTOND${Arch} HAVEGESPEED${Arch}

allINLINE${Arch}: HAVEGESPEEDINLINE${Arch}



HAVEGEND${Arch}:  HAVEGE2.0${Arch}.o HardClock${Arch}.s   HardTick.c mainnd.c PackTest.c cephes.c matrix.c dfft.c special-functions.c   
	${CC} ${CCOPT} HAVEGE2.0${Arch}.o -D${Arch}    HardClock${Arch}.s   HardTick.c mainnd.c PackTest.c cephes.c matrix.c dfft.c special-functions.c   -lm   -o HAVEGEND${Arch}

HAVEGECRYPTOND${Arch}:  HAVEGE2.0${Arch}.o HardClock${Arch}.s   HardTick.c maincryptond.c PackTest.c cephes.c matrix.c dfft.c special-functions.c   
	${CC} ${CCOPT} HAVEGE2.0${Arch}.o -D${Arch}    HardClock${Arch}.s   HardTick.c maincryptond.c PackTest.c cephes.c matrix.c dfft.c special-functions.c   -lm   -o HAVEGECRYPTOND${Arch}

HAVEGE2.0${Arch}.o: HAVEGE2.0.c PersoArchHAVEGE2.0.h LoopBody.h  OneIteration.h
	${CC} -c ${CCOPT} HAVEGE2.0.c  -D${Arch} 
	mv HAVEGE2.0.o  HAVEGE2.0${Arch}.o

HAVEGE2.0inline${Arch}.o: HAVEGE2.0.c PersoArchHAVEGE2.0.h LoopBody.h  OneIteration.h HavegeInline.h
	${CC} -c ${CCOPT} -DNDINLINE  HAVEGE2.0.c  -D${Arch} 
	mv HAVEGE2.0.o  HAVEGE2.0inline${Arch}.o

HAVEGESPEED${Arch}:   HAVEGE2.0${Arch}.o HardClock${Arch}.s   HardTick.c mainspeed.c 
	${CC} ${CCOPT} HAVEGE2.0${Arch}.o -D${Arch}    HardClock${Arch}.s   HardTick.c mainspeed.c   -o HAVEGESPEED${Arch}

HAVEGESPEEDINLINE${Arch}:   HAVEGE2.0inline${Arch}.o HardClock${Arch}.s   HardTick.c mainspeed.c
	${CC} ${CCOPT} -DNDINLINE HAVEGE2.0inline${Arch}.o  -D${Arch}    HardClock${Arch}.s   HardTick.c mainspeed.c   -o HAVEGESPEEDINLINE${Arch}

