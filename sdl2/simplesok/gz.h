#ifndef gz_h_sentinel
#define gz_h_sentinel
  unsigned char *ungz(unsigned char *memgz, long memgzlen, long *resultlen);
  int isGz(unsigned char *memgz, long memgzlen);
#endif
