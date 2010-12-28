/* ========================================================

 * This is an example explaining how to use the SBS system 

 * It uses hybrid programming model with pthread and MPI-io

 * The dataspace is a 3D grid of 40x40x10 points, split

 * among a 2D grid of processes: 4 MPI tasks handle each a 

 * 20x20x10 subdomain. In each MPI task, 4 POSIX threads

 * are running to handle a 10x10x10 subdomain.

 * Only one variable is defined: the temperature, which is

 * a float value for each point.

 * On deployement, 8 MPI tasks must be deployed: 4 will be

 * used as computation tasks and will run 4 pthreads, the

 * 4 other MPI tasks are handling a SBS server.

 * The programme write the temperature one time, then stops.

 * =========================================================

 */

#include <errno.h>

#include <stdio.h> 

#include <stdlib.h>

#include <unistd.h>

#include <math.h> 

#include <pthread.h>

#include <sys/types.h>



#define NUM_THREADS 1

#define TIME_TO_PROCESS 1

#define NB_ITERATIONS 3



#include "mpi.h"

#include "sbs_types.h"

#include "sbs_common.h"

#include "sbs_client.h"

#include "sbs_server.h"



/* the server function takes a communicator to comunicate

with all the other SBS servers */

void server(MPI_Comm comm);

/* in case the computations nodes need one, we pass a

communicatore to our client too */

void client(MPI_Comm comm);

/* the client thread is defined and will take the tid as

first argument, as we need it to get a client-id on SBS

server */

void *client_thread(void *t);

extern int errno;

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

	/* this client function will start the

	given number of pthreads */

	pthread_t thread[NUM_THREADS];

	

	int rc, t;

	int status;

	/* starting pthreads... */

	for(t=0; t<NUM_THREADS; t++)

	{

		rc = pthread_create(&thread[t],NULL,client_thread,(void*)t);

		if(rc)

		{

			printf("error occured while creating thread\n");

			exit(-1);

		}

	}

	/* joining pthreads... */

	for(t=0; t<NUM_THREADS; t++)

	{

		rc = pthread_join(thread[t],(void **)&status);

		if(rc)

		{

			printf("error occured while joining thread\n");

			exit(-1);

		}

	}

	/* after completion, wait for the other tasks */

	MPI_Barrier(MPI_COMM_WORLD);

}



/* this is the client thread */

void *client_thread(void *t)

{	

	sleep(2); /* this is needed because we have to wait

	for the shared memory to be initialize by the SBS server */

	int tid = (int)t;

	printf("new thread created with id %d\n",tid);

	sbs_client handler = sbs_initialize(tid);

	/* here is the data we will write */

	float mydata[10][10][10];

	int x,y,z;

	for(x=0;x<10;x++)

	for(y=0;y<10;y++)

	for(z=0;z<10;z++)

		mydata[x][y][z] = 1.0+tid*1.0;

	/* its size along each dimension */

	size_t lsize[3] = {10,10,10};

	/* its offset within the intermediate subdomain

	(subdomain handled by the underlying MPI task.

	This MPI task knows the global domain's parameters

	through its configuration */

	uint64_t offsets[3] = {10*(tid/2),10*(tid%2),0};

	

	int i;

	gettime();

	for(i=0;i<NB_ITERATIONS;i++) {

		sbs_start_step(handler);

		sleep(TIME_TO_PROCESS);	

		sbs_write(handler,"temperature",

			mydata,SBS_FLOAT,3,offsets,lsize);

		sbs_end_step(handler);

	}

	double time = gettime();

	printf("----------------------------------------\n");

	printf(" end of journey : %lf\n",time);

	printf("----------------------------------------\n");



	sleep(5);

	sbs_finalize(handler);

	/* exit the thread */

	pthread_exit(NULL);

}

