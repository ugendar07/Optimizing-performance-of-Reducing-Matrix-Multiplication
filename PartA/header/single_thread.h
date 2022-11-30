

// Optimize this function

#include<immintrin.h>

void singleThread(int N, int *matA, int *matB, int *output)
{
  assert( N>=4 and N == ( N &~ (N-1)));
  
  union
  {
     __m256i sum_quard;
     int sums[8];
  };
  for(int rowA=0;rowA<N;rowA+=2)
  {
  
    for(int i=0;i<N;i+=2)
    {
      __m256i A1=_mm256_set1_epi32(matA[rowA*N + i]+matA[(rowA+1)*N + i]);
      __m256i A2=_mm256_set1_epi32(matA[rowA*N+(i+1)]+matA[(rowA+1)*N+(i+1)]);
      int rowC=rowA>>1,index=rowC*(N>>1);
      
      for(int colB=0;colB<N;colB+=8)
      {
        __m256i B1= _mm256_loadu_si256((__m256i*)&matB[i*N+colB]);
        __m256i B2= _mm256_loadu_si256((__m256i*)&matB[(i+1)*N+colB]);
        sum_quard=_mm256_add_epi32(_mm256_mullo_epi32(A1,B1),_mm256_mullo_epi32(A2,B2));
        int colC=colB>>1;
        int c =index + colC;
        output[c]+=sums[0]+sums[1];        
        output[c+1]+=sums[2]+sums[3];        
        output[c+2]+=sums[4]+sums[5];        
        output[c+3]+=sums[6]+sums[7];
      }
    }
  }  
}
