
#ifndef AH_ERROR_ONCE
#define AH_ERROR_ONCE

#include <setjmp.h>

#ifdef AH_ERROR
   #define _extrn
#else
   #define _extrn extern
#endif

_extrn jmp_buf _err_jmp;
_extrn int     _errors;       // Number of errors since last call to err_disable()

void reg_exit(void (*f)(void));
void error(const char *, const char * = NULL);
void err_disable(void);
void err_enable(void);
void reg_down(void);
void post_error(void);

#undef _extrn

#endif

