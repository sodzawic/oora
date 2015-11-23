#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "prof.h"

#ifndef SIZE
#define SIZE 500
#endif

/* for Fortran - j*SIZE + i */
//#define IDX(i, j, SIZE)	(((i) * (SIZE)) + (j))
#define IDX(i, j, SIZE) (((j)+ (i)*(SIZE)))

int
chol(double *A)
{
	unsigned int i;
	unsigned int j;
	unsigned int k;

	for (j = 0; j < SIZE; j++) {
		for (i = j; i < SIZE; i++) {
			for (k = 0; k < j; ++k) {
				A[IDX(i, j, SIZE)] -= A[IDX(i, k, SIZE)] *
				    A[IDX(j, k, SIZE)];
			}
		}

		if (A[IDX(j, j, SIZE)] < 0.0) {
			return (1);
		}

		A[IDX(j, j, SIZE)] = sqrt(A[IDX(j, j, SIZE)]);
		for (i = j + 1; i < SIZE; i++)
			A[IDX(i, j, SIZE)] /= A[IDX(j, j, SIZE)];
	}

	return (0);
}

void 
gen(double *A)
{
    int i, j;
    for (i = 0; i < SIZE; ++ i) {
        for (j = 0; j < i; ++j) {
            A[IDX(i, j, SIZE)] = A[IDX(j, i, SIZE)] = rand();
        }
    }
}

int
main(int argc, char **argv)
{
	double *A;
    if (argc > 1) {
        A = calloc(SIZE*SIZE, sizeof(double));
        assert(A != NULL);

        A[IDX(0, 0, SIZE)] = 4.0;   A[IDX(0, 1, SIZE)] = 12.0;  A[IDX(0, 2, SIZE)] = -16.0;
        A[IDX(1, 0, SIZE)] = 12.0;  A[IDX(1, 1, SIZE)] = 37.0;  A[IDX(1, 2, SIZE)] = -43.0;
        A[IDX(2, 0, SIZE)] = -16.0; A[IDX(2, 1, SIZE)] = -43.0; A[IDX(2, 2, SIZE)] = 98.0;

        struct papi_context context = papi_start(argv[1]);
        if (chol(A)) {
            fprintf(stderr, "Error: matrix is either not symmetric or not positive definite.\n");
        } else {
            papi_stop(context);
            //fprintf(stdout, "Tri(L) = \SIZE");
            //for (i = 0; i < 3; i++) {
            //    for (j = 0; j <= i; j++)
            //        printf("%2.8lf\t", A[IDX(i, j, SIZE)]);
            //    printf("\SIZE");
            //}
        }

        free(A);
    } else {
        printf("%s <event>\n", argv[0]);
    }
	return 0;
}

