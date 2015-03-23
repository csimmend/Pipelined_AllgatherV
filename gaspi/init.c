#include "init.h"
#include "success_or_die.h"

void reset_notifications_gaspi(gaspi_segment_id_t segment_id
			       , int *num_recv
			       , int iProc
			       , int nProc
			       )
{
  int i;
  // reset num_recv
  for (i = 0; i < nProc; i++) 
    {
      if (i != iProc)
	{
	  num_recv[i] = -1;
	}
    }
  
  // notification init
  for (i = 0; i < 2*nProc; i++) 
    {
      gaspi_notification_t value;
      SUCCESS_OR_DIE (gaspi_notify_reset (segment_id
					  , (gaspi_notification_id_t) i
					  , &value
					  ));
    }
}

