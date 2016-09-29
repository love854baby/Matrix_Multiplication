//
//  main.cpp
//  MatrixMultiplication
//
//  Created by Ye Guo on 3/15/14.
//  Copyright (c) 2014 Ye Guo. All rights reserved.
//

#include <iostream>
#include <stdlib.h>
#include <sys/time.h>
#include "mpi.h"
#include <math.h>
using namespace std;

#define MATRIX_SIZE 512

float A[MATRIX_SIZE][MATRIX_SIZE], B[MATRIX_SIZE][MATRIX_SIZE], C[MATRIX_SIZE][MATRIX_SIZE], R[MATRIX_SIZE][MATRIX_SIZE];

int main(int argc, char * argv[])
{
	int numproces, myid, source, dest, rows, columns, s, offset;
    int i, j, k, x, y;
    
    MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &numproces);
    
    s = sqrt (numproces - 1);
	offset = MATRIX_SIZE/s;
    float tempArr[offset][offset];
	timeval start, stop, total;
	
	if(myid == 0)
	{
		for(rows=0;rows<MATRIX_SIZE;rows++)
    	{
        	for(columns=0;columns<MATRIX_SIZE;columns++)
        	{
            	A[rows][columns] = rand()/static_cast<float>(RAND_MAX);
            	B[rows][columns] = rand()/static_cast<float>(RAND_MAX);
        	}
    	}
        
        for(rows=0;rows<MATRIX_SIZE;rows++)
    	{
        	for(columns=0;columns<MATRIX_SIZE;columns++)
        	{
            	R[rows][columns] = 0;
                for(int i=0;i<MATRIX_SIZE;i++)
                    R[rows][columns] += A[rows][i] * B[i][columns];
        	}
    	}
    	
    	gettimeofday(&start, NULL);

    	for(rows=0;rows<s;rows++)
    	{
    		for(columns=0;columns<s;columns++)
    		{
    			dest = rows * s + columns + 1;
                i = rows * offset;
                j = columns * offset;
    			MPI_Send(&i, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
    			MPI_Send(&j, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
    			MPI_Send(&A, MATRIX_SIZE*MATRIX_SIZE, MPI_FLOAT, dest, 1, MPI_COMM_WORLD);
    			MPI_Send(&B, MATRIX_SIZE*MATRIX_SIZE, MPI_FLOAT, dest, 1, MPI_COMM_WORLD);
    		}
    	}
        
    	for(rows=0;rows<s;rows++)
    	{
    		for(columns=0;columns<s;columns++)
    		{
    			source = rows*s + columns + 1;
    			MPI_Recv(&tempArr, offset*offset, MPI_FLOAT, source, 2, MPI_COMM_WORLD, NULL);
    			
    			for(i=0;i<offset;i++)
    				for(j=0;j<offset;j++)
    					C[rows*offset + i][columns*offset + j] = tempArr[i][j];
    		}
    	}
        
    	gettimeofday(&stop, NULL);
    	timersub(&stop, &start, &total);
    	
    	cout << "The total time cost is " << total.tv_sec + total.tv_usec/1000000.0 << " seconds" << endl;
        cout << endl;
        
        for(i=0;i<10;i++)
        {
            x = static_cast<int>(rand()/static_cast<float>(RAND_MAX)*(MATRIX_SIZE-1));
            y = static_cast<int>(rand()/static_cast<float>(RAND_MAX)*(MATRIX_SIZE-1));
            
            cout << "Choose value from point (" << x << ", " << y << ")" << endl;
            cout << "Computed value is " << C[x][y] << " and correct value is " << R[x][y] << "." << endl;
            if(C[x][y] == R[x][y])
                cout << "The answer is consistent!" << endl;
            else
                cout << "The answer is not consistent!" << endl;
            cout << endl;
        }
	}
    
    if(myid > 0)
    {
    	source = 0;
    	
		MPI_Recv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, NULL);
		MPI_Recv(&columns, 1, MPI_INT, source, 1, MPI_COMM_WORLD, NULL);
		MPI_Recv(&A, MATRIX_SIZE*MATRIX_SIZE, MPI_FLOAT, source, 1, MPI_COMM_WORLD, NULL);
    	MPI_Recv(&B, MATRIX_SIZE*MATRIX_SIZE, MPI_FLOAT, source, 1, MPI_COMM_WORLD, NULL);
    	
    	for(i=0;i<offset;i++)
    	{
    		for(j=0;j<offset;j++)
    		{
    			tempArr[i][j] = 0;
    			for(k=0;k<MATRIX_SIZE;k++)
    				tempArr[i][j] += A[rows + i][k] * B[k][columns + j];
    		}
    	}
    	
    	MPI_Send(&tempArr, offset*offset, MPI_FLOAT, source, 2, MPI_COMM_WORLD);
    }
    
    MPI_Finalize();
    
    return 0;
}