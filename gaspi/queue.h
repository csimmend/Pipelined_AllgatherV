#ifndef QUEUE_H
#define QUEUE_H

#include <GASPI.h>

void swap_queue_and_wait(gaspi_queue_id_t* queue_id);
void wait_for_flush_queues();

#endif
