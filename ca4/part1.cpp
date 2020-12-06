#include "stdio.h"
#include "omp.h"
#include "ipp.h"
#include <algorithm>
#include <random>


#define VECTOR_SIZE	1048576

// #define VECTOR_SIZE	20

__attribute__((aligned(16))) float random_array[VECTOR_SIZE];

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

	if(!random_array){
		printf("Memory allocation error!!\n");
	}

	for (long i = 0; i < VECTOR_SIZE; i++){
		random_array[i] = get_random();
	}
}


int serial_max(float* array) {
	Ipp64u start, end;
	int serial_duration;
	start = ippGetCpuClocks();
	float max_value = array[0];
	long max_index = 0;
	for (long i = 0; i < VECTOR_SIZE; i++){
		if(array[i] >= max_value){
			max_value = array[i];
			max_index = i;
		}
	}
	end = ippGetCpuClocks();
    serial_duration = (Ipp32s)(end - start);

    printf("Find Max in Serial:	Max value = %f,  Index = %ld, Run time = %d\n", max_value, max_index, serial_duration);
    return serial_duration;
}


int omp_max(float* array) {
	Ipp64u start, end;
	int omp_duration;
	start = ippGetCpuClocks();
	float max_value = array[0];

	#pragma omp parallel for reduction(max:max_value)
	for (long i = 0; i < VECTOR_SIZE; i++)
		max_value = max_value > array[i] ? max_value : array[i];
	
	end = ippGetCpuClocks();
    omp_duration = (Ipp32s)(end - start);
	printf("Find Max in omp: Max value = %f, Run time = %d\n", max_value, omp_duration);
    return omp_duration;
}



int main(int argc, char const *argv[])
{
   print_student_info();

   make_random_float_arr();
	
	int serial_duration = serial_max(random_array);

	int omp_duration = omp_max(random_array);


    printf("Speed up: %f\n", ((float)serial_duration/(float)omp_duration));

}
