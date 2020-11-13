#include <stdio.h>
#include "stdlib.h"
#include "ipp.h"
#include "float.h"
#include <random>
#include "x86intrin.h"

using namespace std;

#define		VECTOR_SIZE			1048576


float* arr;

float get_random()
{
    static std::default_random_engine e;
    static std::uniform_real_distribution<> dis(0, 10); // rage 0 - 1
    return dis(e);
}


void print_student_info() {
    printf("******************************************************\n");
    printf("group members:\n");
    printf("                Alireza Zarenejad:      810196474\n");
    printf("                Mohammad Moein Shafi:   810196492\n");
    printf("******************************************************\n\n");
}


void make_random_float_arr() {
    arr = new float[VECTOR_SIZE];

	if(!arr){
		printf("Memory allocation error!!\n");
	}

	for (long i = 0; i < VECTOR_SIZE; i++){
		arr[i] = get_random();
	}
}


int calc_max_value_serial() {
    // Serial
    Ipp64u start, end;
    int serial_duration;
    float max_value = FLT_MIN;
    int index = 0;

    start = ippGetCpuClocks();
    for(long i=0; i < VECTOR_SIZE; i++)
        if(arr[i] > max_value)
        {
            index = i;
            max_value = arr[i];
        }
    end = ippGetCpuClocks();
    serial_duration = (Ipp32s)(end - start);

    printf("Serial method:   Max value = %f,  Index = %ld,    Run time = %d\n", max_value, long(index), serial_duration);
    return serial_duration;

}


int calc_max_value_parallel() {
    //SIMD
    Ipp64u start, end;
    int parallel_duration;
    start = ippGetCpuClocks();

	__m128 max_values = _mm_set1_ps(FLT_MIN);
	const __m128 increment = _mm_set1_ps(4);
    __m128 indices         = _mm_set_ps(3, 2, 1, 0);
    __m128 maxindices      = indices;

	for (long i = 0; i < VECTOR_SIZE; i += 4){
		const __m128 val = _mm_loadu_ps(&arr[i]);
        const __m128 cmp  = _mm_cmpgt_ps(val, max_values);
        maxindices = _mm_blendv_ps(maxindices, indices, cmp);
		max_values = _mm_max_ps(max_values, val);
		indices = _mm_add_ps(indices, increment);
	}

    float values_arr[4];
    float indices_arr[4];

    _mm_storeu_ps(values_arr, max_values);
    _mm_storeu_ps(indices_arr, maxindices);

    float max_index = indices_arr[0];
    float max_value = values_arr[0];

	for (int i = 1; i < 4; i++){
		if (values_arr[i] > max_value){
			max_value = values_arr[i];
			max_index = indices_arr[i];
		}
	}

    end = ippGetCpuClocks();
    parallel_duration = (Ipp32s)(end - start);

    printf("Parallel method: Max value = %f,  Index = %ld,    Run time = %d\n", max_value, long(max_index), parallel_duration);

    return parallel_duration;
    
}

int main(int argc, const char** argv) {
    print_student_info();

    make_random_float_arr();

    int serial_duration = calc_max_value_serial();
    int parallel_duration = calc_max_value_parallel();

    printf("\nSpeed up: %f\n\n", ((float)serial_duration/(float)parallel_duration));

    return 0;
}
