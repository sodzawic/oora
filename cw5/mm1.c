#include <stdio.h>
#include <stdlib.h>
 
#ifndef SIZE
#define SIZE 500
#endif

#include <sys/time.h>
#include <time.h>

#include "prof.h"

static double gtod_ref_time_sec = 0.0;

/* Adapted from the bl2_clock() routine in the BLIS library */

double dclock()
{
  double         the_time, norm_sec;
  struct timeval tv;
  gettimeofday( &tv, NULL );
  if ( gtod_ref_time_sec == 0.0 )
    gtod_ref_time_sec = ( double ) tv.tv_sec;
  norm_sec = ( double ) tv.tv_sec - gtod_ref_time_sec;
  the_time = norm_sec + tv.tv_usec * 1.0e-6;
  return the_time;
}

int mm(double first[][SIZE], double second[][SIZE], double multiply[][SIZE])
{
  int i,j,k; 
  double sum = 0;
  for (i = 0; i < SIZE; i++) { //rows in multiply
    for (j = 0; j < SIZE; j++) { //columns in multiply
      for (k = 0; k < SIZE; k++) { //columns in first and rows in second
	    sum = sum + first[i][k]*second[k][j];
	  } 
          multiply[i][j] = sum;
	  sum = 0;
    }
  }
  return 0;
}

int main( int argc, const char* argv[] )
{
  int i,j,iret;
  if (argc > 1) {
    double first[SIZE][SIZE];
    double second[SIZE][SIZE];
    double multiply[SIZE][SIZE];
    double dtime;
    for (i = 0; i < SIZE; i++) { //rows in first
      for (j = 0; j < SIZE; j++) { //columns in first
        first[i][j]=i+j;
        second[i][j]=i-j;
        multiply[i][j]=0.0;
      }
    }
    struct papi_context context = papi_start(argv[1]);
    dtime = dclock();
    iret=mm(first,second,multiply); 
    dtime = dclock()-dtime;
    papi_stop(context);
    printf( "Time: %le \n", dtime);

    double check=0.0;
    for(i=0;i<SIZE;i++){
      for(j=0;j<SIZE;j++){
        check+=multiply[i][j];
      }
    }
    printf("check %le \n",check); 
    fflush( stdout );
  } else {
    printf ("%s <event_name>\n", argv[0]);
  }

  return iret;
}

