#include "lib/nlm_lib.h"
#include <chrono>
#include <iostream>
#include "lib/nlm_cu_lib.cuh"

void nlm(std::string& path, std::string& format, std::string& variable,
		 int patchSize, bool noise);

int main(int argc, char** argv) {
  if (argc > 1) {
	bool noise = false;
	std::vector<std::string> paths{};
	std::vector<std::string> formats{};
	std::vector<std::string> variables{};
	std::vector<int> patchSizes{};
	parse_cli_args(argc, argv, paths, formats, variables, patchSizes, noise);
	nlm(paths.at(0), formats.at(0), variables.at(0), patchSizes.at(0), false);
	std::vector<std::vector<double>> image = {
		{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}};
	write_csv("file", image);
	return 0;
  } else {
	std::cout << "Wrong usage" << std::endl;
	std::cout << "Try running " << argv[0] << " --help" << std::endl;
  }
  return 0;
}


void nlm(std::string& path, std::string& format, std::string& variable,
		 int patchSize, bool add_noise) {
  std::vector<double> image{};
#ifdef USE_OCTAVE
  if (format == "mat")
	octave_read(path.c_str(), image, variable);
  else {
	std::cerr << "Make sure the format matches your build parameters";
	return;
  }
#elif defined(USE_MATLAB)
  if (format == "mat")
	matread(path.c_str(), image, variable);
  else {
	std::cerr << "Make sure the format matches your build parameters";
	return;
  }
#elif defined(USE_OPENCV)
  if (format == "png")
	opencv_read(path.c_str(), image);
  else {
	std::cerr << "Make sure the format matches your build parameters";
	return;
  }
#else
  int rowCount = 0, columnCount = 0;
  if (format == "csv")
	read_csv(path.c_str(), image, rowCount, columnCount);
  else {
	std::cerr << "Make sure the format matches your build parameters";
	return;
  }
#endif
  // create gaussian noise
  std::vector<double> noise = std::vector<double>(image.size());
  add_gaussian_noise(image, noise.size(), 0.5, noise);
  // create gaussian kernel
  std::vector<std::vector<double>> kernel =
	  std::vector<std::vector<double>>(patchSize);
  for (std::vector<double>& vec : kernel) {
	vec = std::vector<double>(patchSize);
  }
  gaussian_kernel(kernel, patchSize, 0.8);
  // convert 2d vector to 1d
  std::vector<double> kernel1d = std::vector<double>(patchSize * patchSize);
  int i = 0;
  for (std::vector<double>& item : kernel) {
	std::copy(item.begin(), item.end(), kernel1d.begin() + (i * patchSize));
	++i;
  }
  // create necessary vectors
  std::vector<double> denoised_image = std::vector<double>(image.size());
  std::vector<double> host_patch =
	  std::vector<double>(patchSize * patchSize * image.size());
  // declare device variables
  auto* final_image = (double*)malloc(image.size() * sizeof(double));
  auto* h_patch = (double*)malloc(image.size()*kernel1d.size() * sizeof(double));
  double *device_noise, *device_kernel, *patch, *device_denoised;
  // reserve space on device
  cudaMalloc((void**)&device_noise, image.size() * sizeof(double));
  cudaMalloc((void**)&device_denoised, image.size() * sizeof(double));
  cudaMalloc((void**)&device_kernel, kernel1d.size() * sizeof(double));
  cudaMalloc((void**)&patch,
			 kernel1d.size() * image.size() * sizeof(double));
  // copy variable data from host to device
  cudaMemcpy(device_noise, noise.data(), noise.size()*sizeof(double), cudaMemcpyHostToDevice);
  cudaMemcpy(device_kernel, kernel1d.data(),
			 kernel1d.size() * sizeof(double), cudaMemcpyHostToDevice);
  // configure cuda parameters and run on device
  dim3 blockSize(1024);
  dim3 gridSize(1024/ blockSize.x);
  auto start = std::chrono::high_resolution_clock::now();
  int size = (int)std::sqrt(image.size()) +1;
  int ptSize = (int) std::sqrt(kernel1d.size()) +1;
  nlm_cu::findPatches<<<gridSize, blockSize>>>(device_noise, patch, size, patchSize);
  nlm_cu::non_local_means<<<gridSize, blockSize, size>>>(device_noise, image.size(), 0.02, patch, kernel1d.size(),
							  device_kernel, device_denoised);
  cudaDeviceSynchronize();
  auto stop = std::chrono::high_resolution_clock::now();
  // copy data from device back to host
  cudaMemcpy(final_image, device_denoised, denoised_image.size() * sizeof(double),
			 cudaMemcpyDeviceToHost);
  cudaMemcpy(h_patch, patch,  kernel1d.size()* image.size()* sizeof(double),
			 cudaMemcpyDeviceToHost);
  host_patch = std::vector<double>(h_patch, h_patch+kernel1d.size()*image.size());
  denoised_image = std::vector<double>(final_image, final_image + image.size());
  // convert 1d image vector back to 2d
  std::vector<std::vector<double>> final =
	  std::vector<std::vector<double>>(size);
  i = 0;
  for (std::vector<double>& item : final) {
	item = std::vector<double>(size);
	std::copy(denoised_image.begin() + (i * size),
			  denoised_image.begin() + ((i + 1) * size), item.begin());
	i++;
  }
  // write result to file
  write_csv(path.c_str(), final);
  std::cout << "Took " << (stop - start).count() * 10e-9 << "s" << std::endl;
  // free memory
  cudaFree(patch);
  cudaFree(device_kernel);
  cudaFree(device_denoised);
  cudaFree(device_noise);
  free(final_image);
  free(h_patch);
}
