#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include <math.h> 

#include <pthread.h>

#include <sys/types.h>

#include <errno.h>



#include "mpi.h"

#include "sbs_client.h"

#include "sbs_server.h"

extern int errno;

extern void example_(int64_t* handler, MPI_Fint* comm);

/* the server function takes a communicator to comunicate

with all the other SBS servers */

void server(MPI_Comm comm);

/* in case the computations nodes need one, we pass a

communicatore to our client too */

void client(MPI_Comm comm);



int main( int argc, char *argv[] ) 

{ 

	int myid, numprocs;

	/* First doing some MPI initializations */

	MPI_Init(&argc,&argv); 

	MPI_Comm_size(MPI_COMM_WORLD,&numprocs); 

	MPI_Comm_rank(MPI_COMM_WORLD,&myid); 

	/* we split MPI_COMM_WORLD to get sperate SBS servers

	and computation tasks */

	MPI_Comm newcomm;

	MPI_Comm_split(MPI_COMM_WORLD,myid%2,0,&newcomm);

	

	if(myid%2 == 0) {

		/* starting server */

		server(newcomm);

	} else {

		/* starting client */

		client(newcomm);

	}

	

	/* doing MPI finalization */

	MPI_Finalize(); 

	return 0; 

} 



void server(MPI_Comm comm)

{

	/* the server function is very simple,

	it just start the server main loop */

	sbs_server_main(comm);

	/* after stoping the server main loop,

	we should wait for the other tasks */

	MPI_Barrier(MPI_COMM_WORLD);

}



void client(MPI_Comm comm)

{

	sleep(3);

	int tid = 0;

	sbs_client handler = sbs_initialize(tid);

        int64_t handlerptr = (int64_t)handler;

	MPI_Fint fcomm = MPI_Comm_c2f(comm);

	example_(&handlerptr,&fcomm);

	sbs_finalize(handler);

	MPI_Barrier(MPI_COMM_WORLD);

}

