#ifndef SBS_FILTER_H
#define SBS_FILTER_H
#include "sbs_variable.h"
/***************************************************************************
  FILTER ARE NOT ENABLED IN THIS VERSION OF SBS
 */
typedef struct sbs_filter_s {
	int (*filter)(const sbs_variable,sbs_variable);
	char* name;
} *sbs_filter;

int sbs_copy(sbs_variable vin, sbs_variable vout);

sbs_filter sbs_filter_from_dlopen(/*TODO*/);
sbs_filter sbs_filter_from_perl(/*TODO*/);
sbs_filter sbs_filter_from_python(/*TODO*/);
sbs_filter sbs_filter_from_ruby(/*TODO*/);
#endif
