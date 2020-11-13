#include 	"stdio.h"
#include 	<math.h>
#include    <iostream>
#include 	"ipp.h"
#include <random>
#include 	"x86intrin.h"

#define		VECTOR_SIZE     1048576
float *fArray;

using namespace std;

void print_student_info() {
    printf("******************************************************\n");
    printf("group members:\n");
    printf("                Alireza Zarenejad:      810196474\n");
    printf("                Mohammad Moein Shafi:   810196492\n");
    printf("******************************************************\n\n");
}

float get_random()
{
    static std::default_random_engine e;
    static std::uniform_real_distribution<> dis(0, 10); // rage 0 - 1
    return dis(e);
}

void make_random_float_arr() {
    fArray = new float [VECTOR_SIZE];

	if(!fArray){
		printf("Memory allocation error!!\n");
	}

	for (long i = 0; i < VECTOR_SIZE; i++){
		fArray[i] = get_random();
	}
}


int main (void)
{
    print_student_info();

    make_random_float_arr();

	Ipp64u start, end;
	Ipp64u time1, time2;

    float fSTmpRes[4];
	float fSAvg;
	float fVAvg;
	float fSSD;
	float fVSD;
	float ftemp;



	// Serial implementation
	start = ippGetCpuClocks();
	fSTmpRes[0] = fSTmpRes[1] = fSTmpRes[2] = fSTmpRes[3] = 0.0;
	for (long i = 0; i < VECTOR_SIZE; i+=4)
		fSTmpRes[0] += fArray[i];
	for (long i = 0; i < VECTOR_SIZE; i+=4)
		fSTmpRes[1] += fArray[i + 1];
	for (long i = 0; i < VECTOR_SIZE; i+=4)
		fSTmpRes[2] += fArray[i + 2];
	for (long i = 0; i < VECTOR_SIZE; i+=4)
		fSTmpRes[3] += fArray[i + 3];
	fSAvg = (fSTmpRes[0] + fSTmpRes[1] + fSTmpRes[2] + fSTmpRes[3]) / VECTOR_SIZE;

    // Standard Deviation calculation
	fSTmpRes[0] = fSTmpRes[1] = fSTmpRes[2] = fSTmpRes[3] = 0.0;
	for (long i = 0; i < VECTOR_SIZE; i+=4)
    {
        ftemp = fArray[i] - fSAvg;
		fSTmpRes[0] += ftemp * ftemp;
    }
	for (long i = 0; i < VECTOR_SIZE; i+=4)
    {
        ftemp = fArray[i + 1] - fSAvg;
		fSTmpRes[1] += ftemp * ftemp;
    }
	for (long i = 0; i < VECTOR_SIZE; i+=4)
    {
        ftemp = fArray[i + 2] - fSAvg;
		fSTmpRes[2] += ftemp * ftemp;
    }
	for (long i = 0; i < VECTOR_SIZE; i+=4)
    {
        ftemp = fArray[i + 3] - fSAvg;
		fSTmpRes[3] += ftemp * ftemp;
    }
	fSSD = (fSTmpRes[0] + fSTmpRes[1] + fSTmpRes[2] + fSTmpRes[3]) / VECTOR_SIZE;
	fSSD = sqrt(fSSD);

	end   = ippGetCpuClocks();
	time1 = end - start;

	// Parallel implementation
	start = ippGetCpuClocks();
	__m128 vec;
	__m128 sum = _mm_set1_ps(0.0f);
	for (long i = 0; i < VECTOR_SIZE; i+=4) {
		vec = _mm_loadu_ps (&fArray[i]);
		sum = _mm_add_ps (sum, vec);
	}
	sum = _mm_hadd_ps (sum, sum);
	sum = _mm_hadd_ps (sum, sum);
	fVAvg = (_mm_cvtss_f32 (sum)) / VECTOR_SIZE;

    // Standard Deviation calculation
	sum = _mm_set1_ps(0.0f);
	__m128 tempAvg = _mm_set1_ps(fVAvg);
	__m128 tempMult = _mm_set1_ps(0.0f);
	for (long i = 0; i < VECTOR_SIZE; i+=4) {
		vec = _mm_loadu_ps (&fArray[i]);
		tempMult = _mm_sub_ps (vec, tempAvg);
		tempMult = _mm_mul_ps (tempMult, tempMult);
		sum = _mm_add_ps (sum, tempMult);
	}
	sum = _mm_hadd_ps (sum, sum);
	sum = _mm_hadd_ps (sum, sum);
	fVSD = (_mm_cvtss_f32 (sum)) / VECTOR_SIZE;
    fVSD = sqrt(fVSD);

	end   = ippGetCpuClocks();
	time2 = end - start;

	printf ("\nAverage in Serial method   = %f \nAverage in Parallel method = %f\n", fSAvg, fVAvg);

	printf ("\nStandard Deviation in Serial method   = %f \nStandard Deviation in Parallel method = %f\n", fSSD, fVSD);

	printf ("\nSerial Run time = %d \n", (Ipp32s) time1);
	printf ("Parallel Run time = %d \n", (Ipp32s) time2);
	printf ("\nSpeedup = %f\n\n", (float) (time1)/(float) time2);

	return 0;
}
