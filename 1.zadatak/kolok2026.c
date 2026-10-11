include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>
#define n 2
#define k 4
#define MCW MPI_COMM_WORLD

int main(int argc, char *argv[])
{
    int rank, size;

    int a[n][k];
    int b[k];

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);

    MPI_Status st;
    MPI_Datatype noviA,noviA_r,noviB;

    int s = k / size;
    int c[n], parC[n];
    int koloneA = (int)malloc(sizeof(int) * s*n);
    int elementiB = (int)malloc(sizeof(int) * s);
    MPI_Type_vector(s*n, 1, s, MPI_Datatype MPI_INT, &noviA);
    MPI_Type_commit(&noviA);
    MPI_Type_vector(s, 1, s, MPI_Datatype MPI_INT, &noviB);
    MPI_Type_commit(&noviB);
    MPI_Type_create_resized(noviA, 0, size*sizeof(int), &noviA_r);
    MPI_Type_commit(&noviA_r);  
    if(rank == 0)
    {
        
        for(int i = 0; i < n; i++)
        {
            for(int j = 0; j < k; j++)
            {
                a[i][j] = rand() % 5 + 1;
               
            }
        
        }
        for(int i = 0; i < k; i++)
        {
            b[i] = rand() % 5 + 1;
        }
        for(int i = 0; i < s; i++)
        {
            elementiB[i] = b[i*size];
        }
        for(int j=1; j<size;j++)
        {
            MPI_Send(&B[j], 1, noviB, j, 0, MCW);
        }
        
    }
    else
    {
        MPI_Recv(elementiB, s, MPI_INT, 0, 0, MCW, &st);
    }

MPI_Scatter(a, 1, noviA_r, koloneA, s*n, MPI_INT, 0, MCW);

for(int i = 0; i <n; i++)
{
    parC[i] = 0;
    for(int j = 0; j < s; j++)
    {
    parC[i]+=koloneA[i*s+j]*elementiB[j];
    }
}
MPI_Reduce(parC, c, n, MPI_INT, MPI_SUM, 0, MCW);
if(rank == 0)
{
    for(int i = 0; i < n; i++)
    {
        printf("%d ", c[i]);
    }
}
free(koloneA);
free(elementiB);
MPI_Finalize();
}