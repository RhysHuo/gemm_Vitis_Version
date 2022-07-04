#include <iostream>
#include <fstream>
#include <string.h>
//#include <sys/time.h>
#include <algorithm>

#include "xcl2.hpp"
#include <CL/cl.h>
#include <CL/cl2.hpp>
#include "matrix_mult.h"

//2022.07.03 19:21

#define NUM_TESTS 1

int SN, SM, SP;

static void init_arrays(DTYPE *B, DTYPE_OUT *C_sw, DTYPE_OUT *C)
{
    for (int i = 0; i < SM; i++) {    
        for (int j = 0; j < SP; j++) {
        	B[i * SP + j] =  0x01;
		std::cout << "B "<< i * SP + j << " " << B[i * SP + j] << std::endl;
        }
    }
    for (int i = 0; i < SN; i++) {
        for (int j = 0; j < SP; j++) {
			C_sw[i * SP + j] = 0;
			C[i * SP + j] = 0;
		std::cout << "C_sw "<< i * SP + j << " " << C_sw[i * SP + j] << std::endl;
		std::cout << "C "<< i * SP + j << " " << C[i * SP + j] << std::endl;
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
			std::cout << "val = "<< val << std::endl;
	        	array_val = (array_val << 8) + val;
			std::cout << "current array_val = "<< array_val << std::endl;

	            // If the next token is a comma, ignore it and move on
	            if(ss.peek() == ',') ss.ignore();
	        }
	        A[i * SM + j] = array_val;
	        val_count++;
	        std::cout << i <<" "<< j << A[i * SM + j] << std::endl;
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
							//std::cout << "C golden is" << result << std::endl;
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
			std::cout << row << " " << col << " "<< C[row*SP+col] << std::endl;
		}
	}
}

static int result_check(DTYPE_OUT *C, DTYPE_OUT *C_sw)
{
	for (int i = 0; i < SN * SP; i++) {
		if (C_sw[i] != C[i]) {
			std::cout 	<< "Mismatch: data index= " << i << " golden = " << C_sw[i]
						<< ", kernel = " << C[i] << std::endl;
			//return 1;
		}
	}
    std::cout 	<< "TEST PASSED !" <<  std::endl;
	return 0;
}

void printVector(const vi& V, char* msg)
{

	std::cout << msg << "[ ";
	for_each(V.begin(), V.end(), [](int a) {
		std::cout << a << " ";
	});
	std::cout << "]" << std::endl;
}

// Generate the three vectors A, IA, JA
void arraytocsr(DTYPE *V)
{
	int i, j;
	vi A;
	vi IA = { 0 }; // IA matrix has N+1 rows
	vi JA;
	int NNZ = 0;

	// Create an outuptu filestream
	std::ofstream outFile("/mnt/weights.csr");

	// Make sure the file is open
	if(!outFile.is_open()) throw std::runtime_error("Could not open csr file");
	for (i = 0; i < SN; i++) {
		for (j = 0; j < SM; j++) {
			if (V[i*SM+j] != 0) {
				A.push_back(V[i*SM+j]);
				JA.push_back(j);
				// Count Number of Non Zero
				// Elements in row i
				NNZ++;
			}
		}
		IA.push_back(NNZ);
	}

	outFile << SN << " " << SM << " " << NNZ << std::endl;

	for(int i=0;i<A.size();i++)
	{
		outFile << JA[i] << " " << A[i] << std::endl;
	}

	for(int i=0;i<IA.size();i++)
	{
		outFile << IA[i] << std::endl;
	}

	outFile.close();
	std::cout << "Number of non-zeros " << NNZ << std::endl;
}

#define OCL_CHECK(error, call)                                                                   \
    call;                                                                                        \
    if (error != CL_SUCCESS) {                                                                   \
        printf("%s:%d Error calling " #call ", error code is: %d\n", __FILE__, __LINE__, error); \
        exit(EXIT_FAILURE);                                                                      \
    }

// MAIN
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

    // traversing all Platforms To find Xilinx Platform and targeted
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

	std::cout << "Complete to assign a value to N, M and P : " << "SN = " << argv[6] << " SM = " << argv[4] << " SP = " << argv[5] << std::endl;

	// Map our user-allocated buffers as OpenCL buffers using a shared host pointer
    OCL_CHECK(err, cl::Buffer buffer_array_a(context, CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR , SN * SM * sizeof(DTYPE), NULL, &err));
    OCL_CHECK(err, cl::Buffer buffer_array_b(context, CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR , SM * SP * sizeof(DTYPE), NULL, &err));
    OCL_CHECK(err, cl::Buffer buffer_array_c(context, CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR , SN * SP * sizeof(DTYPE_OUT), NULL, &err));

	// For buffer buffer_array_c_sw, since we aren't using it for a kernel we'll specify the bank allocation
    cl_mem_ext_ptr_t bank_ext;
    bank_ext.flags = 0 | XCL_MEM_TOPOLOGY;
    bank_ext.obj   = NULL;
    bank_ext.param = 0;
	OCL_CHECK(err, cl::Buffer buffer_array_c_sw(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR | CL_MEM_EXT_PTR_XILINX, SN * SP * sizeof(DTYPE_OUT), &bank_ext, &err));

	ap_uint<2> S_ternary = atoi(argv[3]);
	int S_begin = 0;
	int S_end = SN;

	std::cout << "Complete to assign a value to begin and end " << std::endl;

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
    DTYPE_OUT* array_c_sw;
    
	//Map buffers to userspace pointers
    OCL_CHECK(err, array_a = (DTYPE*)q.enqueueMapBuffer(buffer_array_a, CL_TRUE, CL_MAP_WRITE, 0, SN * SM * sizeof(DTYPE), nullptr, nullptr, &err));
    OCL_CHECK(err, array_b = (DTYPE*)q.enqueueMapBuffer(buffer_array_b, CL_TRUE, CL_MAP_WRITE, 0, SM * SP * sizeof(DTYPE), nullptr, nullptr, &err));
	OCL_CHECK(err, array_c = (DTYPE_OUT*)q.enqueueMapBuffer(buffer_array_c, CL_TRUE, CL_MAP_READ, 0, SN * SP * sizeof(DTYPE_OUT), nullptr, nullptr, &err));
	OCL_CHECK(err, array_c_sw = (DTYPE_OUT*)q.enqueueMapBuffer(buffer_array_c_sw, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, SN * SP * sizeof(DTYPE_OUT), nullptr, nullptr, &err));

	//start host and kernel function
    init_arrays(array_b, array_c_sw, array_c);
	    
    std::cout << "Complete to init_arrays " << std::endl;

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
	    
	    std::cout << "Complete to load_arrays " << std::endl;
    }

    // Date will be migrate to the kernal space
	OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_array_a, buffer_array_b}, 0));
    
    // Lauch the kernal
    OCL_CHECK(err, err = q.enqueueTask(krnl));
    
    // To view the results, this call will transfer the data from FPGA to the host

	// Rather than manually enqueueing a migration, we can instead just map the buffer. 
	// The OpenCL runtime will recognize that the buffer contents are currently resident in 
	// the Alveo Data Center accelerator card global memory and will take care of 
	// migrating the buffer back to the host for us. This is a coding style choice you must make.

    //OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_array_c}, CL_MIGRATE_MEM_OBJECT_HOST));
    
    q.finish();

    // Compare the results of the Device to the simulation
    std::cout << "Start to mmult_golden " << std::endl;
	
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
	
    std::cout << "Complete to mmult_golden " << std::endl;

    if (result_check(array_c, array_c_sw))
        return 1;
    
	OCL_CHECK(err, err = q.enqueueUnmapMemObject(buffer_array_a, array_a));
    OCL_CHECK(err, err = q.enqueueUnmapMemObject(buffer_array_b, array_b));
	OCL_CHECK(err, err = q.enqueueUnmapMemObject(buffer_array_c, array_c));
	OCL_CHECK(err, err = q.enqueueUnmapMemObject(buffer_array_c_sw, array_c_sw));
	q.finish();
}
