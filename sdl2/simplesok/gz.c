/*
 * gzBMP - a SDL2 helper that provides support for gziped BMP files.
 * Copyright (C) Mateusz Viste 2014
 */

#include "tinfl.c"
#include "gz.h" /* include self for control */

#define GZ_FLAG_ASCII 1
#define GZ_FLAG_MULTIPART_CONTINUTATION 2
#define GZ_FLAG_EXTRA_FIELD_PRESENT 4
#define GZ_FLAG_ORIG_FILENAME_PRESENT 8
#define GZ_FLAG_FILE_COMMENT_PRESENT 16
#define GZ_FLAG_FILE_IS_ENCRYPTED 32

/* tests a memory chunk to see if it contains valid GZ or not. returns 1 if the GZ seems legit. 0 otherwise. */
int isGz(unsigned char *memgz, long memgzlen) {
  int gzpos = 0, flags = 0;
  if (memgzlen < 16) return(0); /* must be at least 16 bytes long */
  if ((memgz[gzpos++] != 0x1F) || (memgz[gzpos++] != 0x8B)) return(0); /* check magic sig */
  if (memgz[memgzlen - 4] + memgz[memgzlen - 3] + memgz[memgzlen - 2] + memgz[memgzlen - 1] == 0) return(0); /* uncompressed file's len must be > 0 */
  if ((memgz[gzpos] != 0) && (memgz[gzpos++] != 8)) return(0);  /* compression method should be 'store' (0) or 'deflate' (8) */
  flags = memgz[gzpos++]; /* load flags (1 byte) */
  gzpos += 6; /* Discard the file modification timestamp (4 bytes), the extra flags (1 byte) as well as OS type (1 byte) */
  /* skip the extra field (if present) */
  if (flags & GZ_FLAG_EXTRA_FIELD_PRESENT) {
    int extrafieldlen;
    /* load the length of the extra field (2 bytes) */
    extrafieldlen = memgz[gzpos++];
    extrafieldlen <<= 8;
    extrafieldlen |= memgz[gzpos++];
    /* skip the extra field */
    gzpos += extrafieldlen;
  }
  /* skip the filename, if present (null terminated string) */
  if (flags & GZ_FLAG_ORIG_FILENAME_PRESENT) {
    for (;;) {
      if (gzpos >= memgzlen) return(0);
      if (memgz[gzpos++] == 0) break;
    }
  }
  /* skip the file comment, if present (null terminated string) */
  if (flags & GZ_FLAG_FILE_COMMENT_PRESENT) {
    for (;;) {
      if (gzpos >= memgzlen) return(0);
      if (memgz[gzpos++] == 0) break;
    }
  }
  /* seems legit */
  return(1);
}

/* decompress a gz file in memory. returns a pointer to a newly allocated memory chunk (holding uncompressed data), or NULL on error. */
unsigned char *ungz(unsigned char *memgz, long memgzlen, long *resultlen) {
  #define buffinsize 64 * 1024   /* the input buffer must be at least 32K, because that's the (usual) dic size in deflate, apparently */
  #define buffoutsize 256 * 1024 /* it's better for the output buffer to be significantly larger than the input buffer (we are decompressing here, remember? */
  unsigned char *buffout;
  unsigned char *buffin;
  unsigned char compmethod;
  int extract_res, flags;
  unsigned char *result;
  long filelen, compressedfilelen, gzpos = 0, resultpos = 0;

  *resultlen = 0;

  /* Check the magic bytes of the gz stream before starting anything */
  if (memgz[gzpos++] != 0x1F) return(NULL);
  if (memgz[gzpos++] != 0x8B) return(NULL);

  /* allocate buffers for data I/O */
  buffin = malloc(buffinsize);
  buffout = malloc(buffoutsize);
  if ((buffin == NULL) || (buffout == NULL)) {
    if (buffin != NULL) free(buffin);
    if (buffout != NULL) free(buffout);
    return(NULL);
  }

  /* read the uncompressed file length */
  buffin[0] = memgz[memgzlen - 4];
  buffin[1] = memgz[memgzlen - 3];
  buffin[2] = memgz[memgzlen - 2];
  buffin[3] = memgz[memgzlen - 1];
  filelen = buffin[0] | buffin[1] << 8 | buffin[2] << 16 | buffin[3] << 24;

  /* load the compression method (1 byte) - should be 0 (stored) or 8 (deflate) */
  compmethod = memgz[gzpos++];
  if ((compmethod != 0) && (compmethod != 8)) return(NULL);

  /* load flags (1 byte) */
  flags = memgz[gzpos++];

  /* check that the file is not a continuation of a multipart gzip */
  if (flags & GZ_FLAG_MULTIPART_CONTINUTATION) return(NULL);

  /* check that the file is not encrypted */
  if (flags & GZ_FLAG_FILE_IS_ENCRYPTED) return(NULL);

  /* Discard the file modification timestamp (4 bytes), the extra flags (1 byte) as well as OS type (1 byte) */
  gzpos += 6;

  /* skip the extra field (if present) */
  if (flags & GZ_FLAG_EXTRA_FIELD_PRESENT) {
    int extrafieldlen;
    /* load the length of the extra field (2 bytes) */
    extrafieldlen = memgz[gzpos++];
    extrafieldlen <<= 8;
    extrafieldlen |= memgz[gzpos++];
    /* skip the extra field */
    gzpos += extrafieldlen;
  }

  /* skip the filename, if present (null terminated string) */
  if (flags & GZ_FLAG_ORIG_FILENAME_PRESENT) {
    for (;;) if (memgz[gzpos++] == 0) break;
  }

  /* skip the file comment, if present (null terminated string) */
  if (flags & GZ_FLAG_FILE_COMMENT_PRESENT) {
    for (;;) if (memgz[gzpos++] == 0) break;
  }

  /* compute the length of the compressed stream */
  compressedfilelen = memgzlen - (gzpos + 8);

  /* allocate memory for uncompressed content */
  result = malloc(filelen + 1);
  if (result == NULL) return(NULL);  /* failed to open the dst file */
  result[filelen] = 0; /* finish the last byte with zero. just in case. */

  /* start reading and uncompressing the compressed data, computing CRC32 at the same time */
  if (compmethod == 0) { /* if the file is stored, copy it over */
      extract_res = 0;   /* assume we will succeed */
      for (resultpos = 0; resultpos < filelen;) {
        result[resultpos] = memgz[gzpos++];
      }
    } else if (compmethod == 8) { /* the file is deflated */
      size_t total_in = 0, total_out = 0;
      unsigned int infile_remaining = compressedfilelen;
      size_t avail_in = 0;
      tinfl_decompressor *tinflhandler;
      size_t avail_out = buffoutsize;
      void *next_out = buffout;
      const void *next_in = buffin;

      extract_res = 0; /* assume we will succeed */
      tinflhandler = malloc(sizeof(tinfl_decompressor));
      if (tinflhandler == NULL) {
          extract_res = -19;
        } else {
          tinfl_init(tinflhandler);
      }

      while (extract_res == 0) {
         size_t in_bytes, out_bytes;
         tinfl_status status;
         if (!avail_in) {
            /* Input buffer is empty, so read more bytes from input file. */
            unsigned int n = buffinsize;
            if (n > infile_remaining) n = infile_remaining;
            memcpy(buffin, &memgz[gzpos], n);
            gzpos += n;
            next_in = buffin;
            avail_in = n;
            infile_remaining -= n;
         }

         in_bytes = avail_in;
         out_bytes = avail_out;
         status = tinfl_decompress(tinflhandler, (const mz_uint8 *)next_in, &in_bytes, buffout, (mz_uint8 *)next_out, &out_bytes, (infile_remaining ? TINFL_FLAG_HAS_MORE_INPUT : 0));

         avail_in -= in_bytes;
         next_in = (const mz_uint8 *)next_in + in_bytes;
         total_in += in_bytes;

         avail_out -= out_bytes;
         next_out = (mz_uint8 *)next_out + out_bytes;
         total_out += out_bytes;

         if ((status <= TINFL_STATUS_DONE) || (!avail_out)) {
            /* Output buffer is full, or decompression is done, so write buffer to output file. */
            unsigned int n = buffoutsize - (unsigned int)avail_out;
            memcpy(&result[resultpos], buffout, n);
            resultpos += n;
            next_out = buffout;
            avail_out = buffoutsize;
         }

         /* If status is <= TINFL_STATUS_DONE then either decompression is done or something went wrong. */
         if (status <= TINFL_STATUS_DONE) {
            if (status == TINFL_STATUS_DONE) { /* Decompression completed successfully. */
                extract_res = 0;
              } else {  /* Decompression failed. */
                extract_res = -15;
            }
            break;
         }
      }
      if (tinflhandler != NULL) free(tinflhandler);
  }

  *resultlen = filelen;
  return(result);
}
