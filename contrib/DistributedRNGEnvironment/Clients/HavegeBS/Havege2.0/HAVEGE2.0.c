#include "Copyright.h"
#include <stdio.h>
#include "PersoArchHAVEGE2.0.h"
#include "HAVEGE2.0.h"
#define HAVEGE_INIT

#ifndef NDINLINE
#define HAVEGE_NDSIZECOLLECT 0x100000
/* 4 Mbytes are collected in a pass*/
static int HAVEGE_ndpt = 2 * HAVEGE_NDSIZECOLLECT;
static int HAVEGE_BigArray[HAVEGE_NDSIZECOLLECT + 16384];
#else
#include "HavegeInline.h"
#endif



#ifndef MININITRAND
#define MININITRAND 32
#endif
#define CRYPTOSIZECOLLECT 0x40000
/* 1 Mbyte are collected in a pass*/
static int HAVEGE_cryptondpt = 2 * HAVEGE_NDSIZECOLLECT;
static int HAVEGE_BigArrayCrypto[CRYPTOSIZECOLLECT + 16384];


#ifndef NBSTEPHIDING
#define NBSTEPHIDING 32
#endif
static int mininitrand = MININITRAND;
static int nbstephiding = NBSTEPHIDING;
static int PT, pt, PT2, pt2;
static int *Walk;
static int walk[ANDPT + 4097];
static int HARD;


inithavegeparameters (int stepinit, int stephiding)
{
/*
use for overriding implicit parameters for initialization:
stepinit:

8 HAVEGE_COLLECTNDRAND steps are initiated for ndrand();
32 HAVEGE_COLLECTNDRAND steps are initiated for cryptondrand();
2 HAVEGE_COLLECTNDRAND steps are sufficient in practice from our experiments

nbstephiding:
Hiding the internal state is important in cryptondrand().
To collect data, 32 HAVEGE_COLLECTNDRAND steps are used in cryptondrand().
2 HAVEGE_COLLECTNDRAND steps are sufficient in practice.
*/
  mininitrand = stepinit;
  nbstephiding = stephiding;
}

inithavege (int *ARRAY, int SIZE, int *ipt)
{
  int i;
  Walk = &walk[4096];
#ifndef ADR64
  Walk = (int *) (((int) Walk) & 0xfffff000);
#else
  Walk = (int *) (((long long) Walk) & 0xfffffffffffff000);
#endif

  for (i = 0; i < (mininitrand/(SIZE/CRYPTOSIZECOLLECT)); i++)
    {
      HAVEGE_COLLECTNDRAND (ARRAY, SIZE);
    }
*ipt=0;
}

/*  users concerned with performance of their random number generator can
inline ndrand() (see the example in mainspeed.c)
average execution time of ndrand() is reduced by about +- one third depending 
on the architecture */

#ifndef NDINLINE
int
ndrand ()
{
  if (HAVEGE_ndpt >= HAVEGE_NDSIZECOLLECT)
    {
      if (HAVEGE_ndpt >= 2*HAVEGE_NDSIZECOLLECT)
	inithavege (HAVEGE_BigArray, HAVEGE_NDSIZECOLLECT, &HAVEGE_ndpt);
      else
	{
	  HAVEGE_COLLECTNDRAND (HAVEGE_BigArray, HAVEGE_NDSIZECOLLECT);
	  HAVEGE_ndpt = 0;
	}
    }
  return (HAVEGE_BigArray[HAVEGE_ndpt++]);
}
#else
int
HAVEGE_compute ()
{
  
      if (HAVEGE_ndpt >= 2*HAVEGE_NDSIZECOLLECT)
	inithavege (HAVEGE_BigArray, HAVEGE_NDSIZECOLLECT, &HAVEGE_ndpt);
      else
	{
	  HAVEGE_COLLECTNDRAND (HAVEGE_BigArray, HAVEGE_NDSIZECOLLECT);
	  HAVEGE_ndpt = 0;
	}
  return (HAVEGE_BigArray[HAVEGE_ndpt++]);
}
#endif


int
cryptondrand ()
{
  int i;
  if (HAVEGE_cryptondpt >= CRYPTOSIZECOLLECT)
    {
      if (HAVEGE_cryptondpt >= 2*HAVEGE_NDSIZECOLLECT)
	inithavege (HAVEGE_BigArrayCrypto, CRYPTOSIZECOLLECT, &HAVEGE_cryptondpt);
      else
	{
	  for (i = 0; i < nbstephiding; i++)
	    HAVEGE_COLLECTNDRAND (HAVEGE_BigArrayCrypto, CRYPTOSIZECOLLECT);
	  HAVEGE_cryptondpt = 0;
	}
    }
  return (HAVEGE_BigArrayCrypto[HAVEGE_cryptondpt++]);
}

HAVEGE_COLLECTNDRAND (int *RESULT, int SIZE)
{
  int i, in,X;
int *Pt0, *Pt1, *Pt2 , *Pt3;
int PTtest;
  i = 0;
  while (i < SIZE)
    {
#include "LoopBody.h"
    }
  return (X);
}
