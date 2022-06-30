#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kernelMatrixmult.h"

typedef unsigned long u32;

const int BLOCK=B_WIDTH_BLOCK;   //BLOCK should be less than B_WIDTH_BLOCK
                                 //B_WIDTH_BLOCK 是每个block的宽度（列数）
const int STEP=1;

void mxv(ap_uint<2> ternary, int M, DTYPE *A, DTYPE_OUT* C, DTYPE B[B_HEIGHT][B_WIDTH_BLOCK])  {

    //initialize C
	for (int j = 0; j < C_WIDTH_BLOCK; j++) {
	    #pragma HLS UNROLL
	    C[j] = 0;
	}

	if(ternary==0)
	{
	    for(int k = 0; k < M; k+=1) {
	        #pragma HLS PIPELINE
	        
			 for (int j = 0; j < B_WIDTH_BLOCK; j++) {

				for(int z = 0; z < DTYPE_LENGTH; z+=8) {
	  				ap_int<8> A_val = A[k].range(z+7,z);
	  				C[j] += A_val*B[k][j];
				}
			}
		}
	}
	else if (ternary==1)
	{

 	    for(int k = 0; k < M; k+=1) {
            #pragma HLS PIPELINE

		    for (int j = 0; j < B_WIDTH_BLOCK; j++) {

 			    for(int z = 0; z < DTYPE_LENGTH; z+=2) {
                    ap_int<2> A_val = A[k].range(z+1,z);
                    ap_int<2> B_temp,C_val;
                    B_temp = B[k][j].range(z+1,z);
                    C_val = A_val*B_temp;

                    C[j] += C_val;
		        }
			}
 	    }
	}
	else
	{

 	   for(int k = 0; k < M; k+=1) {
            #pragma HLS PIPELINE

		    for (int j = 0; j < B_WIDTH_BLOCK; j++) {

  			    for(int z = 0; z < DTYPE_LENGTH; z+=4) {
                    ap_int<4> A_val = A[k].range(z+3,z);
                    ap_int<4> B_temp,C_val;
                    B_temp = B[k][j].range(z+3,z);
                    C_val = A_val*B_temp;

                    C[j] += C_val;

		        }
			}
 	    }
	}
}


void mmult_accel(ap_uint<2> ternary, int M, DTYPE* A, DTYPE B[B_HEIGHT][B_WIDTH_BLOCK], DTYPE_OUT* C)
{
	for (int p = 0; p < A_HEIGHT_BLOCK; p+=STEP) {
		mxv(ternary, M, A+p*M, C+p*C_WIDTH_BLOCK, B);
	}
	//return 0;
}


void mmult_top(ap_uint<2> ternary, int N, int M, int P, DTYPE* A, DTYPE* B, DTYPE_OUT* C)
{
	DTYPE A_accel[A_WIDTH], B_accel[B_HEIGHT][B_WIDTH_BLOCK];
	DTYPE_OUT C_accel[B_WIDTH_BLOCK];
    #pragma HLS array_partition variable=A_accel type=block factor= 16 dim=1
	#pragma HLS array_partition variable=B_accel type=block factor= 16 dim=2
	#pragma HLS array_partition variable=C_accel type=complete

	for (int B_index = 0; B_index < P/B_WIDTH_BLOCK; B_index++) {

		for (int i = 0; i < M; i++) {

		    for (int j = 0; j < B_WIDTH_BLOCK; j++) {
			    B_accel[i][j] = B[i*P+B_index*B_WIDTH_BLOCK+j];
				//std::cout << i << " " << j << std::endl;
			}
		}

		for (int A_index = 0; A_index < N; A_index++) {
            #pragma HLS loop_tripcount min=64 max=64 avg=64

            for (int j = 0; j < M; j++) {
                A_accel[j] = A[A_index*M+j];
            }

            mmult_accel(ternary, M, A_accel, B_accel, C_accel);

            for (int i = 0; i < C_HEIGHT_BLOCK; i++) {
                for (int j = 0; j < C_WIDTH_BLOCK; j++) {
                    C[(i+A_index*A_HEIGHT_BLOCK)*P+j+B_index*B_WIDTH_BLOCK] = C_accel[i*C_WIDTH_BLOCK+j];
                    //std::cout << "C is " << C[(i+A_index*A_HEIGHT_BLOCK)*C_WIDTH+j+B_index*B_WIDTH_BLOCK] << std::endl;
                }
            }
	    }
	}
}

void kernelMatrixmult1(
ap_uint<2> ternary,
DTYPE* array_a,
DTYPE* array_b,
DTYPE_OUT* array_c,
int M,
int P,
int begin,
int end)
{

    //int k;
    int line_count = end-begin;
    DTYPE *array_temp_a = array_a + begin*M;
    DTYPE *array_temp_b = array_b;
    DTYPE_OUT *array_temp_c = array_c + begin*M;
    //mmult_top1(array_temp_a, array_temp_b, array_temp_c, line_count);
    // #pragma SDS resource(1)
    mmult_top(ternary, line_count, M, P, array_temp_a, array_temp_b, array_temp_c);
}
