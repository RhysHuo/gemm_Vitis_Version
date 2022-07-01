rm host.cpp
git clone https://github.com/RhysHuo/gemm_Vitis_Version.git
cd gemm_Vitis_Version
cp host.cpp ..
cd ..
rm -rf gemm_Vitis_Version
g++ -c -I${XILINX_XRT}/include -I/mnt/scratch/rhyhuo/HLS_arbitrary_Precision_Types/include -g -o host.o host.cpp
g++ -g -L${XILINX_XRT}/lib/ -I/mnt/scratch/rhyhuo/HLS_arbitrary_Precision_Types/include -o host host.o -lOpenCL -lpthread -lrt -lstdc++
