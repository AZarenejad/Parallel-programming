#include <iostream>
#include "omp.h"
#include "ipp.h"
#include <random>

using namespace std;

#define		VECTOR_SIZE   2 << 20

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

void print(vector<float>& numbers_s)
{
    for (auto a : numbers_s)
        cout << a << endl;
}

void fill_with_random_numbers(vector<float>& numbers_s, vector<float>& numbers_pm, vector<float>& numbers_pq)
{
    for (int i = 0; i < VECTOR_SIZE; i++)
    {
        numbers_s[i] = get_random();
        numbers_pm[i] = numbers_s[i];
        numbers_pq[i] = numbers_s[i];
    }
}

void swap(vector<float>& numbers_s, int first, int second) 
{ 
    float temp = numbers_s[first]; 
    numbers_s[first] = numbers_s[second]; 
    numbers_s[second] = temp; 
} 

int partition (vector<float>& numbers_s, int low, int high) 
{ 
    float pivot = numbers_s[high];
    int i = (low - 1);
  
    for (int j = low; j <= high - 1; j++) 
    { 
        if (numbers_s[j] <= pivot) 
        { 
            i++;
            swap(numbers_s, i, j); 
        } 
    } 
    swap(numbers_s, i + 1, high); 
    return (i + 1); 
} 

void quickSort(vector<float>& numbers_s, int low, int high) 
{ 
    if (low < high) 
    { 
        int pi = partition(numbers_s, low, high); 
  
        quickSort(numbers_s, low, pi - 1); 
        quickSort(numbers_s, pi + 1, high); 
    } 
} 

int serial_part(vector<float>& numbers_s)
{
    Ipp64u start, end;
    
    start = ippGetCpuClocks();
    quickSort(numbers_s, 0, VECTOR_SIZE - 1);
    end = ippGetCpuClocks();

    int serial_time = (Ipp32u)(end - start);
    return serial_time;
}

void merge(vector<float>& numbers_pm, vector<float>& temp, int start, int end)
{
   int m = (end + start - 1) / 2;
   int i = start;
   int j = m + 1;
   int index = start;

   while (i < m + 1 && j < end + 1)
   {
      if (numbers_pm[i] < numbers_pm[j])
      {
         temp[index] = numbers_pm[i];
         index++;
         i++;
      } 
      else
      {
         temp[index] = numbers_pm[j];
         index++;
         j++;
      }
   }
   while (i < m + 1)
   {
      temp[index] = numbers_pm[i];
      index++;
      i++;
   }
   while (j < end + 1)
   {
        temp[index] = numbers_pm[j];
        index++;
        j++;
   }
   for (int i = start; i < end + 1; i++)
       numbers_pm[i] = temp[i];

}

void mergesort_serial(vector<float>& numbers_pm, vector<float>& temp, int start, int end)
{
	if (end - start < 2)
    { 
		if (numbers_pm[end] >= numbers_pm[start])
			return;
		else
        {
			swap(numbers_pm[start], numbers_pm[end]);
			return;
		}
	}

    int m = (end + start - 1) / 2;
	mergesort_serial(numbers_pm, temp, start, m);
	mergesort_serial(numbers_pm, temp, m + 1, end);
	merge(numbers_pm, temp, start, end);
}

void parallel_mergesort(vector<float>& numbers_pm, vector<float>& temp, int start, int end, int threads)
{
    if (start >= end)
        return;

    int m = 0;
    if (threads == 1 || end - start < 2)
        mergesort_serial(numbers_pm, temp, start, end);

    else if (threads > 1)
    {
        m = (end + start - 1) / 2;
        #pragma omp task default(shared)
            parallel_mergesort(numbers_pm, temp, start, m, threads / 2);

        #pragma omp task default(shared)
            parallel_mergesort(numbers_pm, temp, m + 1, end, threads / 2);

        #pragma omp taskwait
            merge(numbers_pm, temp, start, end);
     }
}

int parallel_part_mergesort(vector<float>& numbers_pm, vector<float>& temp)
{
    int num_threads = 40;
    Ipp64u start, end;
    
    start = ippGetCpuClocks();
    #pragma omp parallel default(shared)
        #pragma omp single
            parallel_mergesort(numbers_pm, temp, 0, VECTOR_SIZE - 1, num_threads);
    end = ippGetCpuClocks();

    int time = (Ipp32u)(end - start);
    return time;
}

int parallel_quicksort(vector<float>& numbers_pq, int low, int high, int threads)
{
    if (threads == 1)
        quickSort(numbers_pq, low, high);

    else if (low < high) 
    { 
        int pi = partition(numbers_pq, low, high); 
        #pragma omp task default(shared)
            parallel_quicksort(numbers_pq, low, pi - 1, threads / 2);

        #pragma omp task default(shared)
            parallel_quicksort(numbers_pq, pi + 1, high, threads / 2);
    } 
}

int parallel_part_quicksort(vector<float>& numbers_pq)
{
    int num_threads = 100;
    Ipp64u start, end;
    
    start = ippGetCpuClocks();
    #pragma omp parallel default(shared)
        #pragma omp single
            parallel_quicksort(numbers_pq, 0, VECTOR_SIZE - 1, num_threads);
    end = ippGetCpuClocks();

    int time = (Ipp32u)(end - start);
    return time;
}


void check_equality(vector<float>& numbers_s, vector<float>& numbers_pm, vector<float>& numbers_pq)
{
    for (int i = 0; i < VECTOR_SIZE; i++)
        if (numbers_s[i] != numbers_pm[i] || numbers_s[i] != numbers_pq[i])
        {
            cout << "s: " << numbers_s[i] << " , pm: " << numbers_pm[i] << " , pq: " << numbers_pq[i] << endl;
            cout << "Error! Output in Parallel section is not the same with the Serial part!\n";
            return;
        }
    cout << "Output in Parallel section is the same as Serial part. :)\n";
}

int main()
{
    vector<float> numbers_s(VECTOR_SIZE);
    vector<float> numbers_pm(VECTOR_SIZE);
    vector<float> numbers_pq(VECTOR_SIZE);
    vector<float> temp(VECTOR_SIZE);
    print_student_info();
    fill_with_random_numbers(numbers_s, numbers_pm, numbers_pq);

    int serial_time = serial_part(numbers_s);
    if(serial_time == -1)
        return 1;

    int parallel_time_mergesort = parallel_part_mergesort(numbers_pm, temp);
    if(parallel_time_mergesort == -1)
        return 1;

    int parallel_time_quicksort = parallel_part_quicksort(numbers_pq);
    if(parallel_time_quicksort == -1)
        return 1;

    check_equality(numbers_s, numbers_pm, numbers_pq);
     
    cout << "\nSerial takes " << serial_time << " clock cycles"<< endl;
    cout << "Parallel with Mergesort takes " << parallel_time_mergesort << " clock cycles"<< endl;
    cout << "Parallel with Quciksort takes " << parallel_time_quicksort << " clock cycles"<< endl;
    cout << "\nSpeed up with Parallel Mergesort: " << float(serial_time) / parallel_time_mergesort << endl;
    cout << "Speed up with Parallel Quicksort: " << float(serial_time) / parallel_time_quicksort << endl;

    exit(EXIT_SUCCESS);
}
