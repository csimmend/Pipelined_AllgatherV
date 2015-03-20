/*
 * This file is part of a small series of tutorial,
 * which aims to demonstrate key features of the GASPI
 * standard by means of small but expandable examples.
 * Conceptually the tutorial follows a MPI course
 * developed by EPCC and HLRS.
 *
 * Contact point for the MPI tutorial:
 *                 rabenseifner@hlrs.de
 * Contact point for the GASPI tutorial:
 *                 daniel.gruenewald@itwm.fraunhofer.de
 *                 mirko.rahn@itwm.fraunhofer.de
 *                 christian.simmendinger@t-systems.com
 */

#include "assert.h"
#include "constant.h"
#include "data.h"
#include "now.h"
#include "topology.h"

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>


int main (int argc, char *argv[])
{
  int j, k;
  int nProc, iProc;
  int provided, required = MPI_THREAD_FUNNELED;
  MPI_Init_thread(&argc, &argv, required, &provided);
  ASSERT(required == MPI_THREAD_FUNNELED);

  MPI_Comm_rank (MPI_COMM_WORLD, &iProc);
  MPI_Comm_size (MPI_COMM_WORLD, &nProc);

  // MPI -- out of place Allgather
  double *source_array = malloc(M_SZ * nProc * sizeof (double));
  ASSERT (source_array != 0);
  double *recv_array = malloc(M_SZ * nProc * sizeof (double));
  ASSERT (recv_array != 0);
  int *array_len = malloc(nProc * sizeof (int));
  ASSERT (array_len != 0);

  MPI_Barrier(MPI_COMM_WORLD);

  int iter;
  double median[NITER];
  for (iter = 0; iter < NITER; iter++) 
    {
      // recv init
      for (k = 0; k < nProc; k++) 	
	{
	  for (j = 0; j < M_SZ; j++)
	    {
	      recv_array[j+k*M_SZ] = (double) -1;
	    }
	}

      // data init
      data_init(source_array
		, array_len
		, iProc
		, nProc
		);

      double time = -now();
      MPI_Barrier(MPI_COMM_WORLD);

      const int len = array_len[iProc]; 
      MPI_Allgather(&source_array[iProc*M_SZ],
		    len,
		    MPI_DOUBLE,
		    &recv_array[0],
		    len,
		    MPI_DOUBLE,
		    MPI_COMM_WORLD
		    );
     
      MPI_Barrier(MPI_COMM_WORLD);
      time += now();
      
      /* iteration time */
      median[iter] = time;

    }
  
  MPI_Barrier(MPI_COMM_WORLD);

  // validate */ 
  data_validate(recv_array
		, nProc
		);

  MPI_Barrier(MPI_COMM_WORLD);

  sort_median(&median[0], &median[NITER-1]);

  printf ("# mpi %s nProc: %d M_SZ: %d niter: %d time: %g\n"
	  , argv[0], nProc, M_SZ, NITER, median[NITER/2]
         );

  MPI_Barrier(MPI_COMM_WORLD);
 

  MPI_Finalize();


  return EXIT_SUCCESS;

}
