#include "mpi.h" 
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <math.h> 
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/types.h>

#define NUM_THREADS 4

#include "sbs_types.h"
#include "sbs_client.h"
#include "sbs_server.h"

void server();
void client();
void *client_thread(void *t);

int main( int argc, char *argv[] ) 
{ 
	int myid, numprocs;

	MPI_Init(&argc,&argv); 
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs); 
	MPI_Comm_rank(MPI_COMM_WORLD,&myid); 
	
	MPI_Comm newcomm;
	MPI_Comm_split(MPI_COMM_WORLD,myid%2,0,&newcomm);
	
	if(myid == 0) {
		server(newcomm);
	} else {
		client(newcomm);
	}

	MPI_Finalize(); 
	return 0; 
} 

void server(MPI_Comm comm)
{
	sbs_server_main(comm);
	MPI_Barrier(MPI_COMM_WORLD);
}

void client(MPI_Comm comm)
{
	
	pthread_t thread[NUM_THREADS];
	
	int rc, t;
	int status;

	for(t=0; t<NUM_THREADS; t++)
	{
		rc = pthread_create(&thread[t],NULL,client_thread,(void*)t);
		if(rc)
		{
			printf("error occured while creating thread\n");
			exit(-1);
		}
	}
	for(t=0; t<NUM_THREADS; t++)
	{
		rc = pthread_join(thread[t],(void **)&status);
		if(rc)
		{
			printf("error occured while joining thread\n");
			exit(-1);
		}
	}
	MPI_Barrier(MPI_COMM_WORLD);
}

void *client_thread(void *t)
{	
	sleep(2);
	int tid = (int)t;
	printf("new thread created with id %d\n",tid);
	sbs_client handler = sbs_initialize(tid);
	
	float mydata[10][10][10];
	int x,y,z;
	for(x=0;x<10;x++)
	for(y=0;y<10;y++)
	for(z=0;z<10;z++)
		mydata[x][y][z] = tid*1.0;
	
	size_t lsize[3] = {10,10,10};
	uint64_t offsets[3] = {tid/2,tid%2,0};
	
	sbs_write(handler,"temperature",mydata,SBS_FLOAT,3,offsets,lsize);
	sleep(5);
	sbs_finalize(handler);

	pthread_exit(NULL);
}
