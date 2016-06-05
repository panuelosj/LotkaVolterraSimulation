#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "eco.h"

/*void randomizeArray2DInt(int** array, int dim1, int dim2, int nRand){
	int i, j, k;

	for (i=0; i<dim1; i++){
		for (j=0; j<dim2; j++){
			k = (rand() % (INIT_DENSITY+1));
			if (k>NUMB_TROPHIC)
				k = 0;
			array[i][j] = k;
		}
	}
}*/

void randomizeArray2DInt(int** array, int dim1, int dim2, int nRand1, int nRand2, int nRand3){
	int i, j, k;

	for (i=0; i<dim1; i++){
		for (j=0; j<dim2; j++){
			k = (rand() % 100);
			if (k < nRand1)
				array[i][j] = 1;
			else if ((k-nRand1) < nRand2)
				array[i][j] = 2;
			else if ((((k-nRand1)-nRand2)) < nRand3)
				array[i][j] = 3;
			else
				array[i][j] = 0;
		}
	}
}

void printArray1D(float* array, int dim1){
	int i;
	for (i=0; i<dim1; i++)
		printf("%g", array[i]);
	printf("\n");
}

void printArray1DInt(int* array, int dim1){
	int i;
	for (i=0; i<dim1; i++)
		printf("%i", array[i]);
	printf("\n");
}

void printArray2DInt(int** array, int dim1, int dim2){
	int i, j;

	for (i=0; i<dim1; i++){
		for (j=0; j<dim2; j++){
			printf("%i", array[i][j]);
		}
		printf("\n");
	}
}

void copyArray2DInt(int** arrayOld, int** arrayNew, int dim1, int dim2){
	int i, j;
	for (i=0; i<dim1; i++)
		for (j=0; j<dim2; j++)
			arrayNew[i][j] = arrayOld[i][j];
}

void errorCase(const int errorCode){
	system("cat nagato");
	switch(errorCode){
		case ERR_INVALID_INPUT:
			printf("Error: invalid input\n");
			exit(-1);
		case ERR_MALLOC_FAIL:
			printf("Error: out of memory\n");
			exit(-1);
		case ERR_FILE_OPEN:
			printf("Error: file cannot be opened\n");
			exit(-1);
		case ERR_PGPLOT:
			printf("Error: cannot open pgplot window\n");
			exit(-1);
	}
}

int writeCSV(const char filename[], float** arrayIn, const int lengthRow, const int lengthCol) {
	char line[MAX_LINE];
	int i, j;
	float temp;

	FILE *stream = fopen(filename, "w");

	for (i = 0; i < lengthRow; i++) {
		for (j = 0; j < lengthCol; j++) {
			if (arrayIn[i][j] > 0)		// check if input makes sense
				fprintf(stream, "%f", arrayIn[i][j]);
			fprintf(stream, "0.0,");
		}
		fprintf(stream, "\n");
	}
	fclose(stream);
	return 1;
}