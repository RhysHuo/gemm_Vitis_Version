#include <iostream>
#include <fstream>
#include <string.h>
#include <chrono>
#include <algorithm>

#include "xcl2.hpp"
#include <CL/cl.h>
#include <CL/cl2.hpp>
#include "kernelMatrixmult.h"

//2022.07.05 13:36

#define NUM_TESTS 1

#define OCL_CHECK(error, call)                                                                   \
    call;                                                                                        \
    if (error != CL_SUCCESS) {                                                                   \
        printf("%s:%d Error calling " #call ", error code is: %d\n", __FILE__, __LINE__, error); \
        exit(EXIT_FAILURE);                                                                      \
    }

int SN, SM, SP;

static void init_arrays(DTYPE *B, DTYPE_OUT *C_sw, DTYPE_OUT *C)
{
    for (int i = 0; i < SM; i++) {    
        for (int j = 0; j < SP; j++) {
        	B[i * SP + j] =  0x01;
        }
    }
    for (int i = 0; i < SN; i++) {
        for (int j = 0; j < SP; j++) {
			C_sw[i * SP + j] = 0;
			C[i * SP + j] = 0;
		}
	}
}

static void load_arrays_quad(DTYPE *A, std::ifstream& myFile)
{
	// Make sure the file is open
	if(!myFile.is_open()) throw std::runtime_error("Could not open quad file");

	// Helper vars
	std::string line;
	int val;
	int val_count=0;
	DTYPE array_val;

    for (int i = 0; i < SN; i++) {
    	// Read data, line by line
    	std::getline(myFile, line);

	    // Create a stringstream of the current line
	    std::stringstream ss(line);

        for (int j = 0; j < SM; j++) {
	        //fill one array val
        	array_val = 0;
	        for(int z =0; z< DTYPE_LENGTH/4; z++)
	        {
	        	// Extract each integer
	        	ss >> val;
	        	switch(val)
	        	{
	        		case 0:
	        			array_val = array_val << 4;
	        			break;
	        		case 1:
	        			array_val = array_val << 4;
	        			array_val+= 1;
	        			break;
	        		case 2:
	        			array_val = array_val << 4;
	        			array_val+= 2;
	        			break;
	        		case 3:
	        			array_val = array_val << 4;
	        			array_val+= 3;
	        			break;
	        		case 4:
	        			array_val = array_val << 4;
	        			array_val+= 4;
	        			break;
	        		case 5:
	        			array_val = array_val << 4;
	        			array_val+= 5;
	        			break;
	        		case 6:
	        			array_val = array_val << 4;
	        			array_val+= 6;
	        			break;
	        		case 7:
	        			array_val = array_val << 4;
	        			array_val+= 7;
	        			break;
	        		case -1:
	        			array_val = array_val << 4;
	        			array_val+= 15;
	        			break;
	        		case -2:
	        			array_val = array_val << 4;
	        			array_val+= 14;
	        			break;
	        		case -3:
	        			array_val = array_val << 4;
	        			array_val+= 13;
	        			break;
	        		case -4:
	        			array_val = array_val << 4;
	        			array_val+= 12;
	        			break;
	        		case -5:
	        			array_val = array_val << 4;
	        			array_val+= 11;
	        			break;
	        		case -6:
	        			array_val = array_val << 4;
	        			array_val+= 10;
	        			break;
	        		case -7:
	        			array_val = array_val << 4;
	        			array_val+= 9;
	        			break;
	        		case -8:
	        			array_val = array_val << 4;
	        			array_val+= 8;
	        			break;

	        	}
	            // If the next token is a comma, ignore it and move on
	            if(ss.peek() == ',') ss.ignore();
	        }
	        A[i * SM + j] = array_val;
	        val_count++;
	    }
    }
    std::cout << "(QUAD) Val count " << val_count << std::endl;
}

static void load_arrays_tern(DTYPE *A,std::ifstream& myFile)
{
	// Make sure the file is open
	if(!myFile.is_open()) throw std::runtime_error("Could not open tern file");

	// Helper vars
	std::string line;
	int val;
	int val_count=0;
	DTYPE array_val;

    for (int i = 0; i < SN; i++) {
    	// Read data, line by line
    	std::getline(myFile, line);

	    // Create a stringstream of the current line
	    std::stringstream ss(line);

        for (int j = 0; j < SM; j++) {
	        //fill one array val
        	array_val = 0;
	        for(int z =0; z< DTYPE_LENGTH/2; z++)
	        {
	        	// Extract each integer
	        	ss >> val;
	        	switch(val)
	        	{
	        		case 0:
	        			array_val = array_val << 2;
	        			break;
	        		case 1:
	        			array_val = array_val << 2;
	        			array_val+= 1;
	        			break;
	        		case -1:
	        			array_val = array_val << 2;
	        			array_val+= 3;
	        			break;
	        	}
	            // If the next token is a comma, ignore it and move on
	            if(ss.peek() == ',') ss.ignore();
	        }
	        A[i * SM + j] = array_val;
	        val_count++;
	    }
    }
    std::cout << "(TERN) Val count " << val_count << std::endl;
}

static void load_arrays_byte(DTYPE *A,std::ifstream& myFile)
{
	// Make sure the file is open
	if(!myFile.is_open()) throw std::runtime_error("Could not open byte file");

	// Helper vars
	std::string line;
	int val;
	int val_count=0;
	DTYPE array_val;

    for (int i = 0; i < SN; i++) {
    	// Read data, line by line
    	std::getline(myFile, line);

	    // Create a stringstream of the current line
	    std::stringstream ss(line);

        for (int j = 0; j < SM; j++) {

	        //fill one array val
        	array_val = 0;
	        for(int z =0; z< DTYPE_LENGTH/8; z++)
	        {
	        	// Extract each integer
	        	ss >> val;
	        	array_val = (array_val << 8) + val;

	            // If the next token is a comma, ignore it and move on
	            if(ss.peek() == ',') ss.ignore();
	        }
	        A[i * SM + j] = array_val;
	        val_count++;
	    }
    }
    std::cout << "(BYTE) Val count " << val_count << std::endl;
}

void mmult_golden_ternary(DTYPE *A,  DTYPE *B, DTYPE_OUT *C)
{
	for (int row = 0; row < SN; row++) {
		for (int col = 0; col < SP; col++) {
			DTYPE_OUT result = 0;
			for (int k = 0; k < SM; k++) {
				for(int z = 0; z < DTYPE_LENGTH; z+=2) {
					DTYPE A_temp1 = A[row*SM+k];
					ap_int<2> A_val = A_temp1.range(z+1,z);
					DTYPE A_temp = A_val;
					ap_int<2> B_temp;
					switch(A_temp)
					{
						case 0:
							break;
						case 1:
							B_temp = B[k*SP+col].range(z+1,z);
							result += B_temp;
							break;
						case -1:
							B_temp = B[k*SP+col].range(z+1,z);
							result -= B_temp;
							break;
					}
				}
			}
			C[row*SP+col] = result;
		}
	}
}

void mmult_golden_quad(DTYPE *A,  DTYPE *B, DTYPE_OUT *C)
{
	for (int row = 0; row < SN; row++) {
		for (int col = 0; col < SP; col++) {
			DTYPE_OUT result = 0;
			for (int k = 0; k < SM; k++) {
				for(int z = 0; z < DTYPE_LENGTH; z+=4) {
					DTYPE A_temp1 = A[row*SM+k];
					ap_int<4> A_val = A_temp1.range(z+3,z);
					DTYPE A_temp = A_val;
					ap_int<4> B_temp;
					B_temp = B[k*SP+col].range(z+3,z);
					result+=A_val*B_temp;
				}
			}
			C[row*SP+col] = result;
		}
	}
}

void mmult_golden_byte(DTYPE *A,  DTYPE *B, DTYPE_OUT *C)
{
	for (int row = 0; row < SN; row++) {
		for (int col = 0; col < SP; col++) {
			DTYPE_OUT result = 0;
			for (int k = 0; k < SM; k++) {
				for(int z = 0; z < DTYPE_LENGTH; z+=8) {
					DTYPE A_temp1 = A[row*SM+k];
					ap_int<8> A_val = A_temp1.range(z+7,z);
					result+=A_val*B[k*SP+col];
				}
			}
			C[row*SP+col] = result;
		}
	}
}

static int result_check(DTYPE_OUT *C, DTYPE_OUT *C_sw)
{
	for (int i = 0; i < SN * SP; i++) {
		if (C_sw[i] != C[i]) {
			std::cout 	<< "Mismatch: data index= " << i << " golden = " << C_sw[i]
						<< ", kernel = " << C[i] << std::endl;
			return 1;
		}
	}
    std::cout 	<< "TEST PASSED !" <<  std::endl;
	return 0;
}

// main
int main(int argc, char* argv[]) {
    if (argc != 7) {
        std::cout << "Usage: " << argv[0] << " <xclbin>" << " myFile" << " ternary" << " SM" << " SP" << " SN" << std::endl;
        return EXIT_FAILURE;
    }
    
    std::string xclbinFilename = argv[1];
    std::vector<cl::Device> devices;
    cl_int err;
    cl::Context context;
    cl::CommandQueue q;
    cl::Kernel krnl;
    cl::Program program;
    std::vector<cl::Platform> platforms;
    bool found_device = false;

    // Traversing all Platforms To find Xilinx Platform and targeted
    // Device in Xilinx Platform
    cl::Platform::get(&platforms);
    for (size_t i = 0; (i < platforms.size()) & (found_device == false); i++) {
        cl::Platform platform = platforms[i];
        std::string platformName = platform.getInfo<CL_PLATFORM_NAME>();
        if (platformName == "Xilinx") {
            devices.clear();
            platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices);
            if (devices.size()) {
                found_device = true;
                break;
            }
        }
    }
    if (found_device == false) {
        std::cout << "Error: Unable to find Target Device " << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "INFO: Reading " << xclbinFilename << std::endl;
    FILE* fp;
    if ((fp = fopen(xclbinFilename.c_str(), "r")) == nullptr) {
        printf("ERROR: %s xclbin not available please build\n", xclbinFilename.c_str());
        exit(EXIT_FAILURE);
    }
    // Load xclbin
    std::cout << "Loading: '" << xclbinFilename << "'\n";
    std::ifstream bin_file(xclbinFilename, std::ifstream::binary);
    bin_file.seekg(0, bin_file.end);
    unsigned nb = bin_file.tellg();
    bin_file.seekg(0, bin_file.beg);
    char* buf = new char[nb];
    bin_file.read(buf, nb);

    // Creating Program from Binary File
    cl::Program::Binaries bins;
    bins.push_back({buf, nb});
    bool valid_device = false;
    for (unsigned int i = 0; i < devices.size(); i++) {
        auto device = devices[i];
        // Creating Context and Command Queue for selected Device
        OCL_CHECK(err, context = cl::Context(device, nullptr, nullptr, nullptr, &err));
        OCL_CHECK(err, q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
        std::cout << "Trying to program device[" << i << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        program = cl::Program(context, {device}, bins, NULL, &err);
        if (err != CL_SUCCESS) {
            std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
        } else {
            std::cout << "Device[" << i << "]: program successful!\n";
            OCL_CHECK(err, krnl = cl::Kernel(program, "kernelMatrixmult", &err));
            valid_device = true;
            break; // we break because we found a valid device
        }
    }
    if (!valid_device) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    SM = atoi(argv[4]);
    SP = atoi(argv[5]);
    SN = atoi(argv[6]);
	
	if(SP < B_WIDTH_BLOCK)
	{
		std::cout << "Error : P is less than (B_WIDTH_BLOCK = 32), kernel cannot be executed successfully.";
		return EXIT_FAILURE;
	}
	else
	{
		std::cout << "Complete to assign a value to N, M and P : " << "SN = " << argv[6] << " SM = " << argv[4] << " SP = " << argv[5] << std::endl;
	}

	// Map our user-allocated buffers as OpenCL buffers using a shared host pointer
    OCL_CHECK(err, cl::Buffer buffer_array_a(context, CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR , SN * SM * sizeof(DTYPE), NULL, &err));
    OCL_CHECK(err, cl::Buffer buffer_array_b(context, CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR , SM * SP * sizeof(DTYPE), NULL, &err));
    OCL_CHECK(err, cl::Buffer buffer_array_c(context, CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR , SN * SP * sizeof(DTYPE_OUT), NULL, &err));
	
	ap_uint<2> S_ternary = atoi(argv[3]);
	int S_begin = 0;
	int S_end = SN;

	// Set the kernal argument
    int narg = 0;
	OCL_CHECK(err, err = krnl.setArg(narg++, S_ternary));
    OCL_CHECK(err, err = krnl.setArg(narg++, buffer_array_a));
    OCL_CHECK(err, err = krnl.setArg(narg++, buffer_array_b));
    OCL_CHECK(err, err = krnl.setArg(narg++, buffer_array_c));
    OCL_CHECK(err, err = krnl.setArg(narg++, SM));
    OCL_CHECK(err, err = krnl.setArg(narg++, SP));
    OCL_CHECK(err, err = krnl.setArg(narg++, S_begin));
    OCL_CHECK(err, err = krnl.setArg(narg++, S_end));

    std::ifstream myFile(argv[2]);
    DTYPE* array_a;
    DTYPE* array_b;
    DTYPE_OUT* array_c;
    DTYPE_OUT* array_c_sw = new DTYPE_OUT[SN * SP];
    
	//Map buffers to userspace pointers
    OCL_CHECK(err, array_a = (DTYPE*)q.enqueueMapBuffer(buffer_array_a, CL_TRUE, CL_MAP_WRITE, 0, SN * SM * sizeof(DTYPE), nullptr, nullptr, &err));
    OCL_CHECK(err, array_b = (DTYPE*)q.enqueueMapBuffer(buffer_array_b, CL_TRUE, CL_MAP_WRITE, 0, SM * SP * sizeof(DTYPE), nullptr, nullptr, &err));
	OCL_CHECK(err, array_c = (DTYPE_OUT*)q.enqueueMapBuffer(buffer_array_c, CL_TRUE, CL_MAP_READ, 0, SN * SP * sizeof(DTYPE_OUT), nullptr, nullptr, &err));

	//start host and kernel function
    init_arrays(array_b, array_c_sw, array_c);
	    
    std::cout << "Complete : Init_arrays." << std::endl;

    for (int i = 0; i < NUM_TESTS; i++) 
	{

		if (S_ternary==0)
		{
			load_arrays_byte(array_a,myFile);
		}
		else if (S_ternary==1)
		{
			load_arrays_tern(array_a,myFile);
		}
		else
		{
			load_arrays_quad(array_a,myFile);
		}
	    
	    std::cout << "Complete : Load_arrays." << std::endl;
    }
	
	std::cout << "Start : Kernel execution." << std::endl;
	auto fpga_begin = std::chrono::high_resolution_clock::now();
    // Date will be migrate to the kernal space
	OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_array_a, buffer_array_b}, 0));
    
    // Lauch the kernal
    OCL_CHECK(err, err = q.enqueueTask(krnl));
    
    // To view the results, this call will transfer the data from FPGA to the host

	// Rather than manually enqueueing a migration, we can instead just map the buffer. 
	// The OpenCL runtime will recognize that the buffer contents are currently resident in 
	// the Alveo Data Center accelerator card global memory and will take care of 
	// migrating the buffer back to the host for us. This is a coding style choice you must make.

    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_array_c}, CL_MIGRATE_MEM_OBJECT_HOST));
    
    q.finish();
	auto fpga_end = std::chrono::high_resolution_clock::now();
	std::cout << "Complete : Kernel execution." << std::endl;

    // Compare the results of the Device to the simulation
    std::cout << "Start : mmult_golden." << std::endl;
	auto cpu_begin = std::chrono::high_resolution_clock::now();
	
    if (S_ternary==0)
    {
        mmult_golden_byte(array_a, array_b, array_c_sw);
    }
    else if (S_ternary==1)
    {
        mmult_golden_ternary(array_a, array_b, array_c_sw);
    }
    else
    {
        mmult_golden_quad(array_a, array_b, array_c_sw);
    }
	
	auto cpu_end = std::chrono::high_resolution_clock::now();
    std::cout << "Complete : mmult_golden." << std::endl;
	std::cout << "Start : result_check." << std::endl;

    if (result_check(array_c, array_c_sw))
        return 1;
	
	for(int k = 0; k < 256; k++)
		std::cout << "array_a = " << k << " " << array_a[k] << std::endl;
	
	
	for(int k = 0; k < 64; k++)
		std::cout << "array_c = " << k << " " << array_c[k] << std::endl;
	
	std::chrono::duration<double> fpga_duration = fpga_end - fpga_begin;
	std::chrono::duration<double> cpu_duration = cpu_end - cpu_begin;
	//float fpga_throughput = (double) numRuns*3*nbytes / fpga_duration.count() / (1024.0*1024.0);
     	//float cpu_throughput  = (double) numRuns*3*nbytes / cpu_duration.count() / (1024.0*1024.0);
	
	std::cout << std::endl;
	std::cout << "----------------------------------------------------------------------------"   << std::endl;
	std::cout << "         Performance  " << std::endl;
    	//std::cout << "          Total data: " << total << " MBits" << std::endl;
    	std::cout << "           FPGA Time: " << fpga_duration.count() * 1000.0 << " ms" << std::endl;
    	//std::cout << "     FPGA Throughput: " << total / fpga_duration.count() << " MBits/s" << std::endl;
    	//std::cout << "FPGA PCIe Throughput: " << (2*total) / fpga_duration.count() << " MBits/s" << std::endl;
	std::cout << "            CPU Time: " << cpu_duration.count() * 1000.0 << " ms" << std::endl;
	std::cout << "       FPGA Speedup : " << cpu_duration.count() / fpga_duration.count() << " x" << std::endl;
	std::cout << "----------------------------------------------------------------------------"   << std::endl;
	 
	OCL_CHECK(err, err = q.enqueueUnmapMemObject(buffer_array_a, array_a));
    OCL_CHECK(err, err = q.enqueueUnmapMemObject(buffer_array_b, array_b));
	OCL_CHECK(err, err = q.enqueueUnmapMemObject(buffer_array_c, array_c));
	q.finish();
}
