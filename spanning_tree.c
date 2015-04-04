/*
* @author Breno W. Carvalho
* @author Anna K. Rangel
*
* This code is a simple implementation of the Concurrent Initialization Spanning Tree Algorithm
* In this algortihm some nodes (chosen by the function isRoot) elect themselves as root nodes and try to create their own tree over the network, the node with SMALLEST rank will eventually build its tree with each node in the network.
*/
#include <mpi.h>
#include <stdio.h>

typedef int msg_type;

int isRoot(int rank){
	return !(rank % 3);
}

int main (int argc, char* argv[])
{
	const int QUERY  = 0;
	const int ACCEPT = 1;
	const int REJECT = 2;

  int world_rank, world_size;
	int neighbours = 0, received = 0;
	msg_type parent = -1, my_root = -1;

  MPI_Init (&argc, &argv);      /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &world_rank);        /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &world_size);        /* get number of processes */

  //printf( "Spanning Tree Algorithm: Process %d of %d\n", world_rank, world_size );

	if (isRoot(world_rank)) {
		my_root = world_rank;
	  //Send query to every neighbour
	  for(int i = 0; i < world_size; i++){
		  if(i != world_rank){
		    MPI_Send(&my_root, 1, MPI_INT, i, QUERY, MPI_COMM_WORLD);
				neighbours+=1;
				printf("Process %2d send QUERY(root=%2d) to %2d\n", world_rank, my_root, i);
      }
		}
		if(neighbours == 0){
			printf("Process %2d has no neighbours\n", world_rank);
			MPI_Finalize();
			return 0;
		}
	}

	msg_type msg;
	MPI_Status status;
	while(1){
		MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		//printf("\n**Process %d received %d from %d\n\n", world_rank, status.MPI_TAG, status.MPI_SOURCE);
		if(status.MPI_TAG == QUERY){
		  if (msg < my_root || my_root < 0){
				my_root = msg;
				parent = status.MPI_SOURCE;
				neighbours = received = 0;
				for(int i = 0; i < world_size; i++){
					if((i != world_rank) && (i != parent)){
						MPI_Send(&my_root, 1, MPI_INT, i, QUERY, MPI_COMM_WORLD);
						neighbours+=1;
						printf("Process %2d send  QUERY(root=%2d) to %2d\n", world_rank, my_root, i);
				  }
				}
				if(!neighbours){
					MPI_Send(&my_root, 1, MPI_INT, status.MPI_SOURCE, ACCEPT, MPI_COMM_WORLD);
					printf("Process %2d send ACCEPT(root=%2d) to %2d\n", world_rank, my_root, status.MPI_SOURCE);
					break;
				}
		  }else{
				MPI_Send(&my_root, 1, MPI_INT, status.MPI_SOURCE, REJECT, MPI_COMM_WORLD);
				printf("Process %2d send REJECT(root=%2d) to %2d\n", world_rank, my_root, status.MPI_SOURCE);
			}
		}else if(status.MPI_TAG == ACCEPT){
			if(msg == my_root){
				received+=1;
				//printf("Process %d has received: %d and sent %d\n", world_rank, received, neighbours);
		    if(received == neighbours){
					if(parent >= 0){
						MPI_Send(&my_root, 1, MPI_INT, parent, ACCEPT, MPI_COMM_WORLD);
						printf("Process %2d send ACCEPT(root=%2d) to %2d\n", world_rank, my_root, parent);
					}
					break;
				}
			}
		}else if(status.MPI_TAG == REJECT){
			if(msg == my_root){
				received+=1;
				//printf("Process %d has received: %d and sent %d\n", world_rank, received, neighbours);
				if(received == neighbours){
					if(parent >= 0){
						MPI_Send(&my_root, 1, MPI_INT, parent, ACCEPT, MPI_COMM_WORLD);
						printf("Process %2d send ACCEPT(root=%2d) to %2d\n", world_rank, my_root, parent);
					}
					break;
				}
			}
		}
	}
	MPI_Finalize();
}
