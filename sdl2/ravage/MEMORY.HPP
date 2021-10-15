
#include <stdlib.h>

#include "ERROR.HPP"


// My own little fixed length memory management system. Hey: TEMPLATES. Wow.
//#pragma pack(4)
template <class T>
struct Block {
   int      occ;           // Memory block is occupied.
   char     data[(sizeof(T)+3) & 0xfffffffc];
};

template <class T>
class Memory {
   int      initialized;      // Memory system initialized ?
   int      n;                // Number of data blocks (occupied+available).
   int      nfree;            // Free blocks.
   int      last;             // Index of last allocated block.
   int      minfree;          // Statistics: Peak block usage.
   Block<T> *base;            // Pointer to base data block.
   Block<T> *l;               // Pointer to last allocated block.

public:
   Memory() { initialized = 0; }
   Memory(int maxelements) { init(maxelements); }
   ~Memory() { shut(); }
   void  init(int);
   void  shut(void)
   {
      if (initialized) { delete []base; initialized = 0; }
   }
   T     *alloc(size_t);
   void  free(T *p)
   {
      Block<T> *ptr;

      ptr = (Block<T> *)p;
      ptr = (Block<T> *)((char *)p - (int)((char *)(&ptr->data)-(char *)ptr));
      ptr->occ = 0;
      nfree++;
   }
   int   usage(void)
   {
      return n - minfree;
   }
};

// Initialize memory system.
template <class T>
void Memory<T>::init(int maxelements)
{
   int   i;

   minfree = nfree = n = maxelements;
   last = 0;
   l = base = new Block<T>[n];
   for (i = 0; i < n; i++) {
      base[i].occ = 0;
   }
   initialized = 1;
}

template <class T>
T *Memory<T>::alloc(size_t s)
{
   if (!initialized) error("(Memory::alloc): system not initialized.", NULL);
   if (s != sizeof(T)) error("(Memory::alloc): wrong block size.", NULL);
   if (nfree <= 0) error("(Memory::alloc): no free blocks.", NULL);
   do {
      if (++last >= n) {
         last = 0; l = base;
      } else {
         l++;
      };
   } while (l->occ);
   if (--nfree < minfree) minfree--;
   l->occ = 1;

   return (T *)l->data;
}


