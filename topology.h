#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#define RIGHT(iProc) ((iProc + 1 + nProc) % nProc)
#define LEFT(iProc)  ((iProc - 1 + nProc) % nProc)

#endif
