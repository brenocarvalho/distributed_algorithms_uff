/*
* @author Breno W. Carvalho
* @author Anna K. Rangel
*
* This code is a simple implementation of the Concurrent Initialization Spanning Tree Algorithm
* In this algortihm some nodes (chosen by the function isRoot) elect themselves as root nodes and try to create their own tree over the network, the node with SMALLEST rank will eventually build its tree with each node in the network.
*/
#include <mpi.h>
#include <stdio.h>
#define MSG_SIZE 2

typedef int msg_type[MSG_SIZE];

int isRoot(int rank){
	return !(rank % 3);
}

int main (int argc, char* argv[])
{
	const int QUERY  = 0;
	const int ACCEPT = 1;
	const int REJECT = 2;
  const int ACK    = 3;

  int world_rank, world_size;
	int neighbours = 0, received = 0;
	int parent = -1, my_root = -1;

  MPI_Init (&argc, &argv);     											  /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &world_rank);        /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &world_size);        /* get number of processes */

  //printf( "Spanning Tree Algorithm: Process %d of %d\n", world_rank, world_size );
msg_type msg;
	if (isRoot(world_rank)) {
		my_root = world_rank;
	  //Send query to every neighbour
		msg[0] = my_root; msg[1] = QUERY;
	  for(int i = 0; i < world_size; i++){
		  if(i != world_rank){
		    MPI_Send(msg, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
				neighbours+=1;
				printf("%2d ---QUERY(%d)---> %2d\n", world_rank, my_root, i);
      }
		}
		if(neighbours == 0){
			printf("Process %2d has no neighbours\n", world_rank);
			MPI_Finalize();
			return 0;
		}
	}

	MPI_Status status;
	while(1){
		MPI_Recv(&msg, 2, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
		printf("** Process %d received %s,from %d\n", world_rank, (msg[0])?"A/R":"Q", status.MPI_SOURCE);
		if(msg[1] == QUERY){
		  if (msg[0] < my_root || my_root < 0){
				my_root = msg[0];
				parent = status.MPI_SOURCE;
				neighbours = received = 0;
				msg[0] = my_root; msg[1] = QUERY;
				for(int i = 0; i < world_size; i++){
					if((i != world_rank) && (i != parent)){
						printf("%2d ---QUERY(%d)---> %2d\n", world_rank, my_root, i);
						MPI_Send(msg, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
						neighbours+=1;
				  }
				}
				if(!neighbours){
					msg[1] = ACCEPT;
					printf("%2d ---ACCEPT(%d)---> %2d\n", world_rank, my_root, status.MPI_SOURCE);
					MPI_Send(msg, 2, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
					break;
				}
		  }else{
				msg[0] = my_root; msg[1] = REJECT;
				printf("%2d ---REJECT(%d)---> %2d\n", world_rank, my_root, status.MPI_SOURCE);
				MPI_Send(msg, 2, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
			}
		}else if(msg[1] == ACCEPT){
			if(msg[0] == my_root){
				received+=1;
				//printf("%d has received: %d and sent %d\n", world_rank, received, neighbours);
		    if(received == neighbours){
					msg[0] = my_root;
					if(parent >= 0){
						msg[1] = ACCEPT;
						printf("%2d ---ACCEPT(%d)---> %2d\n", world_rank, my_root, parent);
						MPI_Send(msg, 2, MPI_INT, parent, 0, MPI_COMM_WORLD);
					}else{
						msg[1] = ACK;
						for(int i = 0; i < world_size; i++){
							if((i != world_rank) && (i != parent)){
								printf("%2d ---ACK(%d)---> %2d\n", world_rank, my_root, i);
								MPI_Send(msg, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
							}
						}
						break;
					}
				}
			}
		}else if(msg[1] == REJECT){
			if(msg[0] == my_root){
				received+=1;
				//printf("Process %d has received: %d and sent %d\n", world_rank, received, neighbours);
				if(received == neighbours){
					if(parent >= 0){
						msg[0] = my_root; msg[1] = ACCEPT;
						printf("%2d ---ACCEPT(%d)---> %2d\n", world_rank, my_root, parent);
						MPI_Send(msg, 2, MPI_INT, parent, 0, MPI_COMM_WORLD);
					}else{
						msg[1] = ACK;
						for(int i = 0; i < world_size; i++){
							if((i != world_rank) && (i != parent)){
								printf("%2d ---ACK(%d)---> %2d\n", world_rank, my_root, i);
								MPI_Send(msg, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
							}
						}
						break;
					}
				}
			}
		}else if(msg[1] == ACK){
			if(status.MPI_SOURCE == parent){
				break;
			}

			msg[1] = ACK;
			for(int i = 0; i < world_size; i++){
				if((i != world_rank) && (i != parent)){
					printf("%2d ---ACK(%d)---> %2d\n", world_rank, my_root, i);
					MPI_Send(msg, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
				}
			}
			break;
		}
	}
	MPI_Finalize();
  return 0;
}
