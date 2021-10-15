
/*-------------------------------------------------------*/
/*                                                       */
/*  MEMORY manager                                       */
/*  [c]copyright 1995 by AlphaHelix                      */
/*                                                       */
/*                                                       */
/*-------------------------------------------------------*/

/*
#include <malloc.h>
#include <iostream.h>

#include "ERROR.HPP"
#include "MEMORY.HPP"

#define MAXNEW          300

struct Entry {
   void     *ptr;       // Pointer to allocated block.
   int      size;       // Size of block in bytes.
};
static Entry   save[MAXNEW];
static int     nallocs = 0;
static int     mem_now = 0;   // Currently allocated.
static int     peak;          // Memory peak usage.

void *operator new(size_t size)
{
   void  *ptr;
   int   i;

   i = _heapchk();
   if ((i==_HEAPBADBEGIN) || (i==_HEAPBADNODE)) error("Heap Damaged");

   ptr = malloc(size);
   if (ptr == 0) error("Out of Memory");
   for (i = 0; i < MAXNEW; i++) {
      if (save[i].ptr == 0) {
         save[i].ptr = ptr;
         save[i].size = size;
         mem_now += size;
         if (mem_now > peak) peak = mem_now;
         break;
      }
   }
   if (i >= MAXNEW) error("Out of pointer register space");
   nallocs++;
   return ptr;
}

void operator delete(void *ptr)
{
   int   i;

   for (i = 0; i < MAXNEW; i++) {
      if (save[i].ptr == ptr) {
         save[i].ptr = 0;
         mem_now -= save[i].size;
         break;
      }
   }
   if (i >= MAXNEW) error("Tried to delete unregistered memory");
   nallocs--;
   free(ptr);
}

void memory_usage(void)
{
   int   i;

   cout << "Memory peak usage: " << peak/1024 << "kB." << endl;
   if (nallocs > 0) {
      cout << "Unfreed memory block detected." << endl;
      for (i = 0; i < MAXNEW; i++) {
         if (save[i].ptr != 0) {
            cout <<"   entry "<<i<<". size "<<save[i].size<<" bytes."<<endl;
         }
      }
   }
}
*/

void _memory_dummy(void)
{
   ;
}

