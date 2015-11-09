#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include "papi.h"

#define MX 1024
#define NITER 20
#define MEGA 1000000
#define TOT_FLOPS (2*MX*MX*NITER)

double *ad[MX];

static int papi_initialized = 0;
static int eventSet = PAPI_NULL;
static char *event_name;

int papi_init(char *ev_name) {
    /* init lib */
    int papi_err;
    int event = 0;

    papi_initialized = 1;
    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
        fprintf(stderr, "PAPI is unsupported.\n");
        papi_initialized = 0;
    }

    if (PAPI_num_counters() < 1) {
        fprintf(stderr, "PAPI is unsupported.\n");
        papi_initialized = 0;
        return 1;
    }

    if ((papi_err = PAPI_event_name_to_code(ev_name, &event)) != PAPI_OK) {
        fprintf(stderr, "Could convert event name %s to code: %s\n", ev_name, PAPI_strerror(papi_err));
        return papi_err;
    }
    event_name = ev_name;

    if ((papi_err = PAPI_create_eventset(&eventSet)) != PAPI_OK) {
        fprintf(stderr, "Could not create event set: %s\n", PAPI_strerror(papi_err));
        return papi_err;
    }

    if ((papi_err = PAPI_add_event(eventSet, event)) != PAPI_OK ) {
        fprintf(stderr, "Could not add event: %s\n", PAPI_strerror(papi_err));
        return papi_err;
    }
    return 0;
}

int papi_start(void) {
    int papi_err;
    if (papi_initialized) {
        if ((papi_err = PAPI_start(eventSet)) != PAPI_OK) {
            fprintf(stderr, "Could not start counters: %s\n", PAPI_strerror(papi_err));
            return papi_err;
        }
        return 0;
    }
    return 1;
}

int papi_stop(void) {
    int papi_err;
    if (papi_initialized) {
        long long values[1] = {0};
        if ((papi_err = PAPI_stop(eventSet, values)) != PAPI_OK) {
            fprintf(stderr, "Could not get values: %s\n", PAPI_strerror(papi_err));
            return papi_err;
        }
        printf("Performance counters for factorization stage: \n");
        printf("%s %lld\n", event_name, values[0]);
        return 0;
    }
    return 1;
}

#define SIZE 100
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

void mm(double **first, double **second, double **multiply)
{
    register int i,j,k; 
    register double sum = 0;
    for (i = 0; i < SIZE; i++) { //rows in multiply
        for (j = 0; j < SIZE; j++) { //columns in multiply
            for (k = 0; k < SIZE; k+=10) { //columns in first and rows in second
                sum = sum + first[i][k]*second[k][j];
                sum = sum + first[i][k+1]*second[k+1][j];
                sum = sum + first[i][k+2]*second[k+2][j];
                sum = sum + first[i][k+3]*second[k+3][j];
                sum = sum + first[i][k+4]*second[k+4][j];
                sum = sum + first[i][k+5]*second[k+5][j];
                sum = sum + first[i][k+6]*second[k+6][j];
                sum = sum + first[i][k+7]*second[k+7][j];
                sum = sum + first[i][k+8]*second[k+8][j];
                sum = sum + first[i][k+9]*second[k+9][j];
            } 
            multiply[i][j] = sum;
            sum = 0;
        }
    }
}

int main( int argc, char* argv[] )
{
    if (argc > 1) {
        papi_init(argv[1]);
        papi_start();

        int i,j;
        double **first = malloc(sizeof(double*[SIZE]));
        double **second = malloc(sizeof(double*[SIZE]));
        double **multiply = malloc(sizeof(double*[SIZE]));
        double dtime;

        for (i = 0; i < SIZE; i++) { //rows in first
            first[i] = malloc(sizeof(double[SIZE]));
            second[i] = malloc(sizeof(double[SIZE]));
            multiply[i] = malloc(sizeof(double[SIZE]));
            for (j = 0; j < SIZE; j++) { //columns in first
                first[i][j]=i+j;
                second[i][j]=i-j;
            }
        }
        dtime = dclock();
        for (i = 0; i < 1; ++i) {
            mm(first,second,multiply); 
        }
        dtime = dclock()-dtime;
        printf( "Time: %le \n", dtime);
        double total_sum = 0;
        for(i=0;i<SIZE;i++){
            for(j=0;j<SIZE;j++){
                total_sum += multiply[i][j];
            }
        }
        printf( "Sum: %lg \n", total_sum);

        papi_stop();
    } else {
        fprintf(stderr, "%s <event_name>\n", argv[0]);
    }
    return 0;
}

