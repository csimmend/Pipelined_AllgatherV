#include "queue.h"
#include "success_or_die.h"



void swap_queue_and_wait(gaspi_queue_id_t* queue)
{
  gaspi_number_t queue_size_max;
  gaspi_number_t queue_size;
  gaspi_number_t queue_num;

  SUCCESS_OR_DIE (gaspi_queue_num (&queue_num));
  SUCCESS_OR_DIE (gaspi_queue_size_max (&queue_size_max));

  // swap queue
  *queue = (*queue + 1) % queue_num;

  SUCCESS_OR_DIE (gaspi_queue_size (*queue, &queue_size));
  if (queue_size >= queue_size_max - 2)
    {
      SUCCESS_OR_DIE (gaspi_wait (*queue, GASPI_BLOCK));
    }

}

void wait_for_flush_queues ()
{
  gaspi_number_t queue_num;

  SUCCESS_OR_DIE (gaspi_queue_num (&queue_num));

  gaspi_queue_id_t queue = 0;
 
  while( queue < queue_num )
  {
    SUCCESS_OR_DIE (gaspi_wait (queue, GASPI_BLOCK));
    ++queue;
  }
}
