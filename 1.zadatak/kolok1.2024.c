#include<mpi.h>
#include<stdio.h>
#define k 4
#define m 6
#define n 6 
#define MCW MPI_COMM_WORLD

void main (int argc, chat* argv[])
{
	int A[n][k],B[k][m],C[n][m],*local_b,*local_c,p,rank,i,j,s;
	MPI_Status st;
	MPI_Datatype newB,newB_r,newC,newC_r;

   MPI_Init(&argc,&argv);
   MPI_Comm_size(MCW,&p);
   MPI_Comm_rank(MCW,&rank);

   s= m/p;
   

   local_b = (int*)malloc(k*s*sizeof(int));
   local_c = (int*)malloc(n*s*sizeof(int));
   
   MPI_Type_vector(k,s,m,MPI_INT,&newB);
   MPI_Type_commit(&newB);
   
   MPI_Type_create_resized(newB,0,s*sizeof(int),&newB_r);
   MPI_Type_commit(&newB_r);
   
   MPI_Type_vector(n,s,m,MPI_INT,&newC);
   MPI_Type_commit(&newC);
   
   MPI_Type_create_resized(newC,0,s*sizeof(int),&newC_r);
   MPI_Type_commit(&newC_r);
   
   if(rank==0)
   {
      for(i=0;i<n;i++)
      {
        for(j=0;j<k;j++)
        {
           A[i][j]=rand()%100;
        }

     for(i=0;i<k;i++)
      {
        for(j=0;j<m;j++)
        {
           B[i][j]=rand()%100;
        }
		
	}
	
	//b) //p2p operacije salju iz p0->p1, p0->p2 odjednom izvedeni tip
	
		for(int i=0; i<k;i++)
		{
			for(int j=0;j<s;j++) //ovde radimo za sve podatke p0 iz matrice  B i zato su ove dimenzije
			{
				local_b[i*s+j]=B[i][j]; //ovde smo sve procese p0 iz matrice B upisali u local_b
			}
		}
		
		for(int i=1;i<p;i++) //u procesu p0 smo stavili podatke iz matrice B i sada for petlja ide od 1 do p jer treba da prosledim iz po u p1 i p2 zato je ovako
		
		{
			//ovde smo stavili ovu adresu za matricu B jer za p1 mi je pocetak u matrici na adresi B[0][2] a za p2 je B[0][4] i menja mi se j vrednost koja je zapravo i*s
			MPI_Send(&B[0][i*s],1,newB,i,7,MCW);
		
		}
   }
   else //if(rank==0) else za to
   {
	MPI_Recv(local_b,k*s,MPI_INT,0,7,MCW,&st);
   }
   
   MPI_Bcast(A,n*k,MPI_INT,0,MCW);
   MPI_Scatter(B,1,newB_r,local_b,n*s,MPI_INT,0,MCW);

   
   for(i=0;i<n;i++)
      {
        for(j=0;j<s;j++)
        {
           local_c[i*s+j]=0;///c[i][j]
		   
		   for(z=0;z<k;z++) //ovo je za unutrasnje dimenzije A i B matrice
		   {
				local_c[i*s+j]+=A[i][z]*local_b[z*s+j]; //sablon nauci napamet
		   }
        }
}

struct
{
	int vrednost;
	int rank;
} global_max,local_max;

local_max.vrednost=INT_MIN;
local_max.rank=rank;

for(i = 0 ; i<k; i++)
{
	for(j=0;j<s;j++)
	{
		if(local_b[i*s+j]>local_max.vrednost)
		local_max.vrednost=local_b[i*s+j];
	}
}

MPI_Reduce(&local_max,&global_max,1,MPI_2INT,MPI_MAXLOC,0,MCW);

MPI_Bcast(&global_max,1,MPI_2INT,0,MCW);


MPI_Gather(local_c,1,n,C,1,newC_r,global_max.rank,MCW);

if(global_max.rank==rank)
{
	for(int i = 0 ; i<n; i++)
	{
		for(int j=0;j<m;j++)
		
          printf("%d",c[i][j]);
   }
}
	  
	free(local_b);
   free(local_c);

  MPI_Finalize();
	}
}