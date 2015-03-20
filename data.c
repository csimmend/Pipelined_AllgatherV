#include "data.h"
#include "assert.h"
#include "math.h"

void data_init(double *source_array
	       , int *array_len
	       , int iProc
	       , int nProc
	       )
{
  int j, k;
  for (k = 0; k < nProc; k++) 	
    {
      array_len[k] = -1;
      for (j = 0; j < M_SZ; j++)
	{
	  source_array[j+k*M_SZ] = (double) -1;
	}
    }

  // paint my array part
  array_len[iProc] = M_SZ;
  for (j = 0; j < M_SZ; j++)
    {
      source_array[j+iProc*M_SZ] = (double) iProc;
    }

}

void data_validate(double *source_array
		   , int nProc
		   )
{
  int j, k;
  for (k = 0; k < nProc; k++) 	
    {
      for (j = 0; j < M_SZ; j++)
	{
	  ASSERT(source_array[j+k*M_SZ] == (double) (k));
	}
    }
}


static void swap(double *a, double *b)
{
  double tmp = *a;
  *a = *b;
  *b = tmp;
}

void sort_median(double *begin, double *end)
{
  double *ptr;
  double *split;
  if (end - begin <= 1)
    return;
  ptr = begin;
  split = begin + 1;
  while (++ptr != end) {
    if (*ptr < *begin) {
      swap(ptr, split);
      ++split;
    }
  }
  swap(begin, split - 1);
  sort_median(begin, split - 1);
  sort_median(split, end);
}

