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

#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <math.h> 
#include <pthread.h>
#include <sys/types.h>
#include "sbs_variable.h"
#include "sbs_writer.h"

#define NUM_THREADS 4
#define TIME_TO_PROCESS 1
#define NB_ITERATIONS 50

#include "mpi.h"
#include "sbs_types.h"
#include "sbs_common.h"
//#include "sbs_client.h"
//#include "sbs_server.h"

/* the server function takes a communicator to comunicate
with all the other SBS servers */
//void server(MPI_Comm comm);
/* in case the computations nodes need one, we pass a
communicatore to our client too */
void client();
/* the client thread is defined and will take the tid as
first argument, as we need it to get a client-id on SBS
server */
void *client_thread(void *t);

int main( int argc, char *argv[] ) 
{ 
	client();
	return 0; 
} 

void client()
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
}

/* this is the client thread */
void *client_thread(void *t)
{	
	int tid = (int)t;
	printf("new thread created with id %d\n",tid);
	/* here is the data we will write */
	float mydata[10][10][10];
	int x,y,z;
	for(x=0;x<10;x++)
	for(y=0;y<10;y++)
	for(z=0;z<10;z++)
		mydata[x][y][z] = 1.0+tid*1.0;
	/* its size along each dimension */
	size_t lsize[3] = {10,10,10};

	sbs_variable v  = malloc(sizeof(struct sbs_variable_s));
	v->name = "temperature";
	v->type = SBS_FLOAT;
	v->dim = 3;
	v->data = (char*)mydata;
	v->lsizes = lsize;
	v->gsizes = lsize;
	
	/* its offset within the intermediate subdomain
	(subdomain handled by the underlying MPI task.
	This MPI task knows the global domain's parameters
	through its configuration */
	uint64_t offsets[3] = {10*(tid/2),10*(tid%2),0};
	
	int i;
	gettime();
	for(i=0;i<NB_ITERATIONS;i++) {
		sleep(TIME_TO_PROCESS);	
		
		char fname[256];
		sprintf(fname,"temp%d_%d.h5",tid,i);
		sbs_write_hdf5(MPI_COMM_NULL,v,fname);
	}
	double time = gettime();
	printf("----------------------------------------\n");
	printf(" end of journey : %lf\n",time);
	printf("----------------------------------------\n");
	/* exit the thread */
	pthread_exit(NULL);
}
