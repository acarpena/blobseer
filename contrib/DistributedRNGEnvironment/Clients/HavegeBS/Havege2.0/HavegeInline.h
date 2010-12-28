#define HAVEGE_NDSIZECOLLECT 0x100000
/* 4 Mbytes are collected in a pass*/
#ifdef HAVEGE_INIT
int HAVEGE_ndpt = 2 * HAVEGE_NDSIZECOLLECT;
#else
int HAVEGE_ndpt;
#endif
int HAVEGE_BigArray[HAVEGE_NDSIZECOLLECT + 16384];

#define ndrand()  ((HAVEGE_ndpt >= HAVEGE_NDSIZECOLLECT) ? HAVEGE_compute() : HAVEGE_BigArray[HAVEGE_ndpt++])

