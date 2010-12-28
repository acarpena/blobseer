#include "mpi.h" 
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <math.h> 
#include "sbs_types.h"
#include "sbs_variable.h"
#include "sbs_writer.h"

int main( int argc, char *argv[] ) 
{ 
	int myrank;
	MPI_Comm comm = MPI_COMM_WORLD;
		
	MPI_Init(&argc,&argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
	char name[] = "temperature";
	sbs_variable v = malloc(sizeof(struct sbs_variable_s));
	float data[2][3] = {{0.0,0.1,0.2},{1.0,1.1,1.2}};
	v->name = name;
	v->type = SBS_FLOAT;
	v->dim = 2;
	v->offsets = (uint64_t*)malloc(sizeof(uint64_t)*(v->dim));
	v->lsizes = (size_t*)malloc(sizeof(size_t)*(v->dim));
	v->gsizes = (size_t*)malloc(sizeof(size_t)*(v->dim));
	v->offsets[0] = v->offsets[1] = 0;
	v->lsizes[0] = v->gsizes[0] = 2;
	v->lsizes[1] = v->gsizes[1] = 3;
	v->data = data;
	
	sbs_writer_adios_init("config.xml");
	sbs_write_adios(comm,v,"test.bp");
	sbs_writer_adios_finalize(comm);
	
	free(v->offsets); free(v->lsizes); free(v->gsizes);	
	MPI_Finalize();
	return 0; 
} 
