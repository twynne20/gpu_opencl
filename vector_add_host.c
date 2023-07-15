#include <stdio.h>
#include <stdlib.h>
#include <OpenCL/opencl.h>

#define MAX_SOURCE_SIZE (0x100000)

#define DATA_SIZE 1024

int main() {
    int dataA[DATA_SIZE];
    int dataB[DATA_SIZE];
    for(int i = 0; i < DATA_SIZE; i++) {
        dataA[i] = i;
        dataB[i] = DATA_SIZE - i;
    }

    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;   
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    FILE *fp;
    char fileName[] = "./vector_add_kernel.cl";
    char *source_str;
    size_t source_size;

    fp = fopen(fileName, "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);

    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);

    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    cl_kernel kernel = clCreateKernel(program, "vector_add", &ret);

    cl_mem memobjA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * DATA_SIZE, dataA, &ret);
    cl_mem memobjB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * DATA_SIZE, dataB, &ret);
    cl_mem memobjC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * DATA_SIZE, NULL, &ret);

    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobjA);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&memobjB);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&memobjC);

    size_t global_item_size = DATA_SIZE;
    size_t local_item_size = 64; // Can be set to 1 if this line of code gives you trouble

    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

    int dataC[DATA_SIZE];
    ret = clEnqueueReadBuffer(command_queue, memobjC, CL_TRUE, 0, DATA_SIZE * sizeof(int), dataC, 0, NULL, NULL);

    for(int i = 0; i < 10; i++) {
        printf("%d + %d = %d\n", dataA[i], dataB[i], dataC[i]);
    }

    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(memobjA);
    ret = clReleaseMemObject(memobjB);
    ret = clReleaseMemObject(memobjC);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    free(source_str);

    return 0;
}
