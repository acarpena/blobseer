#include <stdio.h>

#include <stdlib.h>

#include <string.h>



#include "sbs_common.h"

#include "sbs_types.h"

#include "sbs_variable.h"



char* sbs_variable_serialize(sbs_variable v, size_t *size)

{

	int s = 0;

	s += sizeof(int); 
	s += sizeof(int); 
	s += sizeof(uint64_t); 
	s += (v->dim)*sizeof(uint64_t); 
	s += 2*(v->dim)*sizeof(size_t); 
	int len;	

	s += (len = strlen(v->name)+1); 
	

	

	char* buffer = (char*) malloc(s*sizeof(char));

	
	s = 0;

	memcpy(buffer+s,&(v->type), sizeof(int));

	s += sizeof(int);

	
	memcpy(buffer+s,&(v->dim), sizeof(int));

	s += sizeof(int);

	
	memcpy(buffer+s,&(v->dim), sizeof(uint64_t));

	s += sizeof(uint64_t);

	
	strcpy(buffer+s,v->name);

	s += len;

	
	memcpy(buffer+s,v->gsizes, (v->dim)*sizeof(size_t));

	s += (v->dim)*sizeof(size_t);

	
	memcpy(buffer+s,v->lsizes, (v->dim)*sizeof(size_t));

	s += (v->dim)*sizeof(size_t);

	
	memcpy(buffer+s,v->offsets, (v->dim)*sizeof(uint64_t));

	s += (v->dim)*sizeof(uint64_t);

	*size = s;



	return buffer;

}



sbs_variable sbs_variable_unserialize(char* buffer, size_t *size)

{

	sbs_variable v = (sbs_variable)malloc(sizeof(struct sbs_variable_s));

	int s = 0;

	int len;

	

	
	memcpy(&(v->type), buffer+s, sizeof(int));

	s += sizeof(int);

	
	memcpy(&(v->dim), buffer+s, sizeof(int));

	s += sizeof(int);

	
	memcpy(&(v->dim), buffer+s, sizeof(uint64_t));

	s += sizeof(uint64_t);

	
	len = strlen(buffer+s);

	v->name = (char*)malloc((len+1)*sizeof(char));

	strcpy(v->name,buffer+s);

	s += len+1;

	
	v->gsizes  = (size_t*)malloc((v->dim)*sizeof(size_t));

	v->lsizes  = (size_t*)malloc((v->dim)*sizeof(size_t));

	v->offsets = (uint64_t*)malloc((v->dim)*sizeof(uint64_t));

	
	memcpy(v->gsizes,buffer+s,(v->dim)*sizeof(size_t));

	s += (v->dim)*sizeof(size_t);

	
	memcpy(v->lsizes,buffer+s,(v->dim)*sizeof(size_t));

	s += (v->dim)*sizeof(size_t);

	
	memcpy(v->offsets,buffer+s,(v->dim)*sizeof(uint64_t));

	s += (v->dim)*sizeof(uint64_t);

	

	*size = s;

	return v;

}



void sbs_variable_print(sbs_variable v)

{

	printf("name\t:\t%s\n", v->name);

	printf("type\t:\t%d\n", v->type);

	printf("time\t:\t%lu\n",(unsigned long)v->time);

	printf("dims\t:\t%d\n", v->dim);

	printf("gsizes\t:\t[");

	int i;

	for(i=0;i < v->dim;i++) 

		printf("%d%c",v->gsizes[i],(i == v->dim-1) ? ']':',');

	printf("\nlsizes\t:\t[");

	for(i=0;i < v->dim;i++)

		printf("%d%c",v->lsizes[i],(i == v->dim-1) ? ']':',');

	printf("\noffsets\t:\t[");

	for(i=0;i < v->dim;i++)

		printf("%ld%c",(long)(v->offsets[i]),

				(i == v->dim-1) ? ']' : ',');

	printf("\n");

}



void print_item(char* buff, int type, int index);

void sbs_variable_print_data(sbs_variable v)

{

	int dim = v->dim;

	int nbitem = 1;

	int i;

	for(i=0;i<dim;i++) nbitem *= v->lsizes[i];

	

	printf("{");

	for(i=0;i<nbitem;i++)

	{

		if(i != 0) printf(",");

		print_item(v->data,v->type,i);

	}

	printf("}");

	printf("\n");

}



void print_item(char* buff, int type, int index)

{

	char* location = buff + index*sbs_sizeof(type);

	switch(type) {

	case SBS_INT:

		printf("%d",((int*)location)[0]);

		break;

	case SBS_FLOAT:

		printf("%f",((float*)location)[0]);

		break;

	case SBS_LONG:

		printf("%ld",((long int*)location)[0]);

		break;

	case SBS_DOUBLE:

		printf("%lf",((double*)location)[0]);

		break;

	default:

		printf(".");

	}

}

