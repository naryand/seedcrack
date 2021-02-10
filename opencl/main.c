#define CL_TARGET_OPENCL_VERSION 120
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#define MAX_SOURCE_SIZE (0x100000)
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    cl_device_id device_id = NULL;
    cl_context context = NULL;
    cl_command_queue command_queue = NULL;
    cl_program program = NULL;
    cl_kernel kernel = NULL;
    cl_platform_id platform_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret;

    FILE *fp;
    char fileName[] = "./kernel.cl";
    char *source_str;
    size_t source_size;

    fp = fopen(fileName, "r");
    if(!fp) {
        fprintf(stderr, "Failed to load kernel\n");
        exit(1);
    }
    source_str = (char *) malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);

    ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
    
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
    command_queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &ret);

    program = clCreateProgramWithSource(context, 1, (const char **) &source_str, 
                                        (const size_t *) &source_size, &ret);
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    if(ret == CL_BUILD_PROGRAM_FAILURE) {
        // Determine the size of the log
        size_t log_size;
        ret = clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        // Allocate memory for the log
        char *log = (char *) malloc(log_size);
        // Get the log
        ret = clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        // Print the log
        printf("%s\n", log);
    }

    kernel = clCreateKernel(program, "find", &ret);

    // ITEMS*GROUPS*SIZE executions of kernel total

    // items per group
    #define ITEMS 1024
    // groups per kernel
    #define GROUPS 1073741824
    // # of times to execute the kernel
    #define SIZE 256

    size_t local_work_size = ITEMS;
    size_t global_work_size = ((size_t) ITEMS)*((size_t) GROUPS);

    cl_event event;
    cl_ulong time_start;
    cl_ulong time_end;
    cl_ulong time_sum = 0;

    for(int i = 0; i < SIZE; i++) {
        size_t offset = global_work_size*((size_t) i);
        ret = clSetKernelArg(kernel, 0, sizeof(int), &i);
        ret = clEnqueueNDRangeKernel(command_queue, 
                                     kernel, 1, &offset, 
                                     &global_work_size, 
                                     &local_work_size, 
                                     0, NULL, &event);
        ret = clWaitForEvents(1, &event);
        ret = clFinish(command_queue);

        ret = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, 
                                      sizeof(time_start), &time_start, NULL);
        ret = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, 
                                      sizeof(time_end), &time_end, NULL);
        time_sum += (time_end - time_start);
    }

    //ret = clFlush(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    ret = clReleaseEvent(event);

    cl_double nanoSeconds = time_sum;
    printf("OpenCL Execution time is: %f seconds \n", nanoSeconds / 1000000000.0);
    free(source_str);

    return 0;
}
