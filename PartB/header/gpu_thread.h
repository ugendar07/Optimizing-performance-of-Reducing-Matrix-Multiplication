

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <vector>

using std::cout;
using std::generate;
using std::vector;

__global__ void RMM(const int *a, const int *b, int *c, int N) {
  // Compute each thread's global row and column index
  int n = N>>1;
  int row = blockIdx.y * blockDim.y + threadIdx.y;
  int col = blockIdx.x * blockDim.x + threadIdx.x;

  // Iterate over row, and down column
  int value = 0;

  for(int k = 0; k < N; k++) {
    // Accumulate results for a single element
    value += a[(row<<1) * N + k] * b[k * N + (col<<1)];
    value += a[(row<<1) * N + k] * b[k * N + (col<<1)+1];
    value += a[((row<<1)+1) * N + k] * b[k * N + (col<<1)];
    value += a[((row<<1)+1) * N + k] * b[k * N + (col<<1)+1];
  }
  c[row * n + col] = value;
}
void gpuThread(int N, int *matA, int *matB, int *output)
{
    int n = N>>1;
    size_t mat_bytes = N * N * sizeof(int);
    size_t res_bytes = n * n * sizeof(int);
    int *d_a, *d_b, *d_c;
  cudaMalloc(&d_a, mat_bytes);
  cudaMalloc(&d_b, mat_bytes);
  cudaMalloc(&d_c, res_bytes);

  cudaMemcpy(d_a, matA.data(), mat_bytes, cudaMemcpyHostToDevice);
  cudaMemcpy(d_b, matB.data(), mat_bytes, cudaMemcpyHostToDevice);


  // Threads per CTA dimension
  int THREADS = 32;

  // Blocks per grid dimension (assumes THREADS divides N evenly)
  int BLOCKS = n / THREADS;

  // Use dim3 structs for block  and grid dimensions
  dim3 threads(THREADS, THREADS);
  dim3 blocks(BLOCKS, BLOCKS);

  // Launch kernel
  RMM<<<blocks, threads>>>(d_a, d_b, d_c, N);

  // Copy back to the host
  cudaMemcpy(output.data(), d_c, res_bytes, cudaMemcpyDeviceToHost);



  cout << "DONE\n";

  // Free memory on device
  cudaFree(d_a);
  cudaFree(d_b);
  cudaFree(d_c);


}

