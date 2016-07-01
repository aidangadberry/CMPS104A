// $Id: auxlib.h,v 1.2 2015-10-01 18:15:04-07 - - $

#ifndef __AUXLIB_H__
#define __AUXLIB_H__

#include <stdarg.h>

//
// DESCRIPTION
//    Auxiliary library containing miscellaneous useful things.
//

//
// Error message and exit status utility.
//

void set_execname (char* argv0);
//
// Sets the program name for use by auxlib messages.
// Must called from main before anything else is done,
// passing in argv[0].
//

const char* get_execname (void);
//
// Returns a read-only value previously set by set_progname.
//

void eprint_status (const char* command, int status);
//
// Print the status returned by wait(2) from a subprocess.
//

int get_exitstatus (void);
//
// Returns the exit status.  Default is EXIT_SUCCESS unless
// set_exitstatus (int) is called.  The last statement in main
// should be:  ``return get_exitstatus();''.
//

void set_exitstatus (int);
//
// Sets the exit status.  Remebers only the largest value.
//

void veprintf (const char* format, va_list args);
//
// Prints a message to stderr using the vector form of 
// argument list.
//

void eprintf (const char* format, ...);
//
// Print a message to stderr according to the printf format
// specified.  Usually called for debug output.
// Precedes the message by the program name if the format
// begins with the characters `%:'.
//

void errprintf (const char* format, ...);
//
// Print an error message according to the printf format
// specified, using eprintf.  Sets the exitstatus to EXIT_FAILURE.
//

void syserrprintf (const char* object);
//
// Print a message resulting from a bad system call.  The
// object is the name of the object causing the problem and
// the reason is taken from the external variable errno.
// Sets the exit status to EXIT_FAILURE.
//

//
// Support for stub messages.
//
#define STUBPRINTF(...) \
        __stubprintf (__FILE__, __LINE__, __func__, __VA_ARGS__)
void __stubprintf (const char* file, int line, const char* func,
                   const char* format, ...);

//
// Debugging utility.
//

void set_debugflags (const char* flags);
//
// Sets a string of debug flags to be used by DEBUGF statements.
// Uses the address of the string, and does not copy it, so it
// must not be dangling.  If a particular debug flag has been set,
// messages are printed.  The format is identical to printf format.
// The flag "@" turns on all flags.
//

bool is_debugflag (char flag);
//
// Checks to see if a debugflag is set.
//

#ifdef NDEBUG
// Do not generate any code.
#define DEBUGF(FLAG,...)   /**/
#define DEBUGSTMT(FLAG,STMTS) /**/
#else
// Generate debugging code.
void __debugprintf (char flag, const char* file, int line,
                    const char* func, const char* format, ...);
#define DEBUGF(FLAG,...) \
        __debugprintf (FLAG, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define DEBUGSTMT(FLAG,STMTS) \
        if (is_debugflag (FLAG)) { DEBUGF (FLAG, "\n"); STMTS }
#endif

#endif
