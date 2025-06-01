#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

const char* kernelSource = 
"__kernel void vec_add(__global float* A, __global float* B, __global float* C) {"
"    int id = get_global_id(0);"
"    C[id] = A[id] + B[id];"
"}";

int main() {
    const int N = 1024;
    float A[N], B[N], C[N];
    for (int i = 0; i < N; i++) {
        A[i] = i;
        B[i] = N - i;
    }

    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;

    cl_mem bufferA, bufferB, bufferC;

    clGetPlatformIDs(1, &platform, NULL);
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    context = clCreateContext(0, 1, &device, NULL, NULL, NULL);
    queue = clCreateCommandQueue(context, device, 0, NULL);

    bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * N, NULL, NULL);
    bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * N, NULL, NULL);
    bufferC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * N, NULL, NULL);

    clEnqueueWriteBuffer(queue, bufferA, CL_TRUE, 0, sizeof(float) * N, A, 0, NULL, NULL);
    clEnqueueWriteBuffer(queue, bufferB, CL_TRUE, 0, sizeof(float) * N, B, 0, NULL, NULL);

    program = clCreateProgramWithSource(context, 1, &kernelSource, NULL, NULL);
    clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    kernel = clCreateKernel(program, "vec_add", NULL);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferA);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferB);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferC);

    size_t globalSize = N;
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, NULL, 0, NULL, NULL);
    clFinish(queue);

    clEnqueueReadBuffer(queue, bufferC, CL_TRUE, 0, sizeof(float) * N, C, 0, NULL, NULL);

    for (int i = 0; i < 10; i++)
        printf("%f ", C[i]);

    printf("\n");

    clReleaseMemObject(bufferA);
    clReleaseMemObject(bufferB);
    clReleaseMemObject(bufferC);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    return 0;
}
