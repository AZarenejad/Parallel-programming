#include "stdio.h"
#include "x86intrin.h"
#include "string"
using namespace std;

typedef union {
	__m128i 			int128;
	
	unsigned char		m128_u8[16];
	signed char			m128_i8[16];

	unsigned short		m128_u16[8];
	signed short		m128_i16[8];

	unsigned int		m128_u32[4];
	signed int			m128_i32[4];

	unsigned long long	m128_u64[2];
	signed long long	m128_i64[2];

} intVec;

void print_u8 (__m128i a)
{
	printf ("Unsigned byte: ");
	intVec tmp;
	tmp.int128 = a;
	printf ("[");
	for (int i=15; i>0; i--) {
		printf ("%X, ", tmp.m128_u8[i]);
	}
	printf ("%X]\n\n", tmp.m128_u8[0]);
}

void print_u16 (__m128i a)
{
	printf ("Unsigned word: ");
	intVec tmp;
	tmp.int128 = a;
	printf ("[");
	for (int i=7; i>0; i--) {
		printf ("%X, ", tmp.m128_u16[i]);
	}
	printf ("%X]\n\n", tmp.m128_u16[0]);
}

void print_u32 (__m128i a)
{
	printf ("Unsigned double word: ");
	intVec tmp;
	tmp.int128 = a;
	printf ("[");
	for (int i=3; i>0; i--) {
		printf ("%X, ", tmp.m128_u32[i]);
	}
	printf ("%X]\n\n", tmp.m128_u32[0]);
}

void print_u64 (__m128i a)
{
	printf ("Unsigned quad word: ");
	intVec tmp;
	tmp.int128 = a;
	printf ("[");
	for (int i=1; i>0; i--) {
		printf ("%llX, ", tmp.m128_u64[i]);
	}
	printf ("%llX]\n\n", tmp.m128_u64[0]);
}


void print_i8 (__m128i a)
{
	printf ("Signed byte: ");
	intVec tmp;
	tmp.int128 = a;
	printf ("[");
	for (int i=15; i>0; i--) {
		printf ("%d, ", tmp.m128_i8[i]);
	}
	printf ("%d]\n\n", tmp.m128_i8[0]);
}

void print_i16 (__m128i a)
{
	printf ("Signed word: ");
	intVec tmp;
	tmp.int128 = a;
	printf ("[");
	for (int i=7; i>0; i--) {
		printf ("%d, ", tmp.m128_i16[i]);
	}
	printf ("%d]\n\n", tmp.m128_i16[0]);
}

void print_i32 (__m128i a)
{
	printf ("Signed double word: ");
	intVec tmp;
	tmp.int128 = a;
	printf ("[");
	for (int i=3; i>0; i--) {
		printf ("%d, ", tmp.m128_i32[i]);
	}
	printf ("%d]\n\n", tmp.m128_i32[0]);
}

void print_i64 (__m128i a)
{
	printf ("Signed quad word: ");
	intVec tmp;
	tmp.int128 = a;
	printf ("[");
	for (int i=1; i>0; i--) {
		printf ("%lld, ", tmp.m128_i64[i]);
	}
	printf ("%lld]\n\n", tmp.m128_i64[0]);
}



void print_int_vector(__m128i a, string type)
{
	if(type == "u8")
		print_u8(a);
	else if(type == "u16")
		print_u16(a);
	else if(type == "u32")
		print_u32(a);
	else if(type == "u64")
		print_u64(a);
	else if(type == "i8")
		print_i8(a);
	else if(type == "i16")
		print_i16(a);
	else if(type == "i32")
		print_i32(a);
	else if (type == "i64")
		print_i64(a);
	else
		printf("unknown type\n");
	
}



void print_spfp_vector(__m128 a)
{
}

unsigned char intArray [16] = {	0X00, 0X11, 0X22, 0X33, 0X44, 0X55, 0X66, 0X77,
								0X88, 0X99, 0XAA, 0XBB, 0XCC, 0XDD, 0XEE, 0XFF};

int main(void)
{
	__m128i a;
	a = _mm_load_si128((const __m128i*)intArray);

	print_int_vector (a , "u8");

	print_int_vector (a , "u16");

	print_int_vector(a, "u32");

	print_int_vector(a, "u64");

	print_int_vector (a , "i8");

	print_int_vector (a , "i16");

	print_int_vector(a, "i32");

	print_int_vector(a, "i64");

	return 0;
}