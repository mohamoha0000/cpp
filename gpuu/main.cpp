// file: main.cpp
#define CL_TARGET_OPENCL_VERSION 120
#include <CL/cl.h>
#include <iostream>
#include <vector>

const char* kernelSource = R"CLC(
__kernel void vector_add(__global const float* A, __global const float* B, __global float* C) {
    int id = get_global_id(0);
    C[id] = A[id] + B[id];
}
)CLC";

int main() {
    const int size = 1024;
    std::vector<float> A(size, 1.0f);
    std::vector<float> B(size, 2.0f);
    std::vector<float> C(size, 0.0f);

    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;

    cl_mem d_A, d_B, d_C;

    cl_int err;

    // 1. الحصول على المنصة
    clGetPlatformIDs(1, &platform, nullptr);
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr);

    context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err);
    queue = clCreateCommandQueue(context, device, 0, &err);

    d_A = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, size * sizeof(float), A.data(), &err);
    d_B = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, size * sizeof(float), B.data(), &err);
    d_C = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size * sizeof(float), nullptr, &err);

    program = clCreateProgramWithSource(context, 1, &kernelSource, nullptr, &err);
    clBuildProgram(program, 0, nullptr, nullptr, nullptr, nullptr);

    kernel = clCreateKernel(program, "vector_add", &err);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_A);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_B);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_C);

    size_t global_size = size;
    clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &global_size, nullptr, 0, nullptr, nullptr);

    clEnqueueReadBuffer(queue, d_C, CL_TRUE, 0, size * sizeof(float), C.data(), 0, nullptr, nullptr);

    std::cout << "C[0] = " << C[0] << ", C[10] = " << C[10] << ", C[100] = " << C[100] << std::endl;

    // تنظيف
    clReleaseMemObject(d_A);
    clReleaseMemObject(d_B);
    clReleaseMemObject(d_C);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    //pausse
    int a;
    std::cin>>a;
    return 0;
}
//g++ main.cpp -o opencl_test -I. -L. -lOpenCL