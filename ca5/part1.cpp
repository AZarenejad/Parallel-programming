#include "stdio.h"
#include <pthread.h>
#include "ipp.h"
#include <float.h>
#include <algorithm>
#include <random>
#include <iostream>


#define VECTOR_SIZE	1048576

#define THREAD_NUMS 4

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
    static std::uniform_real_distribution<> dis(0, 100); // rage 0 - 1
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


typedef struct {
    long start_index;
    long end_index;
    int thread_number;
} thread_param;


typedef struct {
    float max_value;
    long max_index;
} thread_max_result;
 
thread_param thread_params[THREAD_NUMS];

float max_value_global = -1;
long max_index_global = -1;
pthread_mutex_t lock;

void *maximum(void* arg) 
{ 
    thread_param* incoming_param = (thread_param*)arg;
    long start_index = incoming_param->start_index;
    long end_index = incoming_param->end_index;
    int thread_number = incoming_param->thread_number;
    thread_max_result* max_local = (thread_max_result*) malloc(sizeof(thread_max_result));
    max_local->max_value = -1;
    max_local->max_index = -1;
 
    for (long i = start_index; i < end_index; i++) { 
        if (random_array[i] > max_local->max_value){
            max_local->max_value = random_array[i];
            max_local->max_index = i;
        }
    }

    pthread_exit(max_local);
}


void *maximum_lock(void* arg) 
{ 
    thread_param* incoming_param = (thread_param*)arg;
    long start_index = incoming_param->start_index;
    long end_index = incoming_param->end_index;
    int thread_number = incoming_param->thread_number;

    float max_value_local = -1;
    long max_index_local = -1;
 
    for (long i = start_index; i < end_index; i++) { 
        if (random_array[i] > max_value_local){
            max_value_local  = random_array[i];
            max_index_local = i;
        }
    }

   pthread_mutex_lock(&lock);
   if (max_value_global <  max_value_local){
       max_value_global = max_value_local;
       max_index_global = max_index_local;
   }
   pthread_mutex_unlock(&lock);

    return NULL;
}


int pthread_max(float* array)
{   
	Ipp64u start, end;
	int pthread_duration;
	float max_value = -1; 
	long max_index = -1;
    pthread_t thread_handler[THREAD_NUMS]; 
    start = ippGetCpuClocks();
    for (int i = 0; i < THREAD_NUMS; i++)
    { 
        thread_params[i] = {i * (VECTOR_SIZE / THREAD_NUMS), (i + 1) * (VECTOR_SIZE / THREAD_NUMS), i};
        pthread_create(&thread_handler[i], NULL, maximum, &thread_params[i]); 
    }

    void* thread_result;
    thread_max_result* max_thread;  
    for (int i = 0; i < THREAD_NUMS; i++){
        pthread_join(thread_handler[i], &thread_result);
        max_thread = (thread_max_result*) thread_result;
        if(max_thread->max_value > max_value){
            max_value = max_thread->max_value;
            max_index =  max_thread->max_index;
        }
        free(max_thread);
    } 

    end = ippGetCpuClocks();
    pthread_duration = (Ipp32s)(end - start);
    printf("Find Max in Parallel:	Max value = %f,  Index = %ld, Run time = %d\n", max_value, max_index, pthread_duration);
    return pthread_duration;
}


int pthread_max_lock(float* array)
{   
	Ipp64u start, end;
	int pthread_duration;
	float max_value = -1; 
	long max_index = -1;
    pthread_t thread_handler[THREAD_NUMS];
    pthread_mutex_init(&lock, NULL); 

    start = ippGetCpuClocks();
    for (int i = 0; i < THREAD_NUMS; i++)
    { 
        thread_params[i] = {i * (VECTOR_SIZE / THREAD_NUMS), (i + 1) * (VECTOR_SIZE / THREAD_NUMS), i};
        pthread_create(&thread_handler[i], NULL, maximum_lock, &thread_params[i]); 
    }

    for (int i = 0; i < THREAD_NUMS; i++){
        pthread_join(thread_handler[i], NULL);
    }

    pthread_mutex_destroy(&lock);
    end = ippGetCpuClocks();
    pthread_duration = (Ipp32s)(end - start);
    printf("Find Max in Parallel lock:	Max value = %f,  Index = %ld, Run time = %d\n", max_value_global, max_index_global, pthread_duration);
    return pthread_duration;
}


int main(int argc, char const *argv[])
{
    print_student_info();
    
    make_random_float_arr();
    
    int serial_duration = serial_max(random_array);

	int pthread_duration = pthread_max(random_array);

    int pthread_duration_lock = pthread_max_lock(random_array);


    printf("SpeedUp: %f\n", ((float)serial_duration/(float)pthread_duration));

    printf("SpeedUp lock: %f\n", ((float)serial_duration/(float)pthread_duration_lock));
}
