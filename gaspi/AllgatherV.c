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
#include "success_or_die.h"
#include "queue.h"
#include "topology.h"

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>


int main (int argc, char *argv[])
{
  int nProc, iProc;
  int provided, required = MPI_THREAD_FUNNELED;
  MPI_Init_thread(&argc, &argv, required, &provided);
  ASSERT(required == MPI_THREAD_FUNNELED);

  MPI_Comm_rank (MPI_COMM_WORLD, &iProc);
  MPI_Comm_size (MPI_COMM_WORLD, &nProc);

  gaspi_rank_t iProcG, nProcG;
  SUCCESS_OR_DIE (gaspi_proc_init (GASPI_BLOCK));
  SUCCESS_OR_DIE (gaspi_proc_rank (&iProcG));
  SUCCESS_OR_DIE (gaspi_proc_num (&nProcG));
  ASSERT(iProc == iProcG);
  ASSERT(nProc == nProcG);


  // allocate segments for source, work
  gaspi_segment_id_t const segment_id = 0;
  SUCCESS_OR_DIE ( gaspi_segment_create
                   ( segment_id
                     , M_SZ * nProc * sizeof (double)
                     , GASPI_GROUP_ALL
                     , GASPI_BLOCK
                     , GASPI_MEM_UNINITIALIZED
                     ));
  gaspi_pointer_t source_array;
  SUCCESS_OR_DIE ( gaspi_segment_ptr (segment_id, &source_array) );
  ASSERT (source_array != 0);

  int *array_len = malloc(nProc * sizeof (int));

  MPI_Barrier(MPI_COMM_WORLD);

  int iter;
  double median[NITER];
  gaspi_queue_id_t queue_id = 0;

  for (iter = 0; iter < NITER; iter++) 
    {
      gaspi_rank_t left = LEFT(iProc);
      gaspi_rank_t right = RIGHT(iProc);
      int i;

      // notification init
      for (i = 0; i < 2*nProc; i++) 
	{
	  gaspi_notification_t value;
	  SUCCESS_OR_DIE (gaspi_notify_reset (segment_id
					      , (gaspi_notification_id_t) i
					      , &value
					      ));
	}

      // data init
      data_init(source_array
		, array_len
		, iProc
		, nProc
		);

      double time = -now();
      MPI_Barrier(MPI_COMM_WORLD);

      // initial write to left
      if (left != iProc) 
	{
	  const int len = array_len[iProc] * sizeof(double); 
	  const gaspi_notification_id_t data_available = iProc; // left going
	  swap_queue_and_wait(&queue_id);
	  SUCCESS_OR_DIE ( gaspi_write_notify
			   ( segment_id
			     , array_OFFSET (0, iProc)
			     , left
			     , segment_id
			     , array_OFFSET (0, iProc)
			     , len
			     , data_available
			     , array_len[iProc] // array_len == notification val  
			     , queue_id
			     , GASPI_BLOCK
			     ));
	}

      // initial write to right
      if (right != iProc) 
	{
	  const int len = array_len[iProc] * sizeof(double); 
	  const gaspi_notification_id_t data_available = iProc + nProc; // right going
	  swap_queue_and_wait(&queue_id);
	  SUCCESS_OR_DIE ( gaspi_write_notify
			   ( segment_id
			     , array_OFFSET (0, iProc)
			     , right
			     , segment_id
			     , array_OFFSET (0, iProc)
			     , len
			     , data_available
			     , array_len[iProc] // array_len == notification val  
			     , queue_id
			     , GASPI_BLOCK
			     ));
	}

      // GASPI allgather - bidirectional linear dataflow
      int fnl = 0;
      do
	{
	  gaspi_notification_id_t id;
	  SUCCESS_OR_DIE(gaspi_notify_waitsome (segment_id
						, 0
						, 2*nProc
						, &id
						, GASPI_BLOCK
						));
	  gaspi_notification_t value;
	  SUCCESS_OR_DIE (gaspi_notify_reset (segment_id
					      , id
					      , &value
					      ));
	  int to, array_id;
	  if (id >= nProc)
	    {
	      array_id = id - nProc ;
	      to = right;
	    }
	  else
	    {
	      array_id = id;
	      to = left;
	    }

	  // we might receive some messages twice (!!!) 
	  // (but we don't care)
	  if (array_len[array_id] == -1)
	    {
	      // set array_len
	      array_len[array_id] = value;
	      // local recv counter
	      fnl++;
	    }

	  // resend unless this is a return to sender
	  if (array_id != to) 
	    {
	      const int len = array_len[array_id] * sizeof(double); 
	      swap_queue_and_wait(&queue_id);
	      SUCCESS_OR_DIE ( gaspi_write_notify
			       ( segment_id
				 , array_OFFSET (0, array_id)
				 , to
				 , segment_id
				 , array_OFFSET (0, array_id)
				 , len
				 , id // use old id
				 , array_len[array_id]
				 , queue_id
				 , GASPI_BLOCK
				 ));
	    }
	}
      while (fnl < nProc-1);

      // flush queues, reset stale notification counters
      wait_for_flush_queues();

      MPI_Barrier(MPI_COMM_WORLD);
      time += now();
      
      /* iteration time */
      median[iter] = time;

    }
  

  MPI_Barrier(MPI_COMM_WORLD);


  // validate */ 
  data_validate(source_array
		, nProc
		);

  MPI_Barrier(MPI_COMM_WORLD);

  sort_median(&median[0], &median[NITER-1]);

  printf ("# gaspi %s nProc: %d M_SZ: %d niter: %d time: %g\n"
	  , argv[0], nProc, M_SZ, NITER, median[NITER/2]
         );

  MPI_Barrier(MPI_COMM_WORLD);
 

  MPI_Finalize();


  return EXIT_SUCCESS;

}
