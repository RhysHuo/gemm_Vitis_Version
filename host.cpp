#include <iostream>
#include <fstream>
#include <string.h>
//#include <sys/time.h>
#include <algorithm>

#include "xcl2.hpp"
#include <CL/cl.h>
#include "matrix_mult.h"

#define NUM_TESTS 1

int SN, SM, SP;

static void init_arrays(DTYPE *B, DTYPE_OUT *C_sw, DTYPE_OUT *C)
{
    for (int i = 0; i < SM; i++) {
        for (int j = 0; j < SP; j++) {
            //B[i * P + j] = DTYPE(rand() % (M * P));
            //B[i * P + j] =  DTYPE(M * P);
        	B[i * SP + j] =  0x01;
        	// std::cout << "B "<< i * P + j << " " << B[i * P + j] << std::endl;
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

	// Create an input filestream
    //std::ifstream myFile("/mnt/weights_qau.csv");

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

	        //A[i * N + j] = 16;
	        //std::cout << i <<" "<< j << " " << array_val << " " << A[i * N + j] << std::endl;
	        A[i * SM + j] = array_val;
	        val_count++;
	        //std::cout << i <<" "<< j << " " << array_val << " " << A[i * N + j] << std::endl;

	    }
    }

    std::cout << "(QUAD) Val count " << val_count << std::endl;
}


static void load_arrays_tern(DTYPE *A,std::ifstream& myFile)
{

	// Create an input filestream
    //std::ifstream myFile("/mnt/weights_tern.csv");

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

	        //A[i * N + j] = 16;
	        //std::cout << i <<" "<< j << " " << array_val << " " << A[i * N + j] << std::endl;
	        A[i * SM + j] = array_val;
	        val_count++;
	        //std::cout << i <<" "<< j << " " << array_val << " " << A[i * N + j] << std::endl;

	    }
    }

    std::cout << "(TERN) Val count " << val_count << std::endl;
}


static void load_arrays_byte(DTYPE *A,std::ifstream& myFile)
{

	// Create an input filestream
    //std::ifstream myFile("/mnt/weights_byte.csv");

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

	        //A[i * N + j] = 16;
	        //std::cout << i <<" "<< j << " " << array_val << " " << A[i * N + j] << std::endl;
	        A[i * SM + j] = array_val;
	        val_count++;
	        //std::cout << i <<" "<< j << " " << array_val << " " << A[i * N + j] << std::endl;

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
							//std::cout << "B is" << b[k][j].range(z+1,z) << std::endl;
							//std::cout << "C golden is" << result << std::endl;
							break;
						case -1:
							B_temp = B[k*SP+col].range(z+1,z);
							result -= B_temp;
							//std::cout << "B is" << b[k][j].range(z+1,z) << std::endl;
							//std::cout << "C golden is" << result << std::endl;
							break;
					}
				}
			//std::cout << "result is " << result << std::endl;
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
				//std::cout << "result is " << result << std::endl;
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
			//std::cout << row << " " << col << " result is " << result << std::endl;
		}
		//std::cout << row << " " << col << " result is " << result << std::endl;
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
		//else
		//  std::cout << "out :data index= " << i << " golden = " << C_sw[i] << std::endl;
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
			//std::cout << i <<" "<< j << " " << V[i*M+j] << std::endl;
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
		//std::cout << JA[i] << " " << A[i] << std::endl;
	}

	for(int i=0;i<IA.size();i++)
	{
		outFile << IA[i] << std::endl;
		//std::cout << IA[i] << std::endl;
	}

	outFile.close();
	std::cout << "Number of non-zeros " << NNZ << std::endl;
	//printVector(A, (char*)"A = ");
	//printVector(IA, (char*)"IA = ");
	//printVector(JA, (char*)"JA = ");
}

#define OCL_CHECK(error, call)                                                                   \
    call;                                                                                        \
    if (error != CL_SUCCESS) {                                                                   \
        printf("%s:%d Error calling " #call ", error code is: %d\n", __FILE__, __LINE__, error); \
        exit(EXIT_FAILURE);                                                                      \
    }

int main(int argc, char* argv[]) {
    if (argc != 7) {
        std::cout << "Usage: " << argv[0] << " <xclbin>" << "myFile" << "ternary" << "SM" << "SP" << "SN" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream myFile(argv[2]);
    ap_uint<2> S_ternary = atoi(argv[3]);
    ap_uint<2>* ternary = &S_ternary;
    DTYPE* array_a;
    DTYPE* array_b;
    DTYPE_OUT* array_c;
    DTYPE_OUT* array_c_sw;
    SM = atoi(argv[4]);
    SP = atoi(argv[5]);
    SN = atoi(argv[6]);
    
    int *N = &SN;
    int *M = &SM;
    int *P = &SP;
	
    std::cout << "Complete to assign a value to N, M and P : " << "SN = " << argv[6] << " SM = " << argv[4] << " SP = " << argv[5] << std::endl;
	
    int S_begin = 0;
    int* begin = &S_begin;
    int* end = N;
	
    std::cout << "Complete to assign a value to begin and end " << std::endl;

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
	    
	    	std::cout << "Complete to load_arrays_byte " << std::endl;

		//arraytocsr(A);
		//double start_time, end_time, execution_time;

		//======================ONLY CPU ==========================================

		
		//start_time = getTimestamp();
		init_arrays(array_b, array_c, array_c);
	    
		std::cout << "Complete to init_arrays " << std::endl;



		//std::cout << "Kernel" << std::endl;
		//start_time = getTimestamp();
		//kernelMatrixmult1(ternary,A,B,C,SM,SP,0,SN);
		//end_time = getTimestamp();

		//execution_time = (end_time - start_time) / (1000);
		//std::cout << "FPGA " << " Total execution time = " << execution_time << " msec" << std::endl;



		//std::cout << "Golden" << std::endl;
		//std::cout << "A0  is" << A[0] << std::endl;

		//start_time = getTimestamp();
        
        /*
        if (ternary==0)
        {
            mmult_golden_byte(array_a, array_b, array_c_sw);
        }
        else if (ternary==1)
        {
            mmult_golden_ternary(array_a, array_b, array_c_sw);
        }
        else
        {
            mmult_golden_quad(array_a, array_b, array_c_sw);
        }

        //end_time = getTimestamp();

        //execution_time = (end_time - start_time) / (1000);
        //std::cout << "CPU " << " Total execution time = " << execution_time << " msec" << std::endl;

        if (result_check(array_c, array_c_sw))
            return 1;
        
        */

    
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
        cl::Program program(context, {device}, bins, nullptr, &err);
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
    /*
    cl_mem_ext_ptr_t host_buffer_ext;
    host_buffer_ext.flags = XCL_MEM_EXT_HOST_ONLY;
    host_buffer_ext.obj = nullptr;
    host_buffer_ext.param = 0;
    */

    OCL_CHECK(err, cl::Buffer buffer_ternary(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, sizeof(ap_uint<2>), NULL, &err));
    OCL_CHECK(err, cl::Buffer buffer_array_a(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, SN * SM * sizeof(DTYPE), NULL, &err));
    OCL_CHECK(err, cl::Buffer buffer_array_b(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, SM * SP * sizeof(DTYPE), NULL, &err));
    OCL_CHECK(err, cl::Buffer buffer_SM(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, sizeof(int), NULL, &err));
    OCL_CHECK(err, cl::Buffer buffer_SP(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, sizeof(int), NULL, &err));
    OCL_CHECK(err, cl::Buffer buffer_begin(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, sizeof(int), NULL, &err));
    OCL_CHECK(err, cl::Buffer buffer_end(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, sizeof(int), NULL, &err));
    OCL_CHECK(err, cl::Buffer buffer_array_c(context, CL_MEM_WRITE_ONLY | CL_MEM_EXT_PTR_XILINX, SN * SP * sizeof(DTYPE_OUT), NULL, &err));

    // Set the kernal argument
    int narg = 0;
    krnl.setArg(narg++, buffer_ternary);
    krnl.setArg(narg++, buffer_array_a);
    krnl.setArg(narg++, buffer_array_b);
    krnl.setArg(narg++, buffer_array_c);
    krnl.setArg(narg++, buffer_SM);
    krnl.setArg(narg++, buffer_SP);
    krnl.setArg(narg++, buffer_begin);
    krnl.setArg(narg++, buffer_end);
    
    /*
    ap_uint<2> ternary;
    DTYPE* array_a;
    DTYPE* array_b;
    DTYPE_OUT* array_c;
    int M;
    int P;
    int begin;
    int end;
    */

    OCL_CHECK(err, ternary = (ap_uint<2>*)q.enqueueMapBuffer(buffer_ternary, CL_TRUE, CL_MAP_WRITE, 0, sizeof(ap_uint<2>), nullptr, nullptr, &err));
    OCL_CHECK(err, array_a = (DTYPE*)q.enqueueMapBuffer(buffer_array_a, CL_TRUE, CL_MAP_WRITE, 0, SN * SM * sizeof(DTYPE), nullptr, nullptr, &err));
    OCL_CHECK(err, array_b = (DTYPE*)q.enqueueMapBuffer(buffer_array_b, CL_TRUE, CL_MAP_WRITE, 0, SM * SP * sizeof(DTYPE), nullptr, nullptr, &err));
    OCL_CHECK(err, M = (int*)q.enqueueMapBuffer(buffer_SM, CL_TRUE, CL_MAP_WRITE, 0, sizeof(int), nullptr, nullptr, &err));
    OCL_CHECK(err, P = (int*)q.enqueueMapBuffer(buffer_SP, CL_TRUE, CL_MAP_WRITE, 0, sizeof(int), nullptr, nullptr, &err));
    OCL_CHECK(err, begin = (int*)q.enqueueMapBuffer(buffer_begin, CL_TRUE, CL_MAP_WRITE, 0, sizeof(int), nullptr, nullptr, &err));
    OCL_CHECK(err, end = (int*)q.enqueueMapBuffer(buffer_end, CL_TRUE, CL_MAP_WRITE, 0, sizeof(int), nullptr, nullptr, &err));
    OCL_CHECK(err, array_c = (DTYPE_OUT*)q.enqueueMapBuffer(buffer_array_c, CL_TRUE, CL_MAP_READ, 0, SN * SP * sizeof(DTYPE_OUT), nullptr, nullptr, &err));

    //q.finish();

    // Date will be migrate to the kernal space
    q.enqueueMigrateMemObjects({buffer_ternary, buffer_array_a, buffer_array_b, buffer_SM, buffer_SP, buffer_begin, buffer_end}, 0);
    
    // Lauch the kernal
    q.enqueueTask(krnl);
    
    // To view the results, this call will transfer the data from FPGA to the host
    q.enqueueMigrateMemObjects({buffer_array_c}, CL_MIGRATE_MEM_OBJECT_HOST);
    
    q.finish();

    // Compare the results of the Device to the simulation
    std::cout << "Start to mmult_golden " << std::endl;
	
    //bool match = true;
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
    /*
    bool match = true;
    for (int i = 0; i < DATA_NUM; i++) {
        if (out[i] != (in_a[i] + in_b[i])) {
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i << " CPU result = " << in_a[i] + in_b[i] << " Device result = " << out[i]
                      << std::endl;
            match = false;
            break;
        }
    }
    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
    */

    q.enqueueUnmapMemObject(buffer_ternary, ternary);
    q.enqueueUnmapMemObject(buffer_array_a, array_a);
    q.enqueueUnmapMemObject(buffer_array_b, array_b);
    q.enqueueUnmapMemObject(buffer_SM, M);
    q.enqueueUnmapMemObject(buffer_SP, P);
    q.enqueueUnmapMemObject(buffer_begin, begin);
    q.enqueueUnmapMemObject(buffer_end, end);
    q.enqueueUnmapMemObject(buffer_array_c, array_c);
    q.finish();

}

/*
int main(int argc, const char* argv[]) {

   
    


    // Platform related operations
    std::vector<cl::Device> devices = xcl::get_xil_devices();
    cl::Device device = devices[0];

    // Creating Context and Command Queue for selected Device
    cl::Context context(device);
    cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE);
    std::string devName = device.getInfo<CL_DEVICE_NAME>();
    printf("INFO: Found Device=%s\n", devName.c_str());

    cl::Program::Binaries xclBins = xcl::import_binary_file(xclbin_path);
    devices.resize(1);
    cl::Program program(context, devices, xclBins);
    cl::Kernel kerlnalMatrixmult(program, "MatrixMult_kernel");
    std::cout << "INFO: Kernel has been created" << std::endl;

    // Initialization of host buffers
    mmult_test(ternary, A, B, C_sw, C, myFile);

    // DDR Settings


    // Create device buffer and map device buf to host buf






}
*/
