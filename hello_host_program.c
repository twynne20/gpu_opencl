#include <stdio.h>
#include <stdlib.h>
#include <OpenCL/opencl.h>

#define MAX_SOURCE_SIZE (0x100000)

int main() {
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;   
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    FILE *fp;
    char fileName[] = "./hello.cl";
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

    cl_kernel kernel = clCreateKernel(program, "hello_kernel", &ret);

    int num_work_items = 10;
    size_t local_item_size = 1;
    size_t global_item_size = num_work_items * local_item_size;

    cl_mem greetings_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, num_work_items * 13 * sizeof(char), NULL, &ret);

    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&greetings_mem_obj);

    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

    char* greetings = (char*)malloc(num_work_items * 13 * sizeof(char));
    ret = clEnqueueReadBuffer(command_queue, greetings_mem_obj, CL_TRUE, 0, num_work_items * 13 * sizeof(char), greetings, 0, NULL, NULL);

    for (int i = 0; i < num_work_items; i++) {
        printf("%.*s\n", 13, greetings + i * 13);
    }

    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(greetings_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    free(source_str);
    free(greetings);

    return 0;
}
