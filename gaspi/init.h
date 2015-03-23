#ifndef INIT_H
#define INIT_H
#include <GASPI.h>
void reset_notifications_gaspi(gaspi_segment_id_t segment_id
			       , int *num_recv
			       , int iProc
			       , int nProc
			       );
#endif
