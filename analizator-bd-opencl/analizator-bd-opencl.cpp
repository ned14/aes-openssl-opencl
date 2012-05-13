#include "stdafx.h"
#include "CL/cl.h"
#include "cl_encrypt.h"
#ifdef TEST_CL_ENCRYPT
	#include "cl_encrypt_test.h"
#endif
#include <fstream>
#include <sstream>
#include <ctime>
#include "rijndael.h"

#define KiB * 1024
#define MiB * 1024 KiB

/*
cl_int err;
cl_platform_id cp_platform;
cl_device_id device_id;
cl_context context;
cl_command_queue queue;
cl_program program;
cl_kernel kernel;

#define KEYBITS 256

#define BUFFER_SIZE 8 * MiB
#define ROUNDKEYS_SIZE 16 * 15
#define IV_SIZE 8
cl_mem state;
cl_mem roundkeys;
unsigned int argi;
uint32_t count = 128;

unsigned char *inbuf = NULL;
unsigned char *outbuf = NULL;
unsigned char *keys;
unsigned char *iv;

clock_t start, end;

void print_error(cl_int err);
void init(const char *filename);
void create_kernel(const char *kernel_name);
void create_buffers();
void execute(size_t count);
void release();
*/

int main(int argc, char **argv)
{
#ifdef PRINT_TEST_DATA
	print_test_data();
	return EXIT_SUCCESS;
#endif

#ifdef TEST_CL_ENCRYPT
	cl_encrypt_test();
#endif
	const size_t rk_length = 256;
	uint32_t rk[RKLENGTH(rk_length)];
	char key[KEYLENGTH(rk_length)] = "tajny klucz";

	rijndaelSetupEncrypt((unsigned long *) rk, (uint8_t *) key, rk_length);

	const size_t data_length = 128 MiB;
	uint8_t *data = new uint8_t[data_length];

	memset(data, 0, data_length);

	cl_init("aes_encrypt_256", data_length, RKLENGTH(rk_length));

#ifdef PRINT_PLATFORM_INFO
	cl_print_platform_info();
#endif

#ifdef PRINT_DEVICE_INFO
	cl_print_device_info();
#endif

	clock_t start, end;

	start = clock();
	cl_encrypt(data, rk);
	end = clock();

	printf("OpenCL   %ums\n", ((1000*(end-start))/CLOCKS_PER_SEC));

	start = clock();

	for (size_t pos = 0; pos < data_length; pos += 16)
	{
		rijndaelEncrypt((unsigned long *) rk, 14, data + pos, data + pos);
	}

	end = clock();

	printf("Rijndael %ums\n", ((1000*(end-start))/CLOCKS_PER_SEC));

	/*
	for (size_t pos = 0; pos < data_length; pos += 512*16)
	{
		uint8_t zeros[512*8] = {0};

		if (memcmp(data + pos, zeros, 512*8) != 0)
		{
			printf("%u\n", pos);
		}
	}
	*/

	cl_release_all();
	
	delete[] data;
	/*
	char *source = "./aes-encrypt.cl";

	printf("Loading \"%s\"\n", source);
	start = clock();
	init(source);
	create_kernel("aes_encrypt_256");
	end = clock();

	printf("Ready (%ums)\n", end - start);

	printf("\nGPU info:\n");
	unsigned long int param_value;
	
	param_value = clGetDeviceInfo(device_id, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(param_value), &param_value, NULL);
	{
		double memory = (double) param_value;
		char units[][4] = {"B  ", "KiB", "MiB", "GiB"};
		char *unit = units[0];
		for (int i = 1; memory >= 1024; i++)
		{
			memory /= 1024;
			unit = units[i];
		}
		printf("  local memory size        %.2f %s\n", memory, unit);
	}
	clGetDeviceInfo(device_id, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(param_value), &param_value, NULL);
	{
		double memory = (double) param_value;
		char units[][4] = {"B  ", "KiB", "MiB", "GiB"};
		char *unit = units[0];
		for (int i = 1; memory >= 1024; i++)
		{
			memory /= 1024;
			unit = units[i];
		}
		printf("  global memory size       %.2f %s\n", memory, unit);
	}
	clGetDeviceInfo(device_id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(param_value), &param_value, NULL);
	printf("  maximum compute units    %u\n", param_value);
	clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(param_value), &param_value, NULL);
	printf("  maximum work-group size  %u\n", param_value);
	clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(param_value), &param_value, NULL);
	printf("  maximum work-item sizes  %u\n", param_value);

	inbuf = new unsigned char[BUFFER_SIZE];
	outbuf = new unsigned char[BUFFER_SIZE];
	int inlen = BUFFER_SIZE,
		outlen = 0;

	if (argc > 1)
	{
		count = atoi(argv[1]);
	}

	// fill input buffer
	memset(inbuf, 0, sizeof(*inbuf) * BUFFER_SIZE);

	// simple key and iv
	keys = new unsigned char[ROUNDKEYS_SIZE];
	iv = new unsigned char[IV_SIZE];
	unsigned int key_length = (ROUNDKEYS_SIZE == 240) ? 32 : ((ROUNDKEYS_SIZE == 208) ? 24 : 16);

	for (unsigned int i = 0; i < key_length; i++) keys[i] = i + 1;

	for (unsigned int i = 0; i < IV_SIZE; i++) iv[i] = 0;

	key_schedule(keys, ROUNDKEYS_SIZE / 16);

	printf("\nKey info:\n");
	printf("  key length: %u bits\n", key_length);
	printf("  expanded key length: %u bytes\n", ROUNDKEYS_SIZE);
	printf("  rounds: %u\n", ROUNDKEYS_SIZE / 16);

	create_buffers();

	clEnqueueWriteBuffer(queue, state,     CL_TRUE, 0, sizeof(*inbuf) * BUFFER_SIZE,   inbuf,  0, NULL, NULL);
	clEnqueueWriteBuffer(queue, roundkeys, CL_TRUE, 0, sizeof(*keys) * ROUNDKEYS_SIZE, keys,   0, NULL, NULL);

	start = clock();
	execute(count);
	end = clock();

	err = clEnqueueReadBuffer(queue, state, CL_TRUE, 0, sizeof(*inbuf) * BUFFER_SIZE, outbuf, 0, NULL, NULL);

	if (err != CL_SUCCESS) {
		printf("Error: failed to read from buffer\n", err);
		exit(EXIT_FAILURE);
	}

	for (uint32_t i = 0; i < count; i++)
	{
		printf("%2x ", outbuf[i]);
		if ((i+1) % 16 == 0)
		{
			printf("\n");
		}
	}
	release();
	*/

	return EXIT_SUCCESS;
}
/*
void print_error(cl_int err)
{
	switch (err)
	{
		case CL_INVALID_ARG_INDEX: printf("- Invalid argument index\n"); break;
		case CL_INVALID_ARG_SIZE: printf(" - Invalid argument data size\n"); break;
		case CL_INVALID_ARG_VALUE: printf("- Invalid argument value\n"); break;
		case CL_INVALID_COMMAND_QUEUE: printf("- Invalid command-queue\n"); break;
		case CL_INVALID_CONTEXT: printf("- Invalid context or buffer\n"); break;
		case CL_INVALID_EVENT_WAIT_LIST: printf("- Invalid event-wait list\n"); break;
		case CL_INVALID_GLOBAL_OFFSET: printf(" - global_work_offset is not NULL\n"); break;
		case CL_INVALID_KERNEL: printf(" - Invalid kernel object\n"); break;
		case CL_INVALID_KERNEL_ARGS: printf(" - Kernel argument values have not been specified\n"); break;
		case CL_INVALID_MEM_OBJECT: printf("- Invalid buffer\n"); break;
		case CL_INVALID_PROGRAM_EXECUTABLE: printf(" - No valid program avaible\n"); break;
		case CL_INVALID_SAMPLER: printf("- Invalid sampler object\n"); break;
		case CL_INVALID_VALUE: printf("- Offset or size out-of-bounds or *results is a NULL value\n"); break;
		case CL_INVALID_WORK_DIMENSION: printf(" - work_dim is not a valid value\n"); break;
		case CL_INVALID_WORK_GROUP_SIZE: printf(" - global_work_size is not evenly divisable by local_work_size\n"); break;
		case CL_INVALID_WORK_ITEM_SIZE: printf(" - local_work_size is greater than CL_DEVICE_MAX_WORK_ITEM_SIZES\n"); break;
		case CL_MEM_OBJECT_ALLOCATION_FAILURE: printf("- Failed to allocate or read memory for data store associated with buffer\n"); break;
		case CL_OUT_OF_HOST_MEMORY: printf("- Failed to allocate or read resources required by the OpenCL implementation on the host\n"); break;
		case CL_OUT_OF_RESOURCES: printf(" - Nobody knows what happened (CL_OUT_OF_RESOURCES)\n"); break;
	}
}

void init(const char *file_name)
{
	err = clGetPlatformIDs(1, &cp_platform, NULL);
	if (err != CL_SUCCESS) {
		printf("Error: could not connect to compute device\n");
		print_error(err);
		exit(EXIT_FAILURE);
	}

	device_id = NULL;
	// Get a device of the appropriate type
	err = clGetDeviceIDs(cp_platform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);

	if (err != CL_SUCCESS) {
		printf("Warning: no GPU support, falling back to CPU\n");

		err = clGetDeviceIDs(cp_platform, CL_DEVICE_TYPE_CPU, 1, &device_id,
		                     NULL);
	}

	if ( ! device_id) {
		printf("Error: could not get device id\n");
		print_error(err);
		exit(EXIT_FAILURE);
	}

	// Create a compute context
	context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
	if ( ! context) {
		printf("Error: failed to create a compute context\n");
		print_error(err);
		exit(EXIT_FAILURE);
	}

	// Create a command queue
	queue = clCreateCommandQueue(context, device_id, 0, &err);
	if ( ! queue) {
		printf("Error: failed to create a command queue\n");
		print_error(err);
		exit(EXIT_FAILURE);
	}

	std:: fstream f(file_name, std::ios::in);

	if ( ! f.is_open())
	{
		printf("Error: failed to open %s\n", file_name);
		exit(EXIT_FAILURE);
	}

	f.seekg (0, std::ios::end);
	size_t size = (size_t) f.tellg();
	f.seekg (0, std::ios::beg);

	if (size == 0)
	{
		printf("Error: %s is empty\n", file_name);
		exit(EXIT_FAILURE);
	}

	char *source_str = new char[size];

	f.read(source_str, size);

	program = clCreateProgramWithSource(context, 1, (const char **) &source_str,
	                                    NULL, &err);
	if ( ! program) {
		printf("Error: failed to create compute program\n");
		print_error(err);
		exit(EXIT_FAILURE);
	}

	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (err != CL_SUCCESS) {
		size_t len;
		char buffer[1024 * 10]; // error message buffer, 10 KiB

		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG,
		                      sizeof(buffer), buffer, &len);

		printf("Error: failed to create program executable\n%s\n", buffer);
		print_error(err);
		exit(EXIT_FAILURE);
	}
}

void create_kernel(const char *kernel_name)
{
	// Create the compute kernel in the program
	kernel = clCreateKernel(program, kernel_name, &err);
	if ( ! kernel || err != CL_SUCCESS) {
		printf("Error: failed to create compute kernel\n");
		print_error(err);
		exit(EXIT_FAILURE);
	}

	// reset arguments index
	argi = 0;
}

void create_buffers()
{
	state = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, sizeof(*inbuf) * BUFFER_SIZE, NULL, &err);
	if (err != CL_SUCCESS)
	{
		printf("Error: failed to allocate device memory (buffer, size: %lu)\n",
		       BUFFER_SIZE);
		print_error(err);
		exit(EXIT_FAILURE);
	}
	
	roundkeys = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(*keys) * ROUNDKEYS_SIZE, NULL, &err);
	if (err != CL_SUCCESS)
	{
		printf("Error: failed to allocate device memory (roundkeys, size: %lu)\n",
		       ROUNDKEYS_SIZE);
		print_error(err);
		exit(EXIT_FAILURE);
	}

	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &state);
	if (err != CL_SUCCESS)
	{
		printf("Error: failed to bind kernel argument \"buffer\" [#0]\n");
		print_error(err);
		exit(EXIT_FAILURE);
	}

	err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &roundkeys);
	if (err != CL_SUCCESS)
	{
		printf("Error: failed to bind kernel argument \"roundkeys\" [#1]\n");
		print_error(err);
		exit(EXIT_FAILURE);
	}

	err = clSetKernelArg(kernel, 2, sizeof(count), &count);
	if (err != CL_SUCCESS)
	{
		printf("Error: failed to bind kernel argument \"limit\" [#2]\n");
		print_error(err);
		exit(EXIT_FAILURE);
	}
}

void execute(unsigned int count)
{
	if (count == 0)
	{
		printf("Error: execute param \"count\" equals zero\n");
		exit(EXIT_FAILURE);
	}

	size_t local, global;
	// Get the maximum work group size for executing the kernel on the device
	err = clGetKernelWorkGroupInfo(kernel, device_id,
					CL_KERNEL_WORK_GROUP_SIZE,
					sizeof(local), &local, NULL);
	if (err != CL_SUCCESS) {
		printf("Error: failed to retrieve kernel work group info\n%i\n", err);
		exit(EXIT_FAILURE);
	}

	global = (count / local) * local;

	if (count % local)
	{
		global += local;
	}

	printf("\nInfo: global %i, local %i, count %i\n", global, local, count);

	// Execute the kernel over the vector using the
	// maximum number of work group items for this device
	err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global, &local, 0,
	                             NULL, NULL);
	if (err != CL_SUCCESS) {
		printf("Error: failed to execute kernel\n");
		print_error(err);
		exit(EXIT_FAILURE);
	}

	// Wait for all queue to complete
	clFinish(queue);
}

void release()
{
	clReleaseMemObject(state);
	clReleaseMemObject(roundkeys);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);
}
*/