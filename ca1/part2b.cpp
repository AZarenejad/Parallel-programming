#include "stdio.h"
#include "x86intrin.h"


typedef union {
	__m128 			float128;
	
	float		sp[4];
} floatVec;


void print_spfp_vector(__m128 a)
{
    floatVec tmp;
	tmp.float128 = a;
	printf ("[");
	for (int i=3; i>0; i--) {
		printf ("%f, ", tmp.sp[i]);
	}
	printf ("%f]\n\n", tmp.sp[0]);
}

float floatArray [4] = {1.1, 1.5, 2.2, 2.7};

unsigned char floatArray1 [16] = { 0XCD, 0XCC, 0X8C, 0X3F, 0XCD, 0XCC, 0X0C, 0X40,
								0X33, 0X33, 0X53, 0X40, 0XCD, 0XCC, 0X8C, 0X40};


int main()
{
    __m128 a;
    __m128 b;

	a = _mm_load_ps((const float*)floatArray1);
    b = _mm_load_ps(floatArray);

    print_spfp_vector(a);
    print_spfp_vector(b);
}