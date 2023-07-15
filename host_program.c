#include <stdio.h>
#include <stdlib.h>
#include <OpenCL/opencl.h>

#define MAX_SOURCE_SIZE (0x100000)
#define ARRAY_SIZE 1024

int main() {
    // Load the kernel source code into an array source_str
    FILE *fp;
    char *source_str;
    size_t source_size;

    fp = fopen("sum_arrays.cl", "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);

    // Create and initialize arrays A and B on the host
    float *A = (float*)malloc(sizeof(float) * ARRAY_SIZE);
    float *B = (float*)malloc(sizeof(float) * ARRAY_SIZE);
    for(int i = 0; i < ARRAY_SIZE; i++) {
        A[i] = i;
        B[i] = i;
    }

    // Get platform and device information
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;   
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

    // Create an OpenCL context
    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

    // Create a command queue
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    // Create memory buffers on the device for each array (A, B, C)
    cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * ARRAY_SIZE, NULL, &ret);
    cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * ARRAY_SIZE, NULL, &ret);
    cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * ARRAY_SIZE, NULL, &ret);

    // Copy arrays A and B to the device
    ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0, sizeof(float) * ARRAY_SIZE, A, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0, sizeof(float) * ARRAY_SIZE, B, 0, NULL, NULL);

    // Create a program from the kernel source
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);

    // Build the program
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "sum_arrays", &ret);

    // Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_mem_obj);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&c_mem_obj);

    // Execute the OpenCL kernel
    size_t global_item_size = ARRAY_SIZE;
    size_t local_item_size = 64;
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

    // Read the memory buffer C on the device to the local variable C
    float *C = (float*)malloc(sizeof(float) * ARRAY_SIZE);
    ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0, sizeof(float) * ARRAY_SIZE, C, 0, NULL, NULL);

    // Display the result to the screen
    for(int i = 0; i < 10; i++)
        printf("%f ", C[i]);
    printf("\n");

    // Clean up
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(a_mem_obj);
    ret = clReleaseMemObject(b_mem_obj);
    ret = clReleaseMemObject(c_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    free(A);
    free(B);
    free(C);
    return 0;
}
