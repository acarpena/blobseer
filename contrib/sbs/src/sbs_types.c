#include "sbs_types.h"

/* returns the size of SBS types */
int sbs_sizeof(int type)
{
	switch(type) {
	case SBS_CHAR:
		return sizeof(char);
	case SBS_SHORT:
		return sizeof(short);
	case SBS_INT:
		return sizeof(int);
	case SBS_LONG:
		return sizeof(long);
	case SBS_FLOAT:
		return sizeof(float);
	case SBS_DOUBLE:
		return sizeof(double);
	default:
		return 0;
	}
}
