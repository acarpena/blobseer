#ifndef SBS_VARIABLE_H
#define SBS_VARIABLE_H
#include <stdlib.h>
#include <stdint.h>

typedef struct sbs_variable_s {
	// metadata :
	int type;    // type
	int dim;     // number of dimensions
	uint64_t time;   // iteration
	char* name;  // name of the variable
	// chunk :
	size_t* gsizes;    // global sizes
	size_t* lsizes;    // local sizes
	uint64_t* offsets; // local offset
	// data :
	char* data;  // pointer over the data
} *sbs_variable;

char* sbs_variable_serialize(sbs_variable v, size_t *size);
sbs_variable sbs_variable_unserialize(char* buffer, size_t *size);
void sbs_variable_print(sbs_variable v);
void sbs_variable_print_data(sbs_variable v);
#endif
