/* C Example */
#include <mpi.h>
#include <stdio.h>


// Message types:
//   Query
//   Accept
//   Reject

int main (int argc, char* argv[])
{
  //char[] = "QUERY";
  //char[] = "ACCEPT";
  //char[] = "REJECT";
  int world_rank, world_size;

  MPI_Init (&argc, &argv);      /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &world_rank);        /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &world_size);        /* get number of processes */
  printf( "Spanning Tree Algorithm: Process %d of %d\n", world_rank, world_size );

	char[] token;
	if (world_rank != 0) {
		  MPI_Recv(token, 10, MPI_BYTE, world_rank - 1, 0,
		           MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		  printf("Process %s received token %d from process %d\n",
		         world_rank, token, world_rank - 1);
	} else {
		  // Set the token's value if you are process 0
		  //token = -1;
	}
	MPI_Send("query", 10, MPI_BYTE, (world_rank + 1) % world_size,
		       0, MPI_COMM_WORLD);

	// Now process 0 can receive from the last process.
	if (world_rank == 0) {
		  MPI_Recv(&token, 10, MPI_BYTE, world_size - 1, 0,
		           MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		  printf("Process %d received token %s from process %d\n",
		         world_rank, token, world_size - 1);
	}

  MPI_Finalize();
  return 0;
}
