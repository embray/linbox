/*
 * kernel_partial_sp.cl
 *
 *  Created on: Jul 5, 2011
 *      Author: Matthew Wezowicz
 */

#define BLOCK_SIZE 16

__kernel void matrixMuladdKernelModular16SP(__global float* D, float alpha, __global float* A, __global float* B,
		float beta, __global float* C, const int widthA, const int widthB, const float mod){
	//Get Workgroup ID
	int bx = get_group_id(0);
	int by = get_group_id(1);

	//Get Local ID
	int tx = get_local_id(0);
	int ty = get_local_id(1);

	//Range of indecies for sub-matrix of A
	int aBegin = widthA * BLOCK_SIZE * by;
	int aEnd = aBegin + widthA - 1;
	int aStep = BLOCK_SIZE;

	//Range of indecies for sub-matrix of B
	int bBegin = BLOCK_SIZE * bx;
	int bStep = BLOCK_SIZE * widthB;

	//Local storage of sub-matrices of A and B
	__local float As[BLOCK_SIZE][BLOCK_SIZE];
	__local float Bs[BLOCK_SIZE][BLOCK_SIZE];

	//Temporary storage for result
	float Dsub = 0;

	//Setup count for modulus every 32 iterations.
	int m = 0;

	//Loop over all the sub-matrices of A and B required to compute
	//the result sub-matrix
	for(int a = aBegin, b = bBegin; a < aEnd; a += aStep, b += bStep){
		//Load the matrices from global memory to local memory
		//Each thread loads one element of each sub-matrix
		As[ty][tx] = A[a + widthA * ty + tx];
		Bs[ty][tx] = B[b + widthB * ty + tx];

		//Synchronize threads
		barrier(CLK_LOCAL_MEM_FENCE);

		//Multiply the two sub-matrices together
		Dsub += As[ty][0] * Bs[0][tx];
		Dsub += As[ty][1] * Bs[1][tx];
		Dsub += As[ty][2] * Bs[2][tx];
		Dsub += As[ty][3] * Bs[3][tx];
		Dsub += As[ty][4] * Bs[4][tx];
		Dsub += As[ty][5] * Bs[5][tx];
		Dsub += As[ty][6] * Bs[6][tx];
		Dsub += As[ty][7] * Bs[7][tx];
		Dsub += As[ty][8] * Bs[8][tx];
		Dsub += As[ty][9] * Bs[9][tx];
		Dsub += As[ty][10] * Bs[10][tx];
		Dsub += As[ty][11] * Bs[11][tx];
		Dsub += As[ty][12] * Bs[12][tx];
		Dsub += As[ty][13] * Bs[13][tx];
		Dsub += As[ty][14] * Bs[14][tx];
		Dsub += As[ty][15] * Bs[15][tx];
		Dsub = fmod(Dsub, mod);

		//Synchronize threads
		barrier(CLK_LOCAL_MEM_FENCE);
	}
	//Calculates the offset in the result matrix
	int d = widthB * BLOCK_SIZE * by + BLOCK_SIZE * bx;

	//Scale Dsub by alpha
	Dsub = alpha * Dsub;
	Dsub = fmod(Dsub, mod);
	if(Dsub < 0){
		Dsub = mod + Dsub;
	}

	//Scalse Csub by beta
	float Csub = C[d + ty * widthB + tx];
	Csub = beta * Csub;
	Csub = fmod(Csub, mod);
	if(Csub < 0){
		Csub = mod + Csub;
	}

	//Add Dsub and Dsub
	Dsub = Dsub + Csub;
	Dsub = fmod(Dsub, mod);

	//Add the sum to the appropriate spot
	D[d + ty * widthB + tx] = Dsub;
}
