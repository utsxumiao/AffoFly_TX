// MemoryFree library based on code posted here:
// http://forum.arduino.cc/index.php?topic=27536.msg204024#msg204024
// Extended by Matthew Murdoch to include walking of the free list.

#ifndef MEMORY_FREE_H_
#define MEMORY_FREE_H_

#ifdef MEMORY_CHECK

#ifdef __cplusplus
extern "C" {
#endif

int freeMemory();

#ifdef  __cplusplus
}
#endif

#endif

#endif /* MEMORY_FREE_H_ */
