#include "mpi.h" 
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <math.h> 
#include "sbs_client.h"
#include "sbs_server.h"

void first_actor();
void second_actor();

int main( int argc, char *argv[] ) 
{ 
    int myid, numprocs;
 
    MPI_Init(&argc,&argv); 
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs); 
    MPI_Comm_rank(MPI_COMM_WORLD,&myid); 
	
    if(myid == 0) {
	first_actor();
    } else {
	second_actor();
    }

    MPI_Finalize(); 
    return 0; 
} 

void first_actor() {
	sbs_server_main(MPI_COMM_WORLD, MPI_COMM_NULL);
}

void second_actor() {
	sbs_client client = sbs_client_init(MPI_COMM_WORLD, 0);
	sleep(10);
}
