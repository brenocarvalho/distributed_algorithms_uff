/* C Example */
#include <mpi.h>
#include <stdio.h>
 
int main (int argc, char* argv[])
{
  int rank, size;
 
  MPI_Init (&argc, &argv);      /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */
  printf( "Hello world from process %d of %d\n", rank, size );
	int number;
	if (rank == 0) {
		number = 10;
		MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		//number = -1;
		MPI_Send(&number, 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
	} else if (rank == 1) {
		MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
		         MPI_STATUS_IGNORE);
		printf("Process 1 received number %d from process 0\n",
		         number);
	} else if (rank == 2) {
		MPI_Recv(&number, 1, MPI_INT, 0, 1, MPI_COMM_WORLD,
		         MPI_STATUS_IGNORE);
		printf("Process 2 received number %d from process 0\n",
		         number);
	}

  MPI_Finalize();
  return 0;
}
