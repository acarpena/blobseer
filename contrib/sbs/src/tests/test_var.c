#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "sbs_variable.h"

int main(void)
{
	sbs_variable v1 = (sbs_variable)malloc(sizeof(struct sbs_variable_s));
	sbs_variable v2;

	v1->type = 42;
	v1->dim = 3;
	v1->name = "MyVariable";
	v1->gsizes = (size_t*)malloc(3*sizeof(size_t));
	v1->lsizes = (size_t*)malloc(3*sizeof(size_t));
	v1->offsets = (uint64_t*)malloc(3*sizeof(uint64_t));
	v1->gsizes[0] = 5; v1->gsizes[1] = 6; v1->gsizes[2] = 7;
	v1->lsizes[0] = 2; v1->lsizes[1] = 3; v1->lsizes[2] = 4;
	v1->offsets[0] = 3; v1->offsets[1] = 4; v1->offsets[2] = 1;
	
	sbs_variable_print(v1);
	
	int s;
	char* tmp = sbs_variable_serialize(v1,&s);
	printf("v1 has been serialized, its size is %d\n",s);
	v2 = sbs_variable_unserialize(tmp);

	sbs_variable_print(v2);
	return 0;
}
