#ifndef SBS_TYPES_H

#define SBS_TYPES_H

/* this defines basic SBS types */

enum SBS_TYPE {

	SBS_CHAR   = 0,

	SBS_SHORT  = 1,

	SBS_INT    = 2,

	SBS_LONG   = 3,

	SBS_FLOAT  = 4,

	SBS_DOUBLE = 5	

};



int sbs_sizeof(int type);

#endif

