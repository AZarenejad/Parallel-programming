#include "stdio.h"
#include <pthread.h>
#include "ipp.h"
#include <float.h>
#include <algorithm>
#include <random>


#define VECTOR_SIZE	1048576
#define THREAD_NUMS 10

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


float maximum_value[THREAD_NUMS] = { 0 };
long maximum_index[THREAD_NUMS] = { 0 }; 
long thread_num = 0; 

void *maximum(void* arg) 
{ 
    int thread_number = thread_num++; 
    float max_value_local = 0; 
    long max_index_local = 0;
  
    for (long i = thread_number * (VECTOR_SIZE / THREAD_NUMS); i < (thread_number + 1) * (VECTOR_SIZE / THREAD_NUMS); i++) { 
        if (random_array[i] > max_value_local){
            max_value_local = random_array[i];
            max_index_local = i;
        }
    } 
    maximum_value[thread_number] = max_value_local; 
    maximum_index[thread_number] = max_index_local;
    return NULL;
}

int pthread_max(float* array)
{   
	Ipp64u start, end;
	int pthread_duration;
	start = ippGetCpuClocks();
    
	float max_value = 0; 
	long max_index = 0;
    long i; 
    pthread_t thread_handler[THREAD_NUMS]; 
  
    for (i = 0; i < THREAD_NUMS; i++) 
        pthread_create(&thread_handler[i], NULL, maximum, NULL); 
  
    for (i = 0; i < THREAD_NUMS; i++) 
        pthread_join(thread_handler[i], NULL); 

    for (i = 0; i < THREAD_NUMS; i++) { 
        if (maximum_value[i] > max_value) {
            max_value = maximum_value[i];
            max_index = maximum_index[i];
        }

    } 

    end = ippGetCpuClocks();

    pthread_duration = (Ipp32s)(end - start);

    printf("Find Max in Parallel:	Max value = %f,  Index = %ld, Run time = %d\n", max_value, max_index, pthread_duration);
    return pthread_duration;
}







int main(int argc, char const *argv[])
{
    print_student_info();
    
    make_random_float_arr();
    
    int serial_duration = serial_max(random_array);

	int pthread_duration = pthread_max(random_array);


    printf("SpeedUp: %f\n", ((float)serial_duration/(float)pthread_duration));

}