#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "eco.h"

void loadPoint(int** simEnv, int* simLocal, const int x, const int y){
	// load information on local environment

	simLocal[0] = simEnv[x][y];
	if (y==0)
		simLocal[1] = -1;				// empty
	else
		simLocal[1] = simEnv[x][y-1];
	if (x==(ENV_SIZE_X-1))
		simLocal[2] = -1;				// empty
	else
		simLocal[2] = simEnv[x+1][y];
	if (y==(ENV_SIZE_Y-1))
		simLocal[3] = -1;				// empty
	else
		simLocal[3] = simEnv[x][y+1];
	if (x==0)
		simLocal[4] = -1;				// empty
	else
		simLocal[4] = simEnv[x-1][y];
}

void updateEnv(int** simEnv, int* simLocal, const int x, const int y){
	// load back local environment into simulation

	simEnv[x][y] = simLocal[0];
	if (y != 0)
		simEnv[x][y-1] = simLocal[1];
	if (x != (ENV_SIZE_X-1))
		simEnv[x+1][y] = simLocal[2];
	if (y != (ENV_SIZE_Y-1))
		simEnv[x][y+1] = simLocal[3];
	if (x != 0)
		simEnv[x-1][y] = simLocal[4];
}

void updatePopDens(int** simEnv, int* popCount, float* popDens){
	// calculate population densities
	int i, j, k;

	for (i=0; i<NUMB_TROPHIC; i++)
		popCount[i] = 0;
	for (i=0; i<ENV_SIZE_X; i++){
		for (j=0; j<ENV_SIZE_Y; j++){
			for (k=0; k<NUMB_TROPHIC; k++)
				if (simEnv[i][j] == (k+1))
					popCount[k]++;
		}
	}
	for (i=0; i<NUMB_TROPHIC; i++)
		popDens[i] = popCount[i]/(float)ENV_SIZE_TOTAL;
}

void updateAgeStructure(int** simEnv, int** simEnvAge, float* ageStructure, int species){
	// check age structure of target trophic level
	int i;
	int x, y;

	for (i=0; i<ENV_SIZE_TOTAL; i++)
		ageStructure[i] = 0;
	i=0;
	for (x=0; x<ENV_SIZE_X; x++)
		for (y=0; y<ENV_SIZE_Y; y++)
			if (simEnv[x][y] == species) {
				ageStructure[i] = (float)simEnvAge[x][y];
				i++;
			}
}

void incrementAge(int** simEnvAge){
	int x, y;

	for (x=0; x<ENV_SIZE_X; x++)
		for (y=0; y<ENV_SIZE_Y; y++)
			simEnvAge[x][y] = simEnvAge[x][y]+1;
}

void ecoRun(int** simEnv, int** simEnvAge, int simLocal[], float predationRates[], float deathRates[]){
	int i = 0;
	int x, y, loc2;
	float rate;

	// randomly select a reaction to run
	int state = (rand()%(NUMB_TROPHIC+NUMB_TROPHIC+1));

	// Trophic 1 Offspring Generation
	if (state == 0){
		for (rate = RATE_A_BIRTH; rate > 0; rate--) {
			x = (rand() % ENV_SIZE_X);
			y = (rand() % ENV_SIZE_Y);
			loadPoint(simEnv, simLocal, x, y);
			loc2 = (rand()%4)+1;

			if (simLocal[0] == 0 && simLocal[loc2] == 1 && (rand()/(float)RAND_MAX) < rate) {
				simEnv[x][y] = 1;
				simEnvAge[x][y] = 0;
			}
		}
		return;
	}

	// Specialist Predation (state=prey trophic level)
	else if (state >= 1   &&   state < NUMB_TROPHIC) {
		for (rate = predationRates[state-1]; rate > 0; rate--){
			x = (rand() % ENV_SIZE_X);
			y = (rand() % ENV_SIZE_Y);
			loadPoint(simEnv, simLocal, x, y);
			loc2 = (rand()%4)+1;

			if (simLocal[0] == state && simLocal[loc2] == (state+1) && (rand()/(float)RAND_MAX) < rate) {
				simEnv[x][y] = (state+1);
				simEnvAge[x][y] = 0;
			}
		}
		return;
	}

	// Predator Death
	else if (state >= NUMB_TROPHIC   &&   state <= (NUMB_TROPHIC+NUMB_TROPHIC)){
		state -= NUMB_TROPHIC;
		for (rate = deathRates[state]; rate > 0; rate--) {
			x = (rand() % ENV_SIZE_X);
			y = (rand() % ENV_SIZE_Y);
			loadPoint(simEnv, simLocal, x, y);
			loc2 = (rand()%4)+1;

			if (simLocal[0] == (state+1) && (rand()/(float)RAND_MAX) < rate) {
				simEnv[x][y] = 0;
				simEnvAge[x][y] = 0;
			}
		}
		return;
	}	
	return;
}


/*else if (state >= NUMB_TROPHIC   &&   state <= (NUMB_TROPHIC+NUMB_TROPHIC)){
		state -= NUMB_TROPHIC;
		for (rate = RATE_B_DEATH; rate > 0; rate--) {
			x = (rand() % ENV_SIZE_X);
			y = (rand() % ENV_SIZE_Y);
			loadPoint(simEnv, simLocal, x, y);
			loc2 = (rand()%4)+1;

			if (simLocal[0] == (state+1) && (rand()/(float)RAND_MAX) < rate) {
				simEnv[x][y] = 0;
				simEnvAge[x][y] = 0;
			}
		}
		return;
	}	
	return;*/