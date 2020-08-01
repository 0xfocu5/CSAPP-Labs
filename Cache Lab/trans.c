#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if (N == 32 && M == 32)
    {
        for (int i = 0; i < N; i += 8)
        {
            for (int j = 0; j < M; j += 8)
            {
                for (int k = i; k < i + 8; ++k)
                {
                    int tempValue0 = A[k][j];
                    int tempValue1 = A[k][j+1];
                    int tempValue2 = A[k][j+2];
                    int tempValue3 = A[k][j+3];
                    int tempValue4 = A[k][j+4];
                    int tempValue5 = A[k][j+5];
                    int tempValue6 = A[k][j+6];
                    int tempValue7 = A[k][j+7];

                    B[j][k] = tempValue0;
                    B[j+1][k] = tempValue1;
                    B[j+2][k] = tempValue2;
                    B[j+3][k] = tempValue3;
                    B[j+4][k] = tempValue4;
                    B[j+5][k] = tempValue5;
                    B[j+6][k] = tempValue6;
                    B[j+7][k] = tempValue7;
                }
            }
        }
    }
    else if (N == 64 && M == 64)
    {
        for (int i = 0; i < N; i += 8)
        {
            for (int j = 0; j < M; j += 8)
            {
                for (int k = i; k < i + 4; ++k)
                {
                // 1 2
                    int tempValue0 = A[k][j];
                    int tempValue1 = A[k][j+1];
                    int tempValue2 = A[k][j+2];
                    int tempValue3 = A[k][j+3];
                    int tempValue4 = A[k][j+4];
                    int tempValue5 = A[k][j+5];
                    int tempValue6 = A[k][j+6];
                    int tempValue7 = A[k][j+7];

                    B[j][k] = tempValue0;
                    B[j+1][k] = tempValue1;
                    B[j+2][k] = tempValue2;
                    B[j+3][k] = tempValue3;
                    B[j][k+4] = tempValue7;
                    B[j+1][k+4] = tempValue6;
                    B[j+2][k+4] = tempValue5;
                    B[j+3][k+4] = tempValue4;
                }

                for (int l = 0; l < 4; ++l)
                {
                    int tempValue0 = A[i+4][j+3-l];
                    int tempValue1 = A[i+5][j+3-l];
                    int tempValue2 = A[i+6][j+3-l];
                    int tempValue3 = A[i+7][j+3-l];
                    int tempValue4 = A[i+4][j+4+l];
                    int tempValue5 = A[i+5][j+4+l];
                    int tempValue6 = A[i+6][j+4+l];
                    int tempValue7 = A[i+7][j+4+l];

                    B[j+4+l][i] = B[j+3-l][i+4];
                    B[j+4+l][i+1] = B[j+3-l][i+5];
                    B[j+4+l][i+2] = B[j+3-l][i+6];
                    B[j+4+l][i+3] = B[j+3-l][i+7];

                    B[j+3-l][i+4] = tempValue0;
                    B[j+3-l][i+5] = tempValue1;
                    B[j+3-l][i+6] = tempValue2;
                    B[j+3-l][i+7] = tempValue3;
                    B[j+4+l][i+4] = tempValue4;
                    B[j+4+l][i+5] = tempValue5;
                    B[j+4+l][i+6] = tempValue6;
                    B[j+4+l][i+7] = tempValue7;
                } 
            }
        }
    }
    else
    {
        for (int i = 0; i < N; i += 16)
        {
            for (int j = 0; j < M; j += 16)
            {
                for (int k = i; k < i + 16 && k < N; ++k)
                {
                    int temp_position = -1;
                    int temp_value = 0;
                    int l;
                    for (l = j; l < j + 16 && l < M; ++l)
                    {
                        if (l == k)
                        {
                            temp_position = k;
                            temp_value = A[k][k];
                        }
                        else
                        {
                            B[l][k] = A[k][l];
                        }
                    }
                    if (temp_position != -1)
                    {
                        B[temp_position][temp_position] = temp_value;
                    }
                }
            }
        }
    }
}
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}