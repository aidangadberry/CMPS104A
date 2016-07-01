// $Id: auxlib.cpp,v 1.3 2015-10-01 18:15:04-07 - - $

#include <assert.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>

#include "auxlib.h"

static int exitstatus = EXIT_SUCCESS;
static const char* execname = NULL;
static const char* debugflags = "";
static bool alldebugflags = false;

void set_execname (char* argv0) {
    execname = basename (argv0);
}

const char* get_execname (void) {
    assert (execname != NULL);
    return execname;
}

static void eprint_signal (const char* kind, int signal) {
    eprintf (", %s %d", kind, signal);
    const char* sigstr = strsignal (signal);
    if (sigstr != NULL) fprintf (stderr, " %s", sigstr);
}

void eprint_status (const char* command, int status) {
    if (status == 0) return; 
    eprintf ("%s: status 0x%04X", command, status);
    if (WIFEXITED (status)) {
        eprintf (", exit %d", WEXITSTATUS (status));
    }
    if (WIFSIGNALED (status)) {
        eprint_signal ("Terminated", WTERMSIG (status));
        #ifdef WCOREDUMP
        if (WCOREDUMP (status)) eprintf (", core dumped");
        #endif
    }
    if (WIFSTOPPED (status)) {
        eprint_signal ("Stopped", WSTOPSIG (status));
    }
    if (WIFCONTINUED (status)) {
        eprintf (", Continued");
    }
    eprintf ("\n");
}

void veprintf (const char* format, va_list args) {
    assert (execname != NULL);
    assert (format != NULL);
    fflush (NULL);
    if (strstr (format, "%:") == format) {
        fprintf (stderr, "%s: ", get_execname ());
        format += 2;
    }
    vfprintf (stderr, format, args);
    fflush (NULL);
}

void eprintf (const char* format, ...) {
    va_list args;
    va_start (args, format);
    veprintf (format, args);
    va_end (args);
}

void errprintf (const char* format, ...) {
    va_list args;
    va_start (args, format);
    veprintf (format, args);
    va_end (args);
    exitstatus = EXIT_FAILURE;
}

void syserrprintf (const char* object) {
    errprintf ("%:%s: %s\n", object, strerror (errno));
}

int get_exitstatus (void) {
    return exitstatus;
}

void set_exitstatus (int newexitstatus) {
    if (exitstatus < newexitstatus) exitstatus = newexitstatus;
    DEBUGF ('x', "exitstatus = %d\n", exitstatus);
}

void __stubprintf (const char* file, int line, const char* func,
                   const char* format, ...) {
    va_list args;
    fflush (NULL);
    printf ("%s: %s[%d] %s: ", execname, file, line, func);
    va_start (args, format);
    vprintf (format, args);
    va_end (args);
    fflush (NULL);
}     

void set_debugflags (const char* flags) {
    debugflags = flags;
    if (strchr (debugflags, '@') != NULL) alldebugflags = true;
    DEBUGF ('x', "Debugflags = \"%s\", all = %d\n",
           debugflags, alldebugflags);
}

bool is_debugflag (char flag) {
    return alldebugflags or strchr (debugflags, flag) != NULL;
}

void __debugprintf (char flag, const char* file, int line,
                    const char* func, const char* format, ...) {
    va_list args;
    if (not is_debugflag (flag)) return;
    fflush (NULL);
    va_start (args, format);
    fprintf (stderr, "DEBUGF(%c): %s[%d] %s():\n",
             flag, file, line, func);
    vfprintf (stderr, format, args);
    va_end (args);
    fflush (NULL);
}
