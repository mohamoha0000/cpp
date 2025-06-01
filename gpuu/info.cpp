#define CL_TARGET_OPENCL_VERSION 120
#include <CL/cl.h>
#include <iostream>

void print_gpu_info(cl_device_id device) {
    size_t maxWorkGroupSize;
    size_t maxWorkItemSizes[3];
    cl_uint computeUnits;

    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &maxWorkGroupSize, NULL);
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(maxWorkItemSizes), &maxWorkItemSizes, NULL);
    clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &computeUnits, NULL);

    std::cout << "\n=== GPU Device Info ===\n";
    std::cout << "Max Work Group Size      : " << maxWorkGroupSize << "\n";
    std::cout << "Max Work Item Sizes      : "
              << maxWorkItemSizes[0] << " x "
              << maxWorkItemSizes[1] << " x "
              << maxWorkItemSizes[2] << "\n";
    std::cout << "Compute Units (Cores)    : " << computeUnits << "\n";
    std::cout << "=========================\n\n";
}

int main() {
    cl_platform_id platform;
    cl_device_id device;

    // Get first available platform
    if (clGetPlatformIDs(1, &platform, NULL) != CL_SUCCESS) {
        std::cerr << "Failed to get OpenCL platform\n";
        return 1;
    }

    // Get first available GPU device
    if (clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL) != CL_SUCCESS) {
        std::cerr << "Failed to get GPU device\n";
        return 1;
    }

    // Print GPU information
    print_gpu_info(device);
    int a=0;
    std::cin>>a;
    return 0;
}

//g++ info.cpp -o info -I. -L. -lOpenCL