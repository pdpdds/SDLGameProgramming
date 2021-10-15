
#include <iostream>
#include <string.h>

#define AH_ERROR
#include "ERROR.HPP"

using namespace std;

// Local variables.
static char    *err_text;
static char    err_arg0[80];

#define MAXREGS      20       // Max number of registered functions.

static int     nregs = 0;     // Number of registered exit functions.
static void    (*reg_func[MAXREGS])(void);

static int     enabled = 1;   // Enable error processing.



void reg_exit(void (*f)(void))
{
   if (nregs < MAXREGS) reg_func[nregs++] = f;
}

void error(const char *text1, const char *text2)
{
   if (enabled) {
// Set error variables for postprocessing.
      err_text = (char *)text1;
      if (text2) strcpy(err_arg0, text2); else err_arg0[0] = 0;
// WARP speed, Mr. Sulu.
      longjmp(_err_jmp, 1);
   } else {
// No program abort wanted.
      _errors++;
   }
}

void err_enable(void)
{
   enabled = 1;
}

void err_disable(void)
{
   enabled = 0;
   _errors = 0;
}

void reg_down(void)
{
   int   i;

// Execute registered functions.
   for (i = nregs-1; i >= 0; i--) reg_func[i]();
}

void post_error(void)
{
// Print error message.
   cout << err_text << err_arg0 << endl;
}


