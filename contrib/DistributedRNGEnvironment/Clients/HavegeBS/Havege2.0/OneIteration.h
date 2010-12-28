#include "Copyright.h"
#ifndef ENDOFFILE

/* 
In average , one iteration accesses 2 8-word blocks in the Walk table, 
and generate  16 words in the RESULT array. 


The data read on  the Walk table are updated and permuted after each use.
The result of the hardware clock counter read is used for this update.

21 conditional tests are present and two  calls to HardClock(). The 
conditional tests are grouped in two nested  groups of ten conditional tests 
and one test  that controls the permutation.

In average, there should be 4 tests executed and in average 2 of them should 
be mispredicted.

*/



{
  int inter;
  PTtest = PT >> 20;
  if (PTtest & 1)
    {
      PTtest ^= 3;
      PTtest = PTtest >> 1;
      if (PTtest & 1)
	{
	  PTtest ^= 3;
	  PTtest = PTtest >> 1;
	  if (PTtest & 1)
	    {
	      PTtest ^= 3;
	      PTtest = PTtest >> 1;
	      if (PTtest & 1)
		{
		  PTtest ^= 3;
		  PTtest = PTtest >> 1;
		  if (PTtest & 1)
		    {
		      PTtest ^= 3;
		      PTtest = PTtest >> 1;
		      if (PTtest & 1)
			{
			  PTtest ^= 3;
			  PTtest = PTtest >> 1;
			  if (PTtest & 1)
			    {
			      PTtest ^= 3;
			      PTtest = PTtest >> 1;
			      if (PTtest & 1)
				{
				  PTtest ^= 3;
				  PTtest = PTtest >> 1;
				  if (PTtest & 1)
				    {
				      PTtest ^= 3;
				      PTtest = PTtest >> 1;
				      if (PTtest & 1)
					{
					  PTtest ^= 3;
					  PTtest = PTtest >> 1;
					}
				    }
				}
			    }
			}
		    }
		}
	    }
	}
    };
  PTtest = PTtest >> 1;
  pt = (PT >> 18) & 7;
  PT = PT & ANDPT;
  HARD= HardClock();  
  Pt0 = &Walk[PT];
  Pt1 = &Walk[PT2];
  Pt2 = &Walk[PT ^ 1];
  Pt3 = &Walk[PT2 ^ 4];
  RESULT[i] ^= *Pt0;
  RESULT[i + 1] ^= *Pt1;
  RESULT[i + 2] ^= *Pt2;
  RESULT[i + 3] ^= *Pt3;
  i = i + 4;

  inter = (*Pt0 >> (1)) ^ (*Pt0 << (31)) ^ HARD;
  *Pt0 = (*Pt1 >> (2)) ^ (*Pt1 << (30)) ^ HARD;
  *Pt1 = inter;
  *Pt2 = (*Pt2 >> (3)) ^ (*Pt2 << (29)) ^ HARD;
  *Pt3 = (*Pt3 >> (4)) ^ (*Pt3 << (28)) ^ HARD;

  Pt0 = &Walk[PT ^ 2];
  Pt1 = &Walk[PT2 ^ 2];
  Pt2 = &Walk[PT ^ 3];
  Pt3 = &Walk[PT2 ^ 6];
  RESULT[i] ^= *Pt0;
  RESULT[i + 1] ^= *Pt1;
  RESULT[i + 2] ^= *Pt2;
  RESULT[i + 3] ^= *Pt3;
  i = i + 4;

  if (PTtest & 1)
    {
      int *Ptinter;
      Ptinter = Pt0;
      Pt2 = Pt0;
      Pt0 = Ptinter;
    }
  PTtest= (PT2 >> 18);
  inter = (*Pt0 >> (5)) ^ (*Pt0 << (27)) ^ HARD;
  *Pt0 = (*Pt1 >> (6)) ^ (*Pt1 << (26)) ^ HARD;
  *Pt1 = inter;
  HARD= HardClock();
  *Pt2 = (*Pt2 >> (7)) ^ (*Pt2 << (25)) ^ HARD;
  *Pt3 = (*Pt3 >> (8)) ^ (*Pt3 << (24)) ^ HARD;


  Pt0 = &Walk[PT ^ 4];
  Pt1 = &Walk[PT2 ^ 1];

  PT2 = (RESULT[(i - 8) ^ pt2] ^ Walk[PT2 ^ pt2 ^ 7]);  
  PT2 = ((PT2 & ANDPT) & (0xfffffff7)) ^ ((PT ^ 8) & 0x8);
/* avoid PT and PT2 points on the same cache block*/
  pt2 = ((PT2 >> 28) & 7);  
  if (PTtest & 1)
    {
      PTtest ^= 3;
      PTtest = PTtest >> 1;
      if (PTtest & 1)
	{
	  PTtest ^= 3;
	  PTtest = PTtest >> 1;
	  if (PTtest & 1)
	    {
	      PTtest ^= 3;
	      PTtest = PTtest >> 1;
	      if (PTtest & 1)
		{
		  PTtest ^= 3;
		  PTtest = PTtest >> 1;
		  if (PTtest & 1)
		    {
		      PTtest ^= 3;
		      PTtest = PTtest >> 1;
		      if (PTtest & 1)
			{
			  PTtest ^= 3;
			  PTtest = PTtest >> 1;
			  if (PTtest & 1)
			    {
			      PTtest ^= 3;
			      PTtest = PTtest >> 1;
			      if (PTtest & 1)
				{
				  PTtest ^= 3;
				  PTtest = PTtest >> 1;
				  if (PTtest & 1)
				    {
				      PTtest ^= 3;
				      PTtest = PTtest >> 1;
				      if (PTtest & 1)
					{
					  PTtest ^= 3;
					  PTtest = PTtest >> 1;
					}
				    }
				}
			    }
			}
		    }
		}
	    }
	}
    };

  Pt2 = &Walk[PT ^ 5];
  Pt3 = &Walk[PT2 ^ 5];
  RESULT[i] ^= *Pt0;
  RESULT[i + 1] ^= *Pt1;
  RESULT[i + 2] ^= *Pt2;
  RESULT[i + 3] ^= *Pt3;
  i = i + 4;
  inter = (*Pt0 >> (9)) ^ (*Pt0 << (23)) ^ HARD;
  *Pt0 = (*Pt1 >> (10)) ^ (*Pt1 << (22)) ^ HARD;
  *Pt1 = inter;
  *Pt2 = (*Pt2 >> (11)) ^ (*Pt2 << (21)) ^ HARD;
  *Pt3 = (*Pt3 >> (12)) ^ (*Pt3 << (20)) ^ HARD;
  Pt0 = &Walk[PT ^ 6];
  Pt1 = &Walk[PT2 ^ 3];
  Pt2 = &Walk[PT ^ 7];
  Pt3 = &Walk[PT2 ^ 7];
  RESULT[i] ^= *Pt0;
  RESULT[i + 1] ^= *Pt1;
  RESULT[i + 2] ^= *Pt2;
  RESULT[i + 3] ^= *Pt3;
  i = i + 4;
  inter = (*Pt0 >> (13)) ^ (*Pt0 << (19)) ^ HARD;
  *Pt0 = (*Pt1 >> (14)) ^ (*Pt1 << (18)) ^ HARD;
  *Pt1 = inter;
  *Pt2 = (*Pt2 >> (15)) ^ (*Pt2 << (17)) ^ HARD;
  *Pt3 = (*Pt3 >> (16)) ^ (*Pt3 << (16)) ^ HARD;
/* avoid PT and PT2 points on the same cache block*/
  PT = (((RESULT[(i - 8) ^ pt] ^ Walk[PT ^ pt ^ 7])) & (0xffffffef)) ^ ((PT2 ^ 0x10) & 0x10) ;

}
#endif
