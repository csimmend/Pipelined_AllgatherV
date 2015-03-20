#ifndef DATA_H
#define DATA_H

#include "constant.h"

#define POSITION(i,j) ((i) + M_SZ * (j))
#define array_OFFSET(i,j) (POSITION (i,j) * sizeof(double))
#define source_array_ELEM(i,j) ((double *)source_array)[POSITION (i,j)]

void data_init(double *source_array
	       , int *array_len
	       , int iProc
	       , int nProc
	       );


void data_validate(double *source_array
		   , int nProc
		   );


void sort_median(double *begin, double *end);


#endif
