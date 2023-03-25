#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int a[];
int arraySize;
int my_array[1000];

int max(int x, int y){
    if(x > y){
        return x;
    }else{
        return y;
    }
}

int main(int argc, char* argv[])
{

	int pid, np, elements_per_process, n_elements_recieved;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	MPI_Comm_size(MPI_COMM_WORLD, &np);

	//----------------------master process---------------------------------
	if (pid == 0) {
        printf("Hello from master process.\n");
        printf("Number of slave processes is %d\n",np-1);
        //Read size of array
        printf("Please enter size of array...\n");
        scanf("%d",&arraySize);
		//Read array
		printf("Please enter array elements...\n");
		for(int i=0;i<arraySize;i++)
		{
			scanf("%d",&a[i]);
		}
		int index, i;
		elements_per_process = arraySize / (np-1);

		if (np > 1) {

			if(arraySize % (np-1) == 0){//array size is divisible by no. of processes
			// distributes the portion of array to child processes to calculate their local_maximums
				for (i = 1; i < np; i++) {
					index = (i-1) * elements_per_process;

					MPI_Send(&elements_per_process,
							1, MPI_INT, i, 2,
							MPI_COMM_WORLD);
					MPI_Send(&a[index],
							elements_per_process,
							MPI_INT, i, 1,
							MPI_COMM_WORLD);
				}//end for

			}else{//array size is not divisible by no. of processes
				for (i = 1; i < np-1; i++) {
					index = (i-1) * elements_per_process;

					MPI_Send(&elements_per_process,
							1, MPI_INT, i, 2,
							MPI_COMM_WORLD);
					MPI_Send(&a[index],
							elements_per_process,
							MPI_INT, i, 1,
							MPI_COMM_WORLD);
				}//end for

				// last process handles remaining elements
				index = (i-1) * elements_per_process;
				int remaining_elements = arraySize - (i*elements_per_process);
				int Last_process_portion=elements_per_process+remaining_elements;
				MPI_Send(&Last_process_portion,1, MPI_INT,i, 2,MPI_COMM_WORLD);
				MPI_Send(&a[index],Last_process_portion, MPI_INT,i, 1,MPI_COMM_WORLD);

			}//end else
		}//end if

		// collects local max from other processes
		int local_max;
		int local_max_index;
       		int global_max=0;
		int global_max_index;
		for (i = 1; i < np; i++) {
			//recives the local max from slave processes
			MPI_Recv(&local_max, 1, MPI_INT,i, 2,MPI_COMM_WORLD,&status);
			//recives the local max index from slave processes
			MPI_Recv(&local_max_index, 1, MPI_INT,i, 2,MPI_COMM_WORLD,&status);
			int sender= status.MPI_SOURCE;
			printf("Hello from slave#%d Max number in my partition is %d and index is %d \n",sender,local_max,local_max_index);
            global_max=max(global_max,local_max);
			
		}
		//get global max index
		for(int i=0;i<arraySize;i++){
			if(a[i]==global_max){
				global_max_index=i;
			}
		}
		// prints the final max of array
		printf("Master process announce the final max which is %d and index is %d\n",global_max,global_max_index);
	}

	//----------------------slave processes---------------------------------
	else {
		//recives the portion of array from master process
		MPI_Recv(&n_elements_recieved, 1, MPI_INT,0, 2, MPI_COMM_WORLD,&status);
		MPI_Recv(&my_array, n_elements_recieved, MPI_INT,0, 1, MPI_COMM_WORLD,&status);

		// calculates its local maximum
        int mylocal_max=-100000;
		int mylocal_max_index;
			for (int i = 0; i < n_elements_recieved; i++){
				mylocal_max =max(mylocal_max,my_array[i]);
				if(mylocal_max==my_array[i]){
					mylocal_max_index=i;
				}
			}
			// sends the local max to the master process	
			MPI_Send(&mylocal_max, 1, MPI_INT,0, 2, MPI_COMM_WORLD);
			//Send local max index
			MPI_Send(&mylocal_max_index, 1, MPI_INT,0, 2, MPI_COMM_WORLD);

		}
	MPI_Finalize();
	return 0;
}
