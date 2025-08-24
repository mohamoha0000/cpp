#define CL_TARGET_OPENCL_VERSION 120
#include <CL/cl.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>

const int N = 4000; // حجم المصفوفة (8192 × 8192)

const char* kernelSource = R"(
__kernel void mat_mul(__global float* A, __global float* B, __global float* C, int N) {
    int row = get_global_id(0);
    int col = get_global_id(1);

    float sum = 0.0f;
    for (int k = 0; k < N; ++k) {
        sum += A[row * N + k] * B[k * N + col];
    }
    C[row * N + col] = sum;
}
)";

void cpu_matrix_mult(const std::vector<float>& A, const std::vector<float>& B, std::vector<float>& C, int N) {
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j) {
            float sum = 0.0f;
            for (int k = 0; k < N; ++k)
                sum += A[i * N + k] * B[k * N + j];
            C[i * N + j] = sum;
        }
}

int main() {
    std::vector<float> A(N*N), B(N*N), C(N*N), C_cpu(N*N);

    // تعبئة المصفوفات بقيم بسيطة
    for (int i = 0; i < N*N; ++i) {
        A[i] = 1.0f;
        B[i] = 1.0f;
    }

    // ==== CPU ====

    // ==== GPU ====
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;

    clGetPlatformIDs(1, &platform, NULL);
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
    queue = clCreateCommandQueue(context, device, 0, NULL);

    cl_mem bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * N*N, A.data(), NULL);
    cl_mem bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * N*N, B.data(), NULL);
    cl_mem bufferC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * N*N, NULL, NULL);

    program = clCreateProgramWithSource(context, 1, &kernelSource, NULL, NULL);
    clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    kernel = clCreateKernel(program, "mat_mul", NULL);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferA);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferB);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferC);
    clSetKernelArg(kernel, 3, sizeof(int), &N);

    size_t global[2] = { (size_t)N, (size_t)N };

    auto start_gpu = std::chrono::high_resolution_clock::now();
    clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global, NULL, 0, NULL, NULL);
    clFinish(queue);
    auto end_gpu = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_gpu = end_gpu - start_gpu;
    std::cout << "🚀 GPU Time: " << elapsed_gpu.count() << " seconds\n";

    clEnqueueReadBuffer(queue, bufferC, CL_TRUE, 0, sizeof(float) * N*N, C.data(), 0, NULL, NULL);

    // تحقق من صحة النتائج
    int errors = 0;
    for (int i = 0; i < N*N; ++i)
        if (std::fabs(C[i] - C_cpu[i]) > 1e-2f) errors++;
    
    std::cout << "✅ الحسابات " << (errors == 0 ? "مطابقة" : "فيها " + std::to_string(errors) + " خطأ") << "\n";

    // تحرير الموارد
    clReleaseMemObject(bufferA);
    clReleaseMemObject(bufferB);
    clReleaseMemObject(bufferC);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
     //pausse
    int a;
    std::cin>>a;
    return 0;
}
