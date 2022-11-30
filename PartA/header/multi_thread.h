


#include <pthread.h>

// Create other necessary functions here

#define nl 8

struct inputs
{
	int i;
	int N;
	int *matA; 
	int *matB; 
	int *output;
	inputs(int i1,int N1,int *matA1,int *matB1,int *output1)
	{
		i=i1;
		N=N1;
		matA=matA1;
		matB=matB1;
		output=output1;
	}
}*ip[nl];

/*
// Change the order. Computation performed by single Thread piece //
void *Tcomp(void *arg)
{
    int index = *((int*)arg);
    for(int rowA = index*N2; rowA < index*N2 + N2; rowA +=2) {
        int rowC = rowA>>1;
        for(int iter = 0; iter < N; iter++){
            for(int colB = 0; colB < N; colB+=2) 
            {
                int sum = 0;
                sum += matA[rowA * N + iter] * matB[iter * N + colB];
                sum += matA[(rowA+1) * N + iter] * matB[iter * N + colB];
                sum += matA[rowA * N + iter] * matB[iter * N + (colB+1)];
                sum += matA[(rowA+1) * N + iter] * matB[iter * N + (colB+1)];
        
                // compute output indices
                int colC = colB>>1;
                int indexC = rowC * (N>>1) + colC;
                output[indexC] += sum;
            }
    }
  }
  pthread_exit(NULL);
  return NULL;
}

*/




void* singleThread1(void *ip)
{
  	struct inputs *a1=(struct inputs *)ip;
  	int i=a1->i;
  	int N=a1->N;
  	int *matA,*matB,*output;
  	matA=a1->matA;
  	matB=a1->matB;
  	output=a1->output;
  
  	assert( N>=4 and N == ( N &~ (N-1)));
  	union
  	{
     		__m256i sum_quard;
     		int sums[8];
  	};
  	int j = N/nl;  //load on each thread
  	for(int rowA=i*j;rowA<(i+1)*j;rowA+=2)
  	{
    		for(int i=0;i<N;i+=2)
    		{
      			__m256i A1=_mm256_set1_epi32(matA[rowA*N + i]+matA[(rowA+1)*N + i]);
      			__m256i A2=_mm256_set1_epi32(matA[rowA*N+(i+1)]+matA[(rowA+1)*N+(i+1)]);
      			int rowC=rowA>>1;
      			int index=rowC*(N>>1);
      			
      			for(int colB=0;colB<N;colB+=8)
      			{
        			__m256i B1= _mm256_loadu_si256((__m256i*)&matB[i*N+colB]);
        			__m256i B2=_mm256_loadu_si256((__m256i*)&matB[(i+1)*N+colB]);
        		        sum_quard=_mm256_add_epi32(_mm256_mullo_epi32(A1,B1),_mm256_mullo_epi32(A2,B2));
        			int colC=colB>>1;
        			int C =index + colC;
        			output[C]+=sums[0]+sums[1];        
        			output[C+1]+=sums[2]+sums[3];        
        			output[C+2]+=sums[4]+sums[5];        
        			output[C+3]+=sums[6]+sums[7];
      			}
    		}
  	}
  	
  	pthread_exit(NULL);
  	return NULL;	  
}

// Fill in this function
void multiThread(int N, int *matA, int *matB, int *output)
{
	
	pthread_t thread[nl];
	
	for(int i=0;i<nl;i++)
	{
		ip[i] = new inputs(i,N,matA,matB,output);  //input for thread i
		int iter=pthread_create(&thread[i],NULL,&singleThread1,(void *)ip[i]);
	}   
	for(int i=0;i<nl;i++)
	{
		pthread_join(thread[i],NULL);
	}
}
