#include <stdio.h>
#include <stdlib.h>
 
#ifndef SIZE
#define SIZE 512
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
  register unsigned int i,j,k; //<- OPT 1
  //  register unsigned int local_size=SIZE; //<- OPT 2
  register double sum = 0;
  for (i = SIZE; i-- ; ) { //rows in multiply <- OPT 3
    for (j = SIZE; j-- ;) { //columns in multiply
      for (k=0; k<SIZE; ){
      if(k<SIZE-8) { //<- OPT 4
	sum = sum + first[i][k]*second[j][k] //<- OPT 5 (k<->j)
	+ first[i][k+1]*second[j][k+1]
	+ first[i][k+2]*second[j][k+2]
	+ first[i][k+3]*second[j][k+3]
	+ first[i][k+4]*second[j][k+4]
	+ first[i][k+5]*second[j][k+5]
	+ first[i][k+6]*second[j][k+6]
	+ first[i][k+7]*second[j][k+7];
	k=k+8;
      }
      else {
	sum = sum + first[i][k]*second[j][k];
	k++;
      }
      }
      multiply[i][j] = sum;
      sum=0.0;
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

