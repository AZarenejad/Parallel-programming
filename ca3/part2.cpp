#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include "ipp.h"

using namespace std;
using namespace cv;

#define FIRST_IMAGE_PATH "CA03__Q2__Image__01_16x.png"
#define SECOND_IMAGE_PATH "CA03__Q2__Image__02_16x.png"

void print_student_info() {
    printf("******************************************************\n");
    printf("group members:\n");
    printf("                Alireza Zarenejad:      810196474\n");
    printf("                Mohammad Moein Shafi:   810196492\n");
    printf("******************************************************\n\n");
}


int serial_part()
{
    Mat first_image, second_image, result_image;
    Ipp64u start, end;
    first_image = imread(FIRST_IMAGE_PATH, IMREAD_GRAYSCALE);
    result_image = imread(FIRST_IMAGE_PATH, IMREAD_GRAYSCALE);
    second_image = imread(SECOND_IMAGE_PATH, IMREAD_GRAYSCALE);
    if(first_image.empty())
    {
        cout << "couldn't open the first image" << endl;
        return -1;
    }
    if(second_image.empty())
    {
        cout << "couldn't open the second image" << endl;
        return -1;
    }
    int NROWS = first_image.rows;
    int NCOLS = first_image.cols;
    int NROWS_2 = second_image.rows;
    int NCOLS_2 = second_image.cols;

    uint8_t* first_image_data = (uint8_t*)first_image.data;
    uint8_t* result_image_data = (uint8_t*)result_image.data;
    uint8_t* second_image_data = (uint8_t*)second_image.data;

    start = ippGetCpuClocks();
    for(int i = 0; i < NROWS_2; i++)
        for(int j = 0; j < NCOLS_2; j++)
            {
                int index = i * NCOLS + j;
                int index2 = i * NCOLS_2 + j;
                result_image_data[index] = first_image_data[index] + second_image_data[index2] >> 1;
            }
    end = ippGetCpuClocks();
    int serial_time = (Ipp32u)(end - start);
    cout << "Serial takes " << serial_time << " clock cycles"<< endl;
    imwrite("q2_serial.png", result_image);
    return serial_time;
}


int parallel_part()
{
    Mat first_image, second_image, result_image;
    Ipp64u start, end;
    first_image = imread(FIRST_IMAGE_PATH, IMREAD_GRAYSCALE);
    result_image = imread(FIRST_IMAGE_PATH, IMREAD_GRAYSCALE);
    second_image = imread(SECOND_IMAGE_PATH, IMREAD_GRAYSCALE);
    if(first_image.empty())
    {
        cout << "couldn't open the first image" << endl;
        return -1;
    }
    if(second_image.empty())
    {
        cout << "couldn't open the second image" << endl;
        return -1;
    }
    int NROWS = first_image.rows;
    int NCOLS = first_image.cols;
    int NROWS_2 = second_image.rows;
    int NCOLS_2 = second_image.cols;

    __m128i* first_image_data = (__m128i*) first_image.data;
    __m128i* result_image_data = (__m128i*) result_image.data;
    __m128i* second_image_data = (__m128i*) second_image.data;

    start = ippGetCpuClocks();
	__m128i shift = _mm_set1_epi8(127);
	__m128i m1, m2;
    int index, index2;
    for(int i = 0; i < NROWS_2; i++)
        for(int j = 0; j < NCOLS_2 / 16; j++)
        {
            index = i * (NCOLS / 16) + j;
            index2 = i * (NCOLS_2 / 16) + j;

            m1 = _mm_loadu_si128(first_image_data + index);
            m2 = _mm_loadu_si128(second_image_data + index2);
            m2 = m2 >> 1;
            m2 = _mm_and_si128(shift, m2);
            m1 = _mm_adds_epu8(m1, m2);
            _mm_store_si128(result_image_data + index, m1);
        }

    end = ippGetCpuClocks();
    int parallel_time = (Ipp32u)(end - start);
    cout << "Parallel takes " << parallel_time << " clock cycles"<< endl;
    imwrite("q2_parallel.png", result_image);
    return parallel_time;

}


int main(int argc, char const *argv[])
{
    cout << (4 >> 1 ) << endl;
    print_student_info();
    int serial_time = serial_part();
    if(serial_time == -1)
        return 1;
    int parallel_time = parallel_part();
    if(parallel_time == -1)
        return 1;
     
    cout << "Speed up: " << float(serial_time) / parallel_time << endl;

    return 0;
}
