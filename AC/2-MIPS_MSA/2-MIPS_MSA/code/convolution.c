#include <stdio.h>
#include <stdint.h>

#define MATRIX_SIZE 129
#define FILTER_SIZE 2

// Function to perform 2x2 convolution with coefficients
void convolution(int16_t input[MATRIX_SIZE][MATRIX_SIZE], int16_t output[MATRIX_SIZE][MATRIX_SIZE]) {
    int filter[FILTER_SIZE][FILTER_SIZE] = {
        {-1, 1},
        {-1, 1}
    };
	
    for (int i = 0; i < MATRIX_SIZE - 1; i++) {
        for (int j = 0; j < MATRIX_SIZE - 1; j++) {
            output[i][j] = input[i][j] * filter[0][0] +
                           input[i][j + 1] * filter[0][1] +
                           input[i + 1][j] * filter[1][0] +
                           input[i + 1][j + 1] * filter[1][1];
        }
    }
}

int main() {
    int16_t outputMatrix[MATRIX_SIZE][MATRIX_SIZE];
    int16_t inputMatrix[MATRIX_SIZE][MATRIX_SIZE];

    // Initialize your input matrix (ej. fake matrix)
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            inputMatrix[i][j] = i * MATRIX_SIZE + j;
        }
    }

    // Perform convolution with coefficients
    convolution(inputMatrix, outputMatrix);

    // Display the result
    printf("Input Matrix:\n");
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            printf("%6d ", inputMatrix[i][j]);
        }
        printf("\n");
    }
	
    printf("\nOutput Matrix after 2x2 convolution with coefficients:\n");
    for (int i = 0; i < MATRIX_SIZE - 1; i++) {
        for (int j = 0; j < MATRIX_SIZE - 1; j++) {
            printf("%6d ", outputMatrix[i][j]);
        }
        printf("\n");
    }

    return 0;
}
