git clone https://github.com/RhysHuo/gemm_Vitis_Version.git
cd gemm_Vitis_Version
cp host.cpp ..
cd ..
rm -rf gemm_Vitis_Version
g++ -g -std=c++14 -I$XILINX_XRT/include -L${XILINX_XRT}/lib/ -I/mnt/scratch/rhyhuo/HLS_arbitrary_Precision_Types/include -o host.exe host.cpp -lOpenCL -pthread -lrt -lstdc++
