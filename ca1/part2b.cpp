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

float floatArray [4] = {1.1, 2.2, 3.3, 4.4};


int main()
{
    __m128 a;

	a = _mm_load_ps(floatArray);

    print_spfp_vector(a);
}