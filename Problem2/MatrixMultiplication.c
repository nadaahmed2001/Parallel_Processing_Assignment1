#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void print_2d(int **ary,int row,int col)
{
    for (int i = 0; i <row; ++i) {
        for (int j = 0; j < col; ++j) {
            printf("%d ",ary[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char** argv) {
    MPI_Status status;
    MPI_Init(NULL, NULL);
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int choice;
    //make variables for all processes.
    int r1,c1; int **m1;
    int r2,c2; int **m2;

    int rows_per_process;
    int remaining_rows;

//for slave
    int Myr1_per_process,my_c1;
    int **my_m1;
    int **my_m2;
    int my_r2,my_c2;
    int **my_result;

    int **result_matrix;//for master
    int result_rows,result_cols;//for master
    
    if(rank==0)//Master
    {
        printf("Welcome to vector Matrix multiplication program!\n");
        printf("To read dimensions and values from file press 1\n");
        printf("To read dimensions and values from console press 2\n");
        scanf("%d",&choice);
        if(choice==1){//Read file
            FILE *fp;
            int i, j;
            fp = fopen("matrix.txt", "r");
            if(fp==NULL) exit(1);
            fscanf(fp, "%d %d", &r1, &c1);
            fscanf(fp, "%d %d", &r2, &c2);
            if (c1!=r2){
                printf("Matrices Can't be multiplied together\n");
                exit(1);
            }
            printf("r1 = %d, c1 = %d, r2 = %d, c2 = %d \n", r1, c1, r2, c2);
            
            //Allocate matrix 1 in memory
            m1 = (int**)malloc(r1 * sizeof(int*));
            for (i = 0; i < r1; i++)
                m1[i] = (int*)malloc(c1 * sizeof(int));
            
            //Allocate matrix 2
            m2 = (int**)malloc(r2 * sizeof(int*));
            for (i = 0; i < r2; i++)
                m2[i] = (int*)malloc(c2 * sizeof(int));

            //read matrix 1 from file
            for (i = 0; i < r1; i++)
                for (j = 0; j < c1; j++)
                    fscanf(fp, "%d", &m1[i][j]);
            //read matrix 2 from file
            for (i = 0; i < r2; i++)
                for (j = 0; j < c2; j++)
                    fscanf(fp, "%d", &m2[i][j]);

            printf("Matrix 1: \n");
            for (i = 0; i < r1; i++){
                for (j = 0; j < c1; j++)
                    printf("%d ", m1[i][j]);
                printf("\n");
            }
            printf("Matrix 2: \n");
            for (i = 0; i < r2; i++){
                for (j = 0; j < c2; j++)
                    printf("%d ", m2[i][j]);
                printf("\n");
            }
            fclose(fp);

        }
        else if (choice==2){
            printf("Enter dimensions of Matrix 1: ");
            fflush(stdout);
            scanf("%d %d",&r1,&c1);
            printf("Enter dimensions of Matrix 2: ");
            fflush(stdout);
            scanf("%d %d",&r2,&c2);
           if (c1!=r2){
                printf("Matrices Can't be multiplied together\n");
                exit(1);
            }
            //Allocate matrix 1
            m1 = (int**)malloc(r1 * sizeof(int*));
            for (int i = 0; i < r1; i++)
                m1[i] = (int*)malloc(c1 * sizeof(int));
            //Allocate matrix 2
            m2 = (int**)malloc(r2 * sizeof(int*));
            for (int i = 0; i < r2; i++)
                m2[i] = (int*)malloc(c2 * sizeof(int));


            //input m1 elements
            printf("Enter Elements of Matrix 1: \n");
            fflush(stdout);
            for (size_t i = 0; i < r1; i++)
            {
                for (size_t j = 0; j < c1; j++)
                {
                    scanf("%d",&m1[i][j]);
                }
                
            }
            printf("\n");

            //input m2 elements
            printf("Enter Elements of Matrix 2: \n");
            fflush(stdout);
            for (size_t i = 0; i < r2; i++)
            {
                for (size_t j = 0; j < c2; j++)
                {
                    scanf("%d",&m2[i][j]);
                }
                
            }
            printf("\n");


        }//end if choice==2
       
        //Send matrix 2 to all processes
        for(int i=1;i<size;i++){
            //send rows and columns for each process
            MPI_Send(&r2, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
            MPI_Send(&c2, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
            for(int j=0;j<r2;j++){
                MPI_Send(m2[j], c2, MPI_INT, i, 2, MPI_COMM_WORLD);
            }
        } 

        
        int i,j,start_index,remaining_elements;

        //calculate how many rows from matrix 1 for each process
         rows_per_process = r1/(size-1);

        if(r1%(size-1)==0){//Normal case...no extra rows
            //build  result matrix
            result_matrix = (int**)malloc(rows_per_process * sizeof(int*));
            for (int i = 0; i < r1; i++)
                result_matrix[i] = (int*)malloc(c2 * sizeof(int));

            for ( i = 1; i < size; i++)
            {
                    start_index=(i-1)*rows_per_process;
                    //send rows and columns for each process.
                    MPI_Send(&rows_per_process, 1, MPI_INT, i, 2, MPI_COMM_WORLD);

                    MPI_Send(&c1, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
                    //send portion of rows to each process.
                    for(int j=start_index ;j< start_index+rows_per_process;j++){
                        MPI_Send(m1[j], c1, MPI_INT, i, 2, MPI_COMM_WORLD);
                    }
                    
            }


        //recieve the result from each process
        printf("Result matrix is :(%d x %d) \n",r1,c2);
        for(int i=1;i<size;i++){
            MPI_Recv(&result_rows, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
            int sender=status.MPI_SOURCE;
            for(int j=0;j<result_rows;j++){
                MPI_Recv(result_matrix[j], c2, MPI_INT, i, 1, MPI_COMM_WORLD, &status);
            }
            print_2d(result_matrix,result_rows,c2);
            
        }



        }else{
            //build  result matrix
            result_matrix = (int**)malloc(rows_per_process * sizeof(int*));
            for (int i = 0; i < r1; i++)
                result_matrix[i] = (int*)malloc(c2 * sizeof(int));
            for ( i = 1; i < size-1; i++)
            {
                    start_index=(i-1)*rows_per_process;
                    //send rows and columns for each process.
                    MPI_Send(&rows_per_process, 1, MPI_INT, i, 2, MPI_COMM_WORLD);

                    MPI_Send(&c1, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
                    //send portion of rows to each process.
                    for(int j=start_index ;j< start_index+rows_per_process;j++){
                        MPI_Send(m1[j], c1, MPI_INT, i, 2, MPI_COMM_WORLD);
                    }
                    
            }
            //last process gets the extra rows
             start_index=(i-1)*rows_per_process;
             remaining_rows=r1-start_index;

            //build  result matrix again
            result_matrix = (int**)malloc(remaining_rows * sizeof(int*));
            for (int i = 0; i < r1; i++)
                result_matrix[i] = (int*)malloc(c2 * sizeof(int));


             MPI_Send(&remaining_rows, 1, MPI_INT, i, 2, MPI_COMM_WORLD);             
             MPI_Send(&c1, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
             //send portion of rows to last process
             for(int j=start_index ;j< start_index+remaining_rows;j++){
                MPI_Send(m1[j], c1, MPI_INT, i, 2, MPI_COMM_WORLD);
            }
            //recieve the result from each process
        printf("Result matrix is :(%d x %d) \n",r1,c2);
            for(int i=1;i<size;i++){
            MPI_Recv(&result_rows, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
            int sender=status.MPI_SOURCE;
            for(int j=0;j<result_rows;j++){
                MPI_Recv(result_matrix[j], c2, MPI_INT, i, 1, MPI_COMM_WORLD, &status);
            }
            print_2d(result_matrix,result_rows,c2);
            
            }

        }//end else

        
    }
    //Slave processes
    else
    {
        //recieve m2 from master process
        MPI_Recv(&my_r2, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
        MPI_Recv(&my_c2, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        //Building  m2 for each process with size my_r2*my_c2
        my_m2 = (int**)malloc(my_r2 * sizeof(int*));
        for (int i = 0; i < my_r2; i++)
            my_m2[i] = (int*)malloc(my_c2 * sizeof(int));
        
        //recieve m2 from master process
        for(int i=0;i<my_r2;i++){
            MPI_Recv(my_m2[i], my_c2, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
        }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        //each process recive its portion of rows and columns from matrix 1
        MPI_Recv(&Myr1_per_process, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
        MPI_Recv(&my_c1, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

        //Building  m1 for each process with size my_r1_per_process*my_c1
        my_m1 = (int**)malloc(Myr1_per_process * sizeof(int*));
        for (int i = 0; i < Myr1_per_process; i++)
        my_m1[i] = (int*)malloc(my_c1 * sizeof(int));

        //allocate the multiplication result for each process
        my_result = (int**)malloc(Myr1_per_process * sizeof(int*));
        for (int i = 0; i < Myr1_per_process; i++)
        my_result[i] = (int*)malloc(my_c2 * sizeof(int));


        //recieve portion of rows from master process.
        for(int i=0;i<Myr1_per_process;i++){
            MPI_Recv(my_m1[i], my_c1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
            //Multiply the portion of rows with m2
            for(int j=0;j<my_c2;j++){
                my_result[i][j]=0;
                for(int k=0;k<my_c1;k++){
                    my_result[i][j]+=my_m1[i][k]*my_m2[k][j];
                }
            }

        }

        //send result matrix to master
        //1-send rows
        MPI_Send(&Myr1_per_process, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        //2-send result matrix
        for(int i=0;i<Myr1_per_process;i++){
            MPI_Send(my_result[i], my_c2, MPI_INT, 0, 1, MPI_COMM_WORLD);
        }



    }


    MPI_Finalize();
    return 0;
}
