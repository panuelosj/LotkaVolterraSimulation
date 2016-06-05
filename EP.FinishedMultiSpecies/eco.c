#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "cpgplot.h"
#include "eco.h"


/*==============================================================================
Reactions:

A + 0 -> A + A 		aBirth
B + A -> B + B 		abPred
    B ->     0 		bDeath
==============================================================================*/

int main()
{
	//
	printf("\n====================================================================\n");
	printf("This program is able to simulate a variety of ecological\n");
	printf("situations in a 2D lattice\n");
	printf("====================================================================\n");

	//==========================================================================
	//--------------------------SYSTEM INITIALIZATIONS--------------------------
	//==========================================================================
	
	// initialize random seed
	srand(time(NULL));

	// force print all outputs (remove stdout buffer)
	setbuf(stdout, NULL);

	// initialize pgplot window
	if (!cpgopen("/XWINDOW"))
		errorCase(ERR_PGPLOT);
	cpgpap(20.0, 0.33);						// set window size
	cpgsubp(3,1);							// subdivide window into panels

	// color indexes (R, G, B)
	cpgscr(0, 0.0, 0.0, 0.0);				// empty space, black
	cpgscr(1, 1.0, 1.0, 1.0);
	cpgscr(10, 0.0, 0.0, 0.0);				// empty space, black
	cpgscr(11, 0.5, 0.5, 0.5);				// Trophic 1, gray
	cpgscr(12, 0.5, 1.0, 1.0);				// Trophic 2, cyan
	cpgscr(13, 1.0, 0.5, 0.0);				// Trophic 3, orange
	cpgscr(14, 1.0, 0.0, 0.0);
	cpgscir(10,NUMB_TROPHIC+10);


	//==========================================================================
	//--------------------------VARIABLE INITIALIZATIONS------------------------
	//==========================================================================

	// generic variables
	int i, j, k;						// counters

	// simulation environment
	int** simEnv = allocateArray2DInt(ENV_SIZE_X, ENV_SIZE_Y);
	int** simEnvAge = allocateArray2DInt(ENV_SIZE_X, ENV_SIZE_Y);
	int* simLocal = allocateArray1DInt(5);

	// inputs
	char input;

	// current location and time
	int x,y;
	int tGlobal,t;
	int flagUpdate;

	// rates
	float predationRates[NUMB_TROPHIC-1] = RATE_PRED;
	float deathRates[NUMB_TROPHIC] = RATE_DEATH;

	//float aBirth = 0;			// A+0 -> A+A
//	float abPred = 0;			// B+A -> B+B
//	float bDeath = 0;			//   B ->   0

//	int aFlag; int abFlag; int bFlag;

	// population counts;
	int popCount[NUMB_TROPHIC];
	float popDens[NUMB_TROPHIC];
	float popDensOld[NUMB_TROPHIC];
	for (i=0; i<NUMB_TROPHIC; i++){
		popCount[i] = 0;
		popDens[i] = 0.0;
		popDensOld[i] = 1.0/(float)INIT_DENSITY;
	}
	float* ageStructure = allocateArray1D(ENV_SIZE_TOTAL);


	// pgplot variables
	float* plotImg = allocateArray1D(ENV_SIZE_TOTAL);
	//float TR[6] = {0, 1, 0, 0, 0, 1};
	float TR[6] = {0, 0, 1, ENV_SIZE_Y, -1, 0};
	float plotMinBound = 0.0;
	float plotMaxBound = (float)NUMB_TROPHIC;


	//==========================================================================
	//--------------------------ACTUAL CODE-------------------------------------
	//==========================================================================
	
	// environment initialization
	randomizeArray2DInt(simEnv, ENV_SIZE_X, ENV_SIZE_Y, NUMB_TROPHIC);

	// load initial display
	for (i=0; i<ENV_SIZE_X; i++)
		for (j=0; j<ENV_SIZE_Y; j++)
			plotImg[i*ENV_SIZE_Y+j] = (float)(simEnv[i][j]);
	cpgpanl(1,1);
	cpgswin(0, ENV_SIZE_X-1, 0, ENV_SIZE_Y-1);
	cpgsvp(0.01, 0.99, 0.01, 0.99);
	cpgimag(plotImg, ENV_SIZE_Y, ENV_SIZE_X, 1, ENV_SIZE_Y, 1, ENV_SIZE_X, plotMinBound, plotMaxBound, TR);

	// Load graph labels
	// Population Density vs Time Plot
	cpgpanl(2,1);
	cpgsvp(0.08, 0.92, 0.08, 0.92);
	cpgswin(0, ENV_SIZE_X, 0, 1);
	cpgbox("ABCINTS", 0.0, 0, "ABCINTS", 0.0, 0);
	cpglab("Time", "Population Density", "");

	// Phase Portrait Plot
	cpgpanl(3,1);
	cpgsvp(0.08, 0.92, 0.08, 0.92);
	cpgswin(0, 1, 0, 1);
	cpgbox("ABCINTS", 0.0, 0, "ABCINTS", 0.0, 0);
	cpglab("", "", "Phase Portrait");
	cpgsci(11);
	cpglab("Population Density SpA", "", "");
	cpgsci(12);
	cpglab("", "Population Density SpB", "");
	
	// initial delay to visualize starting matrix
	for (t=0; t<500000000; t++){}

	tGlobal = 1;
	while(1){
		//aFlag = 0; abFlag = 0; bFlag = 0;

		// run simulation for a full Monte Carlo timestep (ENV_SIZE_X*ENV_SIZE_Y)
		for (t=0; t<ENV_SIZE_TOTAL; t++){
			ecoRun(simEnv, simEnvAge, simLocal, predationRates, deathRates);
		}
		incrementAge(simEnvAge);

	

		// plot stuffs
		if ((tGlobal%1) == 0){

			// calculate population densities
			updatePopDens(simEnv, popCount, popDens);

			// PLOT population densities
			cpgpanl(2,1);
			cpgsvp(0.08, 0.92, 0.08, 0.92);
			cpgswin(0, ENV_SIZE_X, 0, 1);
			for (i=0; i<NUMB_TROPHIC; i++){
				cpgsls(1); cpgsci(i+11);				// line style and color
				cpgmove((tGlobal-1), popDensOld[i]);
				cpgdraw(tGlobal, popDens[i]);
			}

			//printArray2DInt(simEnvAge, ENV_SIZE_X, ENV_SIZE_Y);

			// PLOT age structure
			/*updateAgeStructure(simEnv, simEnvAge, ageStructure, 1);
			cpgpanl(3,1);
			cpgsvp(0.08, 0.92, 0.08, 0.92);
			cpgswin(0, 10, 0, (ENV_SIZE_TOTAL/10));
			cpgsls(1); cpgsci(1);						// line style and color
			cpgeras();
			cpgbox("ABCINTS", 0.0, 0, "ABCINTS", 0.0, 0);
			cpglab("Age", "Number of Individuals", "Age Structure");
			cpghist(popCount[1], ageStructure, 0, 10, 10, 1);*/

			
			// PLOT phase portrait
			cpgpanl(3,1);
			cpgsvp(0.08, 0.92, 0.08, 0.92);
			cpgswin(0, 1, 0, 1);
			cpgsls(1); cpgsci(1);						// line style and color
			cpgmove(popDensOld[0], popDensOld[1]);
			cpgdraw(popDens[0], popDens[1]);

			for (i=0; i<NUMB_TROPHIC; i++)
				popDensOld[i] = popDens[i];
		}

		// load array and display on pgplot
		if ((tGlobal%1) == 0){
			cpgpanl(1,1);
			cpgswin(0, ENV_SIZE_X, 0, ENV_SIZE_Y);
			cpgsvp(0.01, 0.99, 0.01, 0.99);
			for (i=0; i<ENV_SIZE_X; i++)
				for (j=0; j<ENV_SIZE_Y; j++)
					plotImg[i*ENV_SIZE_Y+j] = (float)(simEnv[i][j]);
			cpgimag(plotImg, ENV_SIZE_Y, ENV_SIZE_X, 1, ENV_SIZE_Y, 1, ENV_SIZE_X, plotMinBound, plotMaxBound, TR);
		}


		tGlobal++;
		//for (t=0; t<10000000; t++){}
	}
}