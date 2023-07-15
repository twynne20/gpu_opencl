#define CL_TARGET_OPENCL_VERSION 120
#include <cl.h>
#include <stdio.h>

int main() {
    // Get the number of platforms
    cl_uint num_platforms;
    clGetPlatformIDs(0, NULL, &num_platforms);

    // Get the platform IDs
    cl_platform_id platforms[num_platforms];
    clGetPlatformIDs(num_platforms, platforms, NULL);

    // Print each platform's version
    for (cl_uint i = 0; i < num_platforms; i++) {
        char buffer[1024];
        clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, 1024, &buffer, NULL);
        printf("Platform %d version: %s\n", i, buffer);
    }

    return 0;
}
