/*-------------------------------------------------------*/
/*                                                       */
/* Disk IO module.                                       */
/* [c]copyright 1996 by AlphaHelix                       */
/*                                                       */
/* This module doesn't cache or buffer data in any way.  */
/* It solely relies on SMARTDRIVE or similar software    */
/* to efficiently cache data.                            */
/*                                                       */
/*-------------------------------------------------------*/

#include <string.h>
#include <iostream>

#include "COMPATIBILITY.HPP"
#include "ERROR.HPP"
#include "MEMORY.HPP"
#include "DISKIO.HPP"
#include <Windows.h>

using namespace std;

#define MAXPOOLS     5        // Max open filepools.

// Some static variables.
static struct {
   int      active;
   FilePool *pool;
} now[MAXPOOLS];

static char curDIR[255];



/*---------------------------------------------------------
 Function: class myFile

 Description:
---------------------------------------------------------*/

myFile::myFile()
{
   active = 0;
	memory = NULL;
}

int myFile::open(const char *f)
{
   FILE  *s;
   File  *file = 0;
   int   i;

// First have closer look at the active FilePools.
   for (i = 0; i < MAXPOOLS; i++) {
      if (now[i].active) {
         if ((file = now[i].pool->findfile(f))) break;
      }
   }
   if (file) {
// File is in the FilePool.
      active = 1;
      realfile = 0;
      stream = now[i].pool->stream;
      base = file->fptr;
      size = file->size;
      fseek(stream, base, SEEK_SET);
   } else {
      s = fopen(f, "rb");
      if (!s) return 0;
// Stroke. File is on disk. Prepare myfile.
      active = 1;
      realfile = 1;
      stream = s;
      base = 0;
      fseek(stream, 0, SEEK_END);
      size = ftell(stream);
      fseek(stream, 0, SEEK_SET);
   }
   return 1;
}

int myFile::creat(char *f)
{
   FILE  *s;

   s = fopen(f, "wb");
   if (!s) return 0;
// Yeah. It really works.
   active = 1;
   realfile = 1;
   stream = s;
   base = 0;
   size = 0;

   return 1;
}

void myFile::close(void)
{
   if (active && realfile) fclose(stream);
	if (memory != NULL) free(memory);

	memory = NULL;
   active = 0;
}

void myFile::read(void *ptr, int size)
{
   fread(ptr, size, 1, stream);
}

void myFile::write(void *ptr, int size)
{
   fwrite(ptr, size, 1, stream);
}

void *myFile::getMemory(void)
{
	long oPos = ftell(stream);
	int sizeRemaining = getsize() - gettell();

	if(memory == NULL)
	{
		memory = malloc(sizeRemaining);
		read(memory, sizeRemaining);
	}

	fseek(stream, oPos, SEEK_SET); // Revert back to original seek position.

	return memory;
}



void *loadfile(const char *f, int *size)
{
   myFile   myfile;
   void     *ptr;

   if (!myfile.open(f)) error("file not found (loadfile): ", f);

	if(size) *size = myfile.getsize();

   ptr = new char[myfile.getsize()];
   myfile.read(ptr, myfile.getsize());
   myfile.close();

   return ptr;
}

void unloadfile(void *p)
{
   delete []p;
}




//-------------------------------------//
// class FilePool                      //
//-------------------------------------//
FilePool::FilePool()
{
   stream = 0;
}

void FilePool::open(char *f)
{
   int   i;

// Try to open database.
   stream = fopen(f, "rb");
   if (!stream) error("Heap of files not found: ", f);
// Read header.
   fread(hdr, HDRSIZE, 1, stream);
   fread(&flags, sizeof(flags), 1, stream);
   fread(&n, sizeof(n), 1, stream);
// Read file header data.
   file = new File[n];
   fread(file, sizeof(File), n, stream);
// Add this base to the list of active datapools.
   for (i = 0; i < MAXPOOLS; i++) {
      if (!now[i].active) break;
   }
   if (i >= MAXPOOLS) error("Too many open databases.");
   now[i].active = 1;
   now[i].pool = this;
}

void FilePool::close(void)
{
   int   i;

   if (stream) {
      delete []file;
      fclose(stream);
   }
// remove pool from list of active pools.
   for (i = 0; i < MAXPOOLS; i++) {
      if (now[i].pool == this) now[i].active = 0;
   }
   stream = 0;
}

File *FilePool::findfile(const char *f)
{
   int   i;

   for (i = 0; i < n; i++) {
      if (strcmpi(f, file[i].name) == 0) break;
   }
   if (i >= n) return 0;

   return &file[i];
}

//-----------------------------------------------


void initfilesystem(void)
{
   int   i;

   #ifdef WINDOWS
   GetCurrentDirectory(254, curDIR);
   #else
   getcwd(curDIR, 254);
   #endif // WINDOWS

   cout << "Current directory: " << curDIR << endl;

   for (i = 0; i < MAXPOOLS; i++) {
      now[i].active = 0;
   }
}

void shutfilesystem(void)
{
   return;
}


