#include "stdio.h"
#include "omp.h"
#include "ipp.h"
#include <float.h>
#include <algorithm>
#include <random>


#define VECTOR_SIZE	1048576

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


int omp_lock_max(float* array)
{   
	Ipp64u start, end;
	int omp_lock_duration;
	omp_lock_t lock;
	omp_init_lock(&lock);

	start = ippGetCpuClocks();

	float max_value = array[0];
	long max_index = 0;

	#pragma omp parallel for num_threads(4)
	for (long i = 0; i < VECTOR_SIZE; i++){
		if(array[i] > max_value) {
			omp_set_lock(&lock);
				max_value = array[i];
				max_index = i;
			omp_unset_lock(&lock);
		}
	}

	omp_destroy_lock(&lock);
	end = ippGetCpuClocks();
    omp_lock_duration = (Ipp32s)(end - start);

	printf("Find Max with omp lock:	Max value = %f,  Index = %ld, Run time = %d\n", max_value, max_index, omp_lock_duration);
    return omp_lock_duration;
}


long opm_critical_max(float* array)
{   
	Ipp64u start, end;
	int omp_critical_duration;
	start = ippGetCpuClocks();
	float max_value = array[0];
	long max_index = 0;
	float max_value_local;
	long max_index_local;
	#pragma omp parallel num_threads(2) shared(array, max_value, max_index) private(max_value_local, max_index_local)
	{
		max_value_local = FLT_MIN;
		max_index_local = -1;
		#pragma omp for nowait
		for (long i = 0; i < VECTOR_SIZE; i++){
			if (array[i] > max_value_local){
				max_value_local = array[i];
				max_index_local = i;
			}
		}
		#pragma omp critical
		{
			if(max_value_local > max_value){
				max_value = max_value_local;
				max_index = max_index_local;
			}
		}
	}
	end = ippGetCpuClocks();
    omp_critical_duration = (Ipp32s)(end - start);
	printf("Find Max with omp crttical:	Max value = %f,  Index = %ld, Run time = %d\n", max_value, max_index, omp_critical_duration);
    return omp_critical_duration;
}


int omp_max_reduction_without_index(float* array) {
	Ipp64u start, end;
	int omp_duration;
	start = ippGetCpuClocks();
	float max_value = array[0];

	#pragma omp parallel for reduction(max:max_value)
	for (long i = 0; i < VECTOR_SIZE; i++)
		max_value = max_value > array[i] ? max_value : array[i];
	
	end = ippGetCpuClocks();
    omp_duration = (Ipp32s)(end - start);
	printf("Find Max in omp reduction without index: Max value = %f, Run time = %d\n", max_value, omp_duration);
    return omp_duration;
}


struct find_max {
	float val;
	long index; 
};

#pragma omp declare reduction(maximum : struct find_max : omp_out = omp_in.val > omp_out.val ? omp_in : omp_out)

long omp_max_with_reduction_declaration(float* array)
{   

	Ipp64u start, end;
	int omp_reduction_duration;

	start = ippGetCpuClocks();

	struct find_max max;
	max.val = FLT_MIN;
	max.index = -1;

	#pragma omp parallel for num_threads(4) reduction(maximum:max)
	for (long i = 0; i < VECTOR_SIZE; i++){
		if (array[i] > max.val){
			max.val = array[i];
			max.index = i;
		}
	}

	end = ippGetCpuClocks();
   	omp_reduction_duration = (Ipp32s)(end - start);
	printf("Find Max in omp reduction: Max value = %f, Max Index: %ld, Run time = %d\n", max.val, max.index, omp_reduction_duration);
    return omp_reduction_duration;
}



int main(int argc, char const *argv[])
{
   print_student_info();

   make_random_float_arr();
	
	int serial_duration = serial_max(random_array);

	int omp_lock_duration = omp_lock_max(random_array);

	int omp_critical_duration = opm_critical_max(random_array);

	int omp_duration_reduction_without_index = omp_max_reduction_without_index(random_array);

	int omp_reduction_duration = omp_max_with_reduction_declaration(random_array);


    printf("Speed up with omp lock: %f\n", ((float)serial_duration/(float)omp_lock_duration));

	printf("Speed up with omp critical: %f\n", ((float)serial_duration/(float)omp_critical_duration));

	printf("Speed up with omp reduction without index: %f\n", ((float)serial_duration/(float)omp_duration_reduction_without_index));

	printf("Speed up with omp reduction declaration: %f\n", ((float)serial_duration/(float)omp_reduction_duration));

}
