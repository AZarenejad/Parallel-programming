#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include "ipp.h"

using namespace std;
using namespace cv;

#define FIRST_IMAGE_PATH "CA03__Q1__Image__01.png"
#define SECOND_IMAGE_PATH "CA03__Q1__Image__02.png"

void print_student_info() {
    printf("******************************************************\n");
    printf("group members:\n");
    printf("                Alireza Zarenejad:      810196474\n");
    printf("                Mohammad Moein Shafi:   810196492\n");
    printf("******************************************************\n\n");
}


int serial_part()
{
    Mat first_image, second_image, diff_image;
    Ipp64u start, end;
    first_image = imread(FIRST_IMAGE_PATH, IMREAD_GRAYSCALE);
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

    diff_image.create(NROWS, NCOLS, CV_8UC1);

    uint8_t* first_image_data = (uint8_t*)first_image.data;
    uint8_t* second_image_data = (uint8_t*) second_image.data;
    uint8_t* diff_image_data = (uint8_t*)diff_image.data;

    int index = 0;

    start = ippGetCpuClocks();
    for(int i = 0; i < NROWS; i++)
        for(int j = 0; j < NCOLS; j++)
        {
            index = i * NCOLS + j;
            diff_image_data[index] = abs(second_image_data[index] - first_image_data[index]);
        }
    end = ippGetCpuClocks();
    int serial_time = (Ipp32u)(end - start);
    cout << "Serial takes " << serial_time << " clock cycles"<< endl;
    imwrite("q1_serial.png", diff_image);
    return serial_time;
}



int parallel_part()
{
    Mat first_image, second_image, diff_image;
    Ipp64u start, end;
    first_image = imread(FIRST_IMAGE_PATH, IMREAD_GRAYSCALE);
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
    diff_image.create(NROWS, NCOLS, CV_8UC1);

    __m128i* first_image_data = (__m128i*) first_image.data;
    __m128i* second_image_data = (__m128i*) second_image.data;
    __m128i* diff_image_data = (__m128i*) diff_image.data;

    __m128i m1, m2, sub0, sub1, abs;
    int index = 0;

    start = ippGetCpuClocks();
    for(int i = 0; i < NROWS; i++)
        for(int j = 0; j < NCOLS/16; j++)
        {
            index = i * (NCOLS/16) + j;
            m1 = _mm_loadu_si128((__m128i*)(first_image_data + index));
            m2 = _mm_loadu_si128((__m128i*)(second_image_data + index));
            sub0 = _mm_subs_epu8(m1, m2);
            sub1 = _mm_subs_epu8(m2, m1);
            abs = _mm_or_si128(sub0, sub1);
            _mm_store_si128((__m128i*)(diff_image_data + index), abs);
        }
    end = ippGetCpuClocks();
    int parallel_time = (Ipp32u)(end - start);
    cout << "Parallel takes " << parallel_time << " clock cycles"<< endl;
    imwrite("q1_parallel.png", diff_image);
    return parallel_time;

}



int main(int argc, char const *argv[])
{
    print_student_info();
    int serial_time = serial_part();
    if(serial_time == -1)
        return 1;
    int parallel_time = parallel_part();
    if(parallel_time == -1)
        return 1;
     
    cout << "Speed up: " << (float)serial_time / parallel_time << endl;

    return 0;
}

