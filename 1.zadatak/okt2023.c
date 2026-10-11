include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>

#define k 2
#define l 3
#define m 4

int main(int argc, char *argv[])
{
    int rank, size;

    int a[k][l];
    int b[l][m];

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Status st;

    // Mora broj procesa = broj kolona A = broj vrsta B
    if(size != l)
    {
        if(rank == 0)
        {
            printf("Broj procesa mora biti %d\n", l);
        }

        MPI_Finalize();
        return 0;
    }

    srand(time(NULL) + rank);

    // Root inicijalizuje matrice
    if(rank == 0)
    {
        printf("Matrica A:\n");

        for(int i = 0; i < k; i++)
        {
            for(int j = 0; j < l; j++)
            {
                a[i][j] = rand() % 5 + 1;
               
            }
        
        }

        printf("\nMatrica B:\n");

        for(int i = 0; i < l; i++)
        {
            for(int j = 0; j < m; j++)
            {
                b[i][j] = rand() % 5 + 1;
                
            }
            
        }

        printf("\n");
    }

    // Lokalna kolona iz A
    int koloneA[k];

    // Pomocni niz za slanje
    int kol[k];

    // Root salje kolone matrice A
    if(rank == 0)
    {
        for(int j = 1; j < l; j++)
        {
            for(int i = 0; i < k; i++)
            {
                kol[i] = a[i][j];
            }

            MPI_Send(kol, k, MPI_INT, j, 0, MPI_COMM_WORLD);
        }

        // Root uzima prvu kolonu
        for(int i = 0; i < k; i++)
        {
            koloneA[i] = a[i][0];
        }
    }
    else
    {
        MPI_Recv(koloneA, k, MPI_INT, 0, 0, MPI_COMM_WORLD, &st);
    }

    // Lokalna vrsta iz B
    int vrsteB[m];

    MPI_Scatter(b, m, MPI_INT,vrsteB, m, MPI_INT,0, MPI_COMM_WORLD);

    // Lokalni deo matrice C
    int c[k][m];

    // Inicijalizacija na 0
    for(int i = 0; i < k; i++)
    {
        for(int j = 0; j < m; j++)
        {
            c[i][j] = 0;
        }
    }

    // Racunanje spoljnog proizvoda
    for(int i = 0; i < k; i++)
    {
        for(int j = 0; j < m; j++)
        {
            c[i][j] += koloneA[i] * vrsteB[j];
        }
    }

    // Konacna matrica C
    int cKon[k][m];

    MPI_Reduce(c, cKon,k * m,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);

    // Proizvod elemenata po vrstama
    int prodVrstaA[k];

    MPI_Reduce(koloneA,prodVrstaA,k,MPI_INT,MPI_PROD,0,MPI_COMM_WORLD);

    // Ispis
    if(rank == 0)
    {
        printf("Matrica C = A * B:\n");

        for(int i = 0; i < k; i++)
        {
            for(int j = 0; j < m; j++)
            {
                printf("%d ", cKon[i][j]);
            }

            printf("\n");
        }

        printf("\nProizvod elemenata po vrstama A:\n");

        for(int i = 0; i < k; i++)
        {
            printf("%d ", prodVrstaA[i]);
        }

        printf("\n");
    }

    MPI_Finalize();

    return 0;
}