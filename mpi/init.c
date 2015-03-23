#include "constant.h"

void recv_init_mpi(double *recv_array
		   , int *offsets
		   , int nProc
		   )
{
  int j, k;

  // recv allgatherv, out of place
  for (k = 0; k < nProc; k++) 	
    {
      for (j = 0; j < M_SZ; j++)
	{
	  recv_array[j+k*M_SZ] = (double) -1;
	}
    }
  
  // offsets allgatherv
  for (k = 0; k < nProc; k++) 	
    {
      offsets[k] = k*M_SZ;
    }


}


