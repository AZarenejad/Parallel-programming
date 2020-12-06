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


int serial_sort(float* array) {
    return  1;
}


int omp_sort(float* array) {
    return 1;
}



int main(int argc, char const *argv[])
{
   print_student_info();

   make_random_float_arr();


  	int serial_duration = serial_sort(random_array);

	int omp_duration = omp_sort(random_array);


    printf("Speed up: %f\n", ((float)serial_duration/(float)omp_duration));

}
