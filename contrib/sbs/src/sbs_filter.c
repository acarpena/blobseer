#include <string.h>

#include "sbs_common.h"
#include "sbs_variable.h"
#include "sbs_types.h"
#include "sbs_filter.h"
/***************************************************************************
 FILTER ARE NOT ENABLED IN THIS VERSION OF SBS
 
int identity(const sbs_variable vin, sbs_variable vout);
size_t multidim_memcpy(char* out, char* in, int dim, size_t* size, int type);
 
sbs_filter sbs_filter_from_dlopen() 
{
	// for the moment, we return identity filter
	sbs_filter f = (sbs_filter)malloc(sizeof(sbs_filter));
	f->name = "identity";
	f->filter = identity;
	return f;
}

int identity(const sbs_variable vin, sbs_variable vout)
{
	vout->type = vin->type;
	vout->dim  = vin->dim;
	vout->time = vin->time;
	
	vout->name = (char*)malloc(sizeof(char)*(strlen(vin->name)));
	strcpy(vout->name,vin->name);
	
	vout->gsizes = (size_t*)malloc(sizeof(size_t)*(vin->dim));
	memcpy(vout->gsizes,vin->gsizes,sizeof(size_t)*(vin->dim));

	vout->lsizes = (size_t*)malloc(sizeof(size_t)*(vin->dim));
	memcpy(vout->lsizes,vin->lsizes,sizeof(size_t)*(vin->dim));

	vout->offsets = (uint64_t*)malloc(sizeof(uint64_t)*(vin->dim));
	memcpy(vout->offsets,vin->offsets,sizeof(uint64_t)*(vin->dim));
	*/
	/*	size_t s = sbs_sizeof(vin->type);;
	int i;
	for(i=0; i < vin->dim; i++) s *= vin->lsizes[i];
	memcpy(vin->data,vout->data,s);
	multidim_memcpy(vout->data,vin->data,vin->dim, // TODO

	size_t s;
	int i;
	for(i=0; i < vin->dim; i++)
	{
		
	}	
	return 0;
}
*/
size_t  multidim_memcpy(char* out, char* in, 
		     int dim, size_t* lsizes, size_t* gsizes,
		     uint64_t* offsets, int type_size) 
{
	if(dim == 1) 
	{
		size_t s   = type_size*lsizes[0];
		uint64_t o = type_size*offsets[0];
		//printf("achieved memcpy with size %d\n",s);
		//DEBUG(printf("[memdebug] : achieved memcpy, ptr = %d, size = %d\n",(int)(out+o),s);)
		memcpy(out+o,in,s);
		return s;
	} else {
		int i; 
		uint64_t gchunk = type_size;
		uint64_t lchunk = type_size;
		for(i=1;i<dim;i++)
		{
			gchunk *= gsizes[i];
			lchunk *= lsizes[i];
		}
		uint64_t inoffset = 0;
		uint64_t outoffset = offsets[0]*gchunk;
		size_t res = 0;
		for(i=0;i<lsizes[0];i++)
		{
			res += multidim_memcpy(out+outoffset,in+inoffset, 
		     			dim-1, lsizes+1, gsizes+1,
		     			offsets+1, type_size);
			inoffset += lchunk;
			outoffset += gchunk;
		}
		return res;
	}
}

int sbs_copy(sbs_variable vin, sbs_variable vout)
{
	int dim = vin->dim;
	size_t* lsizes = vin->lsizes;
	size_t* gsizes = vout->lsizes;
	uint64_t* offsets = vin->offsets;
	/*
	int i;
	for(i=0;i<dim/2;i++)
	{
		uint64_t tmp = offsets[i];
		offsets[i] = offsets[dim-i-1];
		offsets[dim-i-1] = tmp;
	}*/
	int type_size = sbs_sizeof(vin->type);	
	return (int)multidim_memcpy(vout->data,vin->data,
			dim, lsizes, gsizes, 
			offsets, type_size);
}

int sbs_ref(sbs_variable vin, sbs_variable vout)
{
	vout->data = vin->data;
	return 0;
	
}
