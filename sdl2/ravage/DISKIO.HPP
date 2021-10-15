
#ifndef AH_DISKIO
#define AH_DISKIO

#include <stdio.h>

#define HDRSIZE      80

class myFile {
   int   active;
   int   realfile;
   FILE  *stream;
	void *memory;						//Contents of file stored in memory
   int   base;
   int   size;
public:
	myFile();
   ~myFile() { close(); }
   int   open(const char *f);
   int   creat(char *f);
   void  close(void);
   void  read(void *ptr, int size);
   void  write(void *ptr, int size);
   FILE  *getFILE(void) { return stream; }
	void *getMemory(void);
   int   getbase(void) { return base; }
   int   getsize(void) { return size; }
	int	gettell(void) { return ftell(stream) - base; }
};

struct File {
   char     name[80];            // Name of the file (including directory).
   long     flags;               // Some flags.
   long     size;                // File size in bytes;
   long     fptr;                // File ptr to get access to data.
};

class FilePool {
   FILE  *stream;                // File handle to database file.
   char  hdr[HDRSIZE];           // Header string. (like "DATABASE VER 1.0")
   long  flags;
   long  n;                      // Number of files in database.
   File  *file;                  // File headers.

   File  *findfile(const char *f);
   friend class myFile;

public:
   FilePool();
   ~FilePool(void) { close(); }
   void open(char *f);
   void close(void);
};

void *loadfile(const char *f, int *size);
void unloadfile(void *);

void initfilesystem(void);
void shutfilesystem(void);

#endif

