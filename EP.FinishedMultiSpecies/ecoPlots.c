#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "eco.h"

void plotSimulation(int** simEnv, float* plotImg, float plotMinBound, float plotMaxBound, const float TR[6]){
	int i, j;

	cpgpanl(1,1);
	cpgswin(0, ENV_SIZE_X, 0, ENV_SIZE_Y);
	cpgsvp(0.01, 0.99, 0.01, 0.99);
	for (i=0; i<ENV_SIZE_X; i++)
		for (j=0; j<ENV_SIZE_Y; j++)
			plotImg[i*ENV_SIZE_Y+j] = (float)(simEnv[i][j]);
	cpgimag(plotImg, ENV_SIZE_Y, ENV_SIZE_X, 1, ENV_SIZE_Y, 1, ENV_SIZE_X, plotMinBound, plotMaxBound, TR);
}