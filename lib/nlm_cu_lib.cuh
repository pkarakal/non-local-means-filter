#ifndef NON_LOCAL_MEANS_FILTER_NLM_CU_LIB_CUH
#define NON_LOCAL_MEANS_FILTER_NLM_CU_LIB_CUH

namespace nlm_cu {

  static inline __device__ void reduce(const double* arr, double* res) {
	extern __shared__ double data[];
	unsigned int tid = threadIdx.x;
	unsigned int i = blockIdx.x * blockDim.x + threadIdx.x;
	data[tid] = arr[i] + arr[i + blockDim.x];
	__syncthreads();
	for (unsigned int s = blockDim.x / 2; s > 0; s >>= 1) {
	  if (tid < s) {
		data[tid] += data[tid + s];
	  }
	  __syncthreads();
	}
	if (tid == 0)
	  res[blockIdx.x] = data[0];
  }

  static inline __device__ double*
  calculateNormalizingConstant(double* image, unsigned long size,
									   double degreeOfFiltering) {
	extern __shared__ double normalizingConstants[];
	unsigned int tid = threadIdx.x;
	unsigned int id = blockIdx.x * blockDim.x + tid;
	normalizingConstants[id] = 0;
	double* loop_data;
	loop_data = (double*)cudaMalloc((void**)&loop_data, size * sizeof(double));
	for (int i = 0; i < size; ++i) {
	  loop_data[i] = exp(-std::pow(image[id] - image[i], 2) /
						 std::pow(degreeOfFiltering, 2));
	}
	double* res;
	res = (double*)cudaMalloc((void**)&res, sizeof(double));
	reduce(loop_data, res);
	normalizingConstants[id] = res[0];
	cudaFree(loop_data);
	cudaFree(res);
	return normalizingConstants;
  }

  static __global__ void non_local_means(const double* image, unsigned long imageSize,
								  double degreeOfFiltering, const double* patch,
								  unsigned long patchSize,
								  const double* patchWeights,
								  double* denoised_image) {
	extern __shared__ double shared_weights[];
	extern __shared__ double shared_patches[];

	double tmp, D = 0, normZ = 0;
	int row = 0, col = 0;
	unsigned int tid = threadIdx.x;
	unsigned int id = blockDim.x * blockIdx.x + tid;

	if (tid < std::pow(patchSize, 2))
	  shared_patches[tid] = patchWeights[tid];

	for (int i = 0; i < imageSize; i++) {
	  for (int j = i * patchSize, j2 = 0; j < (i+1) *patchSize; j++, j2++) {
		if (col == patchSize) {
		  row++;
		  col = 0;
		}
		if (row == patchSize)
		  row = 0;
		if (patch[id * patchSize + j2] != 0 && patch[j] != 0) {
		  tmp = pow((patch[id * patchSize + j2] - patch[j]), 2);
		  tmp *= shared_patches[row * patchSize + col];
		  D += tmp;
		}
		col++;
	  }

	  D = exp((-D) / pow(degreeOfFiltering, 2));
	  shared_weights[tid] = D;
	  denoised_image[id] += shared_weights[tid] * image[i];
	  normZ += D;
	  D = 0;
	}
	denoised_image[id] /= normZ;
  }

  static __global__ void findPatches(const double* image, double* patch, int imageSize,
							  int patchSize) {
	int row, row2, col, col2, count = 0, range = (patchSize - 1) / 2;
	for (int i = 0; i < imageSize; ++i) {
	  for (int j = 0; j < imageSize; ++j) {
		for (row = i - range, row2 = 0; row2 < patchSize; ++row, ++row2) {
		  for (col = j - range, col2 = 0; col2 < patchSize; ++col, ++col2) {
			if ((row >= imageSize) || (col >= imageSize) || row < 0 || col < 0)
			  patch[count++] = 0;
			else if (row >= 0 || col >= 0)
			  patch[count++] = image[row * imageSize + col];
		  }
		}
	  }
	}
  }

} // namespace nlm_cu
#endif // NON_LOCAL_MEANS_FILTER_NLM_CU_LIB_CUH
