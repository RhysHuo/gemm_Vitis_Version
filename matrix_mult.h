#ifndef __MATRIX_MULT_H__
#define __MATRIX_MULT_H__

#include "ap_int.h"
#include <vector>

// 64 * 16(values in 32-bit word (ternary)) = 1024
//1536 and 16 in a word so 96 words of 32-bit
//1536 and 4 in a word so 384 words of 8-bit

#define DTYPE_LENGTH 32 //8//32 //8 //32//32 //8//32//8
#define MAX_N    2048 //64
#define MAX_M    2048 //384 //1536 //384 //1536 // 384 //48 //768 //96 //384 //96 //384 //96 //384// 96
#define MAX_P    2048 //512 //64//1//64//1

//#define SN    64 //2048 //64
//#define SM    384 //1536 //384 //1536 //384 //1536 // 384 //48 //768 //96 //384 //96 //384 //96 //384// 96
//#define SP    512//64//1//64//1


//48 for WL 64
//96 for WL 32
// 384 for WL 8
// 768 for WL 4

#define A_HEIGHT   MAX_N
#define A_WIDTH    MAX_M

#define B_HEIGHT   MAX_M
#define B_WIDTH    MAX_P

#define C_HEIGHT   MAX_N
#define C_WIDTH    MAX_P

typedef ap_int<DTYPE_LENGTH> DTYPE;
typedef ap_int<16> DTYPE_OUT;

#define A_HEIGHT_BLOCK  1// 4096 //(512/4)
#define B_WIDTH_BLOCK 32 //32 //64 //64 //128 // 64 //64 //64 //8//8// //16//32//1//32//1//32//1// 1//32//(128/4)

/*
 A_HEIGHT_BLOCK  is for software part data partitioning due to the limitation in 
 the Xilinx kernel sds_alloc so A_HEIGHT_BLOCK should be A_HEIGHT divided by 
 the number of considered blocks
*/
#define C_HEIGHT_BLOCK  A_HEIGHT_BLOCK 


#define C_WIDTH_BLOCK   B_WIDTH_BLOCK //B_WIDTH_BLOCK should be less than P

typedef unsigned long u32;

typedef std::vector<int> vi;



void mmult_accel(ap_uint<2> ternary, int M, DTYPE* A, DTYPE B[B_HEIGHT][B_WIDTH_BLOCK], DTYPE_OUT* C);
//#pragma SDS data access_pattern(A:SEQUENTIAL)


//#pragma SDS data zero_copy(B[0:B_HEIGHT*B_WIDTH], C[0:A_HEIGHT*B_WIDTH])
//#pragma SDS data copy(A[0:A_HEIGHT*A_WIDTH])
//#pragma SDS data access_pattern(A:RANDOM)
//#pragma SDS data zero_copy(A[0:A_HEIGHT*A_WIDTH],B[0:B_HEIGHT*B_WIDTH], C[0:A_HEIGHT*B_WIDTH])
//#pragma SDS data sys_port(A:AFI,B:AFI,C:AFI)
//#pragma SDS data access_pattern(A:ACP;B:ACP;C:ACP)
//#pragma SDS data zero_copy(A[0:line_count*A_WIDTH],B[0:B_HEIGHT*B_WIDTH], C[0:A_HEIGHT*B_WIDTH])
void mmult_top(ap_uint<2> ternary,int N, int M, int P, DTYPE* A, DTYPE* B, DTYPE_OUT* C);
//int mmult_top2(int A[A_WIDTH*A_HEIGHT], int B[B_HEIGHT*B_WIDTH], int C[C_HEIGHT*C_WIDTH],int line_count);
//int mmult_top3(int A[A_WIDTH*A_HEIGHT], int B[B_HEIGHT*B_WIDTH], int C[C_HEIGHT*C_WIDTH],int line_count);
//int mmult_top4(int A[A_WIDTH*A_HEIGHT], int B[B_HEIGHT*B_WIDTH], int C[C_HEIGHT*C_WIDTH],int line_count);

#endif //__MATRIX_MULT_H__



