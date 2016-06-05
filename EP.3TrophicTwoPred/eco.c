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

	cpgscr(20, ((float)rand()/RAND_MAX), ((float)rand()/RAND_MAX), ((float)rand()/RAND_MAX));
	cpgscr(21, ((float)rand()/RAND_MAX), ((float)rand()/RAND_MAX), ((float)rand()/RAND_MAX));
	cpgscr(22, ((float)rand()/RAND_MAX), ((float)rand()/RAND_MAX), ((float)rand()/RAND_MAX));
	cpgscr(23, ((float)rand()/RAND_MAX), ((float)rand()/RAND_MAX), ((float)rand()/RAND_MAX));
	cpgscr(24, ((float)rand()/RAND_MAX), ((float)rand()/RAND_MAX), ((float)rand()/RAND_MAX));
	cpgscr(25, ((float)rand()/RAND_MAX), ((float)rand()/RAND_MAX), ((float)rand()/RAND_MAX));
	cpgscr(26, ((float)rand()/RAND_MAX), ((float)rand()/RAND_MAX), ((float)rand()/RAND_MAX));
	cpgscr(27, ((float)rand()/RAND_MAX), ((float)rand()/RAND_MAX), ((float)rand()/RAND_MAX));
	cpgscr(28, ((float)rand()/RAND_MAX), ((float)rand()/RAND_MAX), ((float)rand()/RAND_MAX));
	cpgscr(29, ((float)rand()/RAND_MAX), ((float)rand()/RAND_MAX), ((float)rand()/RAND_MAX));
	cpgscr(30, ((float)rand()/RAND_MAX), ((float)rand()/RAND_MAX), ((float)rand()/RAND_MAX));
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
	int iteration = 1;
	int flagUpdate;

	// rates
	float aBirthRate = RATE_A_BIRTH;
	float predationRates[NUMB_TROPHIC-1] = RATE_PRED;
	float deathRates[NUMB_TROPHIC] = RATE_DEATH;

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

	// data collection archive
	float** archiveSpeciesDensity = allocateArray2D(NUMB_TROPHIC,TIME_MAX);
	for (i=0; i<NUMB_TROPHIC; i++)
		for (j=0; j<TIME_MAX; j++)
			archiveSpeciesDensity[i][j] = 0.0;
	float** archiveEquilibriumPopulation = allocateArray2D(ITERATION_MAX,NUMB_TROPHIC);
	float** archiveEquilibriumA = allocateArray2D(21,21);
	float** archiveEquilibriumB = allocateArray2D(21,21);



	//==========================================================================
	//--------------------------ACTUAL CODE-------------------------------------
	//==========================================================================

	// big loop for repeating the same setup multiple times
	while(iteration < ITERATION_MAX) {

		//==========================================================================
		//--------------------------PGPLOT INITIALIZATION---------------------------
		//==========================================================================

		// environment initialization
		randomizeArray2DInt(simEnv, ENV_SIZE_X, ENV_SIZE_Y, 10, 10, 10);
		//randomizeArray2DInt(simEnv, ENV_SIZE_X, ENV_SIZE_Y, rand()%100, rand()%100);
		updatePopDens(simEnv, popCount, popDens);
		for (i=0; i<NUMB_TROPHIC; i++)
			popDensOld[i] = popDens[i];
		cpgmove(popDens[0], popDens[1]);

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
		cpgswin(0, TIME_MAX, 0, 1);
		cpgbox("ABCINTS", 0.0, 0, "ABCINTS", 0.0, 0);
		cpglab("Time", "Population Density", "");

		// Phase Portrait Plot
		cpgpanl(3,1);
		cpgsvp(0.08, 0.92, 0.08, 0.92);
		cpgswin(0, 1, 0, 1);
		cpgbox("ABCINTS", 0.0, 0, "ABCINTS", 0.0, 0);
		cpglab("", "", "Phase Portrait");
		cpgsci(11);
		cpglab("Population Density SpC", "", "");
		cpgsci(12);
		cpglab("", "Population Density SpB", "");

		// initial delay to visualize starting matrix
		//for (t=0; t<50000000; t++){}
		//predationRates[1] = (((float)iteration/10.0)+10.9);


		// edit rates between each iteration
		/*if ((iteration%20)==1) {
			aBirthRate += 0.5;
			predationRates[0] = 1.0;
		}
		predationRates[0] += 0.5;*/
		//aBirthRate += PARAM_INCREMENT;
		predationRates[0] += PARAM_INCREMENT;


		tGlobal = 1;
		while(tGlobal < TIME_MAX){

			// run simulation for a full Monte Carlo timestep (ENV_SIZE_X*ENV_SIZE_Y)
			for (t=0; t<ENV_SIZE_TOTAL; t++){
				ecoRun(simEnv, simEnvAge, simLocal, aBirthRate, predationRates, deathRates);
			}
			incrementAge(simEnvAge);


			// calculate population densities
			updatePopDens(simEnv, popCount, popDens);
			// archive species density time series averaged over multiple iterations
			for (i=0; i<NUMB_TROPHIC; i++){
				archiveSpeciesDensity[i][tGlobal] = (archiveSpeciesDensity[i][tGlobal]*(iteration-1))/(iteration);
				archiveSpeciesDensity[i][tGlobal] += (popDens[i]/iteration);
			}
			// archive species density averaged over the last 200 timesteps
			if (tGlobal >300) {
				for (i=0; i<NUMB_TROPHIC; i++){
					archiveEquilibriumPopulation[iteration][i] = (archiveEquilibriumPopulation[iteration][i]*(tGlobal-301))/(tGlobal-300);
					archiveEquilibriumPopulation[iteration][i] += (popDens[i]/(tGlobal-300));
				}
			}

			// plot stuffs
			if ((tGlobal%1) == 0){
				// PLOT population densities
				cpgpanl(2,1);
				cpgsvp(0.08, 0.92, 0.08, 0.92);
				cpgswin(0, TIME_MAX, 0, 1);
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
				cpgsls(1); cpgsci(1);
				//cpgsls(1); cpgsci(iteration+20);				// line style and color
				cpgmove(popDensOld[2], popDensOld[1]);
				cpgdraw(popDens[2], popDens[1]);
				
				cpgsci(1);
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

		printf("Done iteration %i, BirthRate %f, PredationRate %f,%f", iteration, aBirthRate, predationRates[0], predationRates[1]);
		printf("     SpDens: %f, %f, %f \n", archiveEquilibriumPopulation[iteration][0], archiveEquilibriumPopulation[iteration][1], archiveEquilibriumPopulation[iteration][2]);

		k=((iteration-1)%20);
		//archiveEquilibriumA[k][((iteration-1)-k)/20] = archiveEquilibriumPopulation[iteration][0];
		//archiveEquilibriumB[k][((iteration-1)-k)/20] = archiveEquilibriumPopulation[iteration][1];

		iteration++;
		cpgpanl(1,1);
		cpgeras();
		cpgpanl(2,1);
		cpgeras();
		cpgpanl(3,1);
		cpgeras();
	}

	// export results
	writeCSV("BirthRateVariable.csv", archiveEquilibriumPopulation, ITERATION_MAX, NUMB_TROPHIC);
	//writeCSV("SpeciesA.csv", archiveEquilibriumA, 21, 21);
	//writeCSV("SpeciesB.csv", archiveEquilibriumB, 21, 21);

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
		cpgswin(0, TIME_MAX, 0, 1);
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

	/*// PLOT archived population densities
	for (tGlobal=0; tGlobal<TIME_MAX; tGlobal++) {
		cpgpanl(2,1);
		cpgsvp(0.08, 0.92, 0.08, 0.92);
		cpgswin(0, TIME_MAX, 0, 1);
		for (i=0; i<NUMB_TROPHIC; i++){
			cpgsls(1); cpgsci(i+11);				// line style and color
			cpgmove((tGlobal-1), archiveSpeciesDensity[i][tGlobal-1]);
			cpgdraw(tGlobal, archiveSpeciesDensity[i][tGlobal]);
		}
		// PLOT archived phase portrait
		cpgpanl(3,1);
		cpgsvp(0.08, 0.92, 0.08, 0.92);
		cpgswin(0, 1, 0, 1);
		cpgsls(1); cpgsci(1);						// line style and color
		cpgmove(archiveSpeciesDensity[0][tGlobal-1], archiveSpeciesDensity[1][tGlobal-1]);
		cpgdraw(archiveSpeciesDensity[0][tGlobal], archiveSpeciesDensity[1][tGlobal]);
	}*/
	// loop forever upon completion
	while(1) {
	}
}