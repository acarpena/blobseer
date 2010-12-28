#ifdef MAINC

#include <stdio.h>
#include <stdlib.h>
#include "sbs_client.h"
#include "sbs_server.h"

#ifdef MPI
#	include "mpi.h"
#endif

extern void cm1_(void);

sbs_client handler;
int client_id;

int myprint_(char* s, int l)
{
	/*
	char* s2 = malloc(l+1);
	memcpy(s2,s,l);
	s2[l] = '\0';
	fprintf(stderr,"%s\n",s2);
	free(s2);*/
	return 1;
}

#ifdef MPI
MPI_Fint fcomm;
#endif

int get_client_id_(int* i)
{
	*i = client_id;
	return 1;
}

#ifdef MPI
int get_registered_comm_(MPI_Fint *c)
{
	*c = fcomm;
#else
int get_registered_comm_(int *c)
{
	*c = 0;
#endif
	return 1;
}

#define NSBS 5
int get_registered_sbs_handler_(uint64_t* h)
{
	*h = (uint64_t)handler;
	return 1;
}

int main(int argc, char** argv)
{
#ifdef MPI
	MPI_Init(&argc,&argv);
	int rank, size;

	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);

	MPI_Comm newcom;
	MPI_Comm_split(MPI_COMM_WORLD,(rank%NSBS == 0),0,&newcom);
	
	if(rank % NSBS == 0) {
		sbs_server_main(newcom);
		MPI_Barrier(MPI_COMM_WORLD);
	} else {
		sleep(4);
		fcomm = MPI_Comm_c2f(newcom);
		int newrank;
		MPI_Comm_rank(newcom,&newrank);
		client_id = newrank%(NSBS-1);
		handler = sbs_initialize(newrank%(NSBS-1));
		cm1_();
		printf("cm1 terminated\n");
		sbs_finalize(handler);
		MPI_Barrier(MPI_COMM_WORLD);
	}
	MPI_Finalize();
#else
	cm1_();
#endif
	return 0;
}

#endif