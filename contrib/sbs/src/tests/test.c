#include "mpi.h" 
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <math.h> 
#include "sbs_mailbox.h"

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
	sbs_mailbox mb = sbs_mailbox_create();
	if(mb == NULL) exit(0);
	printf("task 0 : mailbox created.\n");
	
	printf("task 0 : sending sem key %d...\n",mb->sem_id);
	MPI_Send(&(mb->sem_id), 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
	printf("task 0 : key sent !\n");

	MPI_Send(&(mb->mem_id), 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
	printf("task 0 : key send !\n");

	printf("task 0 : sending \"matthieu\" through shared memory...\n");
	uint64_t offset;
	sbs_mailbox_write(mb,"matthieu",9,&offset);
	printf("task 0 : done ! offset is %d\n",(int)offset);

	printf("task 0 : sending \"lucas\" through shared memory...\n");
	sbs_mailbox_write(mb,"lucas",6,&offset);
	printf("task 0 : done ! offset is %d\n",(int)offset);

	sleep(10);

	sbs_mailbox_delete(mb);
	printf("task 0 : shared mailbox deleted.\n");
}

void second_actor() {
	int mem_key;
	int sem_key;
	MPI_Status status;

	printf("task 1 : waiting for mem key...\n");
	MPI_Recv(&sem_key, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,&status);
	printf("task 1 : sem key received ! key is %d\n",sem_key);
	MPI_Recv(&mem_key, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,&status);
	printf("task 1 : mem key received ! key is %d \n",mem_key);
	sbs_mailbox mb = sbs_mailbox_init(sem_key,mem_key);
	if(mb == NULL) {sleep(15); exit(0);}
	while(mb->shared_contents->offset == 0)
		sleep(0.5);
	
	char buffer[30];
	sbs_mailbox_read(mb,buffer,29,0);
	printf("task 1 : reading %s\n",buffer);
	printf("task 1 : %s\n",&(buffer[9]));	
}
