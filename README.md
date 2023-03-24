Assignment 1 in High Performance Computing (Parallel Processing)
- Using MPI_SEND & MPI_RECV ONLY.
- Running the C code on Ubuntu terminal on virtual box
- Must install MPI first
 ## Problem 1:
 Develop a parallel program that outputs max number in a given array.
    
#### Master:
- Reads size of array.
- Reads elements inside array.
- Distributes the work among slaves processes:\
    • Sends size of the array.\
    • Sends the assigned partition of the array
- After each slave finishes its work, master process receives max number and its index from each process.
- Then master computes the max number from max numbers returned from each slave.
- Output the final max number and its index in the original array to user.

#### Slaves each one will:
- Receives size of the array.
- Receives the portion of the array.
- Calculates max number.
- Sends max number and its index back to master process.

-> Note:\
Size of array may not be divisible by number of processes. So, this case is handeled

### ScreenShots of my source code output:

![Problem1Capture1](https://user-images.githubusercontent.com/60941223/227506808-0b0e82ec-60da-4a53-bdb8-70378537f570.JPG)
![Problem1Capture2](https://user-images.githubusercontent.com/60941223/227506821-df42e0b9-43d2-4e42-8b21-0d0b06ac1912.JPG)

## Problem 2 (Matrix Multiplication)
Write a matrix multiplication program, Matrices’ dimensions and values are taken as
an input.\
Take input from console and from file.\
Must use dynamic allocation.

### ScreenShots of my source code output:
Reading from console
![Problem2Capture2](https://user-images.githubusercontent.com/60941223/227507350-2dfec154-fde8-4ba0-ae1a-9d1bdbc00e27.JPG)
Reading from file
![Problem2WithFiles](https://user-images.githubusercontent.com/60941223/227507606-ed16027b-3c42-462e-a58e-580eb8bfdf7e.JPG)

