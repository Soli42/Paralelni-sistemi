#include <stdio.h>
#include <mpi.h>
#define r 4
#define m 3
#define k 2
#define MCW MPI_COMM_WORLD
int main(int argc, char* argv[])
{
    int A[r][m], B[m][k], C[r][k], p, rank, l, i, j, * local_a, s, local_c;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &p);
    MPI_Datatype novi_t, novi_t_r, novi_c, novi_c_r;
    l = r / p;
    MPI_Type_vector(l, m, p m, MPI_INT, &novi_t);
    MPI_Type_commit(&novi_t);
    MPI_Type_vector(l, k, p * k, MPI_INT, &novi_c);
    MPI_Type_commit(&novi_c);
    MPI_Type_create_resized(novi_t, 0, m * sizeof(int), &novi_t_r);
    MPI_Type_commit(&novi_t_r);
    MPI_Type_create_resized(novi_c, 0, k * sizeof(int), &novi_c_r);
    MPI_Type_commit(&novi_c_r);
    local_a = (int)malloc(l * m * sizeof(int));
    local_c = (int)malloc(l * k * sizeof(int));
    if (rank == 0)
    {
        for (i = 0; i < r; i++)
        {
            for (j = 0; j < m; j++)
            {
                A[i][j] = rand() % 5 + 1;
            }
        }
        for (i = 0; i < m; i++)
        {
            for (j = 0; j < k; j++)
            {
                B[i][j] = rand() % 5 + 1;
            }
        }
    }
    MPI_Scatter(A, 1, novi_t_r, &local_a, l * m, MPI_INT, 0, MCW);
    MPI_Bcast(B, m * k, MPI_INT, 0, MCW);

    for (i = 0; i < l; i++)
    {
        for (j = 0; j < k; j++)
        {
            for (s = 0; s < m; s++) 
            {
                local_c[i * k + j] += local_a[i * m + s] * B[s][j];
            }
        }
    }

    MPI_Gather(local_c, l * k, MPI_INT, C, 1, novi_c_r, 0, MCW);

    MPI_Finalize();
    return 0;
}