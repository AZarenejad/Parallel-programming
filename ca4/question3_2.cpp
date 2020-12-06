#include <stdio.h>
#include <math.h>
#include <sys/time.h> 
#include <sys/resource.h>  
#include <omp.h>

double timeGetTime() 
{     
	struct timeval time;     
	struct timezone zone;     
	gettimeofday(&time, &zone);     
	return time.tv_sec + time.tv_usec*1e-6; 
}  


const long int VERYBIG = 100000;

int main( void )
{
  int i;
  long int j, k, sum;
  double sumx, sumy, total, z;
  double starttime, elapsedtime;
  // ---------------------------------------------------------------------
  // Output a start message
  printf( "OpenMP Parallel Timings for %ld iterations \n\n", VERYBIG );

  // repeat experiment several times
  for( i=0; i<6; i++ )
  {
    // reset check sum and total
    sum = 0;
    total = 0.0;
    
    // Work loop, do some work by looping VERYBIG times
    #pragma omp parallel num_threads(4) private( sumx, sumy, k, starttime, elapsedtime)   \
      reduction( +: sum, total)
      {
        // get starting time
        starttime = timeGetTime();
        #pragma omp for schedule(static)
        for( int j=0; j<VERYBIG; j++)
        {
          // increment check sum
          sum += 1;
          
          // Calculate first arithmetic series
          sumx = 0.0;
          for( k=0; k<j; k++ )
          sumx = sumx + (double)k;

          // Calculate second arithmetic series
          sumy = 0.0;
          for( k=j; k>0; k-- )
          sumy = sumy + (double)k;

          if( sumx > 0.0 )total = total + 1.0 / sqrt( sumx );
          if( sumy > 0.0 )total = total + 1.0 / sqrt( sumy );
      }
    
      // get ending time and use it to determine elapsed time
      elapsedtime = timeGetTime() - starttime;
      printf("elapsedtime for thread number %d = %10d \n", omp_get_thread_num(), (int)(elapsedtime * 1000));
    }
    // get ending time and use it to determine elapsed time
    printf("**************************\n");
  }

  // return integer as required by function header
  return 0;
}
// **********************************************************************
