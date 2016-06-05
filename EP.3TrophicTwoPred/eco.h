#define ENV_SIZE_X 256	
#define ENV_SIZE_Y 256
#define ENV_SIZE_TOTAL (ENV_SIZE_X*ENV_SIZE_Y)
#define NUMB_TROPHIC 3
#define INIT_DENSITY 10 //** MUST BE LARGER THAN NUMB_TROPHIC, ELSE UPPER SP WILL HAVE ZERO STARTING DENSITY
#define SIZE_LOCAL 5

#define ERR_INVALID_INPUT 1
#define ERR_MALLOC_FAIL 2
#define ERR_FILE_OPEN 3
#define ERR_PGPLOT 4

#define RATE_A_BIRTH 5.0		//1.0		//10.0	//20.0			//4.0
//#define RATE_PRED {8.0, 40.0} 		//100.2		//10.2	//20.0			//20.0
#define RATE_PRED {9.8, 10.0} 		//100.2		//10.2	//20.0			//20.0
#define RATE_DEATH {0.0, 1.0, 1.0}

#define NUMB_REPEAT 100
#define TIME_MAX 500
#define ITERATION_MAX 50

#define PARAM_INCREMENT 0.2
#define MAX_LINE 99999
//#define RATE_B_DEATH 1.0		//0.1		//0.1	//0.1			//0.1


// generic array allocations (square lattice)
float* allocateArray1D(const int dim1);
int* allocateArray1DInt(const int dim1);
float** allocateArray2D(const int dim1, const int dim2);
int** allocateArray2DInt(const int dim1, const int dim2);
float*** allocateArray3D(const int dim1, const int dim2, const int dim3);
int*** allocateArray3dInt(const int dim1, const int dim2, const int dim3);

// generic array operations
void randomizeArray2DInt(int** array, int dim1, int dim2, int nRand1, int nRand2, int nRand3);
void printArray1D(float* array, int dim1);
void printArray1DInt(int* array, int dim1);
void printArray2DInt(int** array, int dim1, int dim2);
void copyArray2DInt(int** arrayOld, int** arrayNew, int dim1, int dim2);

// ecology calculation
void ecoRun(int** simEnv, int** simEnvAge, int simLocal[], float aBirthRate, float predationRates[], float deathRates[]);
void loadPoint(int** simEnv, int* simLocal, const int x, const int y);
void updateEnv(int** simEnv, int* simLocal, const int x, const int y);
void updatePopDens(int** simEnv, int* popCount, float* popDens);
void updateAge(int** simEnv, int** simEnvAge, float* ageStructure);
void incrementAge(int** simEnvAge);

// plots
void plotSimulation(int** simEnv, float* plotImg, float plotMinBound, float plotMaxBound, const float TR[6]);

// miscellaneous
void errorCase(const int errorCode);
int writeCSV(const char filename[], float** arrayIn, const int lengthRow, const int lengthCol);