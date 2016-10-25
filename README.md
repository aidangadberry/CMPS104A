o Tmac.mm-etc
.if t .Newcentury-fonts
.INITR* \n[.F]
.SIZE 12 14
.TITLE CMPS-104A Fall\~2016 Project\~1 \
"String Set and Preprocessor"
.RCS "$Id: asg1-stringset.mm,v 1.15 2016-10-10 12:03:27-07 - - $"
.PWD
.URL
.GETST* HASH_OUTPUT Figure_HASH_OUTPUT
.EQ
delim $$
.EN
.de OPT=LI
.   LI "\f[CB]\\$[1]\f[R]\|\f[I]\\$2\f[R]"
..
.H 1 "Overview"
Write a main program for the language
.V= oc
that you will be compiling this quarter.
Also, include a string set ADT for it,
and make it preprocess the program using the C preprocessor,
.V= /usr/bin/cpp .
The main program will be called from Unix according the
usage given below under the synopsis.
This means that your compiler will read in a single
.V= oc
program,
possibly with some options,
as described below.
.P
The name of the compiler is
.V= oc
and the file extension for programs written in this language will be
.V= .oc
as well.
Option letters are given with the usual Unix syntax.
All debugging output should be printed to the standard error,
not the standard output.
Use the macros
.V= DEBUGF
and
.V= DEBUGSTMT
to generate debug output.
(See the example 
.V= expr-smc ,
module
.V= auxlib ).
.SH=BVL
.MANPAGE=LI SYNOPSIS
.V= oc
.=V \|[ -ly ]
.=V \|[ -@
.IR flag \|.\|.\|.]
.=V \|[ -D
.IR string ]
.IR \|program \f[CB].oc\f[P]
.MANPAGE=LI OPTIONS
.nr optPi \n[Pi]*2
.VL \n[optPi]
.OPT=LI -@ flags
Call
.V= set_debugflags ,
and use
.V= DEBUGF
and
.V= DEBUGSTMT
for debugging.
The details of the flags are at the implementor's discretion,
and are not documented here.
.OPT=LI -D string
Pass this option and its argument to
.V= cpp .
This is mostly useful as
.V= -D__OCLIB_OH__
to suppress inclusion of the code from
.V= oclib.oh
when testing a program.
.OPT=LI -l
Debug
.V= "yylex()"
with
.V= "yy_flex_debug = 1"
.OPT=LI -y
Debug
.V= "yyparse()"
with
.V= "yydebug = 1"
.LE
.P
.LE
Besides the debug options,
your compiler will always produce output files for each assignment.
Whenever your compiler is run for any particular project,
it must produce output files for the current project
and for all previous projects.
Note that since
.IR program
is in italics,
it indicates that you use the name specified in
.V= argv .
Your compiler will work on only one program per process,
but it will be run multiple times by the grader and each
run must produce a different set of output files.
.ds prog \f[I]program\f[P]
.DS I
.TS
tab(|); l l l.
asg1|write the string set to|\*[prog]\f[CB].str\f[P]
asg2|write each scanned token to|\*[prog]\f[CB].tok\f[P]
asg3|write the abstract syntax tree to|\*[prog]\f[CB].ast\f[P]
asg4|write the symbol table to|\*[prog]\f[CB].sym\f[P]
asg5|write the intermediate language to|\*[prog]\f[CB].oil\f[P]
.TE
.DE
.P
The first project will produce only the
.V= \.str
file.
The second project will produce both the 
.V= \.str
and
.V= \.tok
files.
Each subsequent project will produce the files of all previous
projects and also the one for the current project.
Do not open output files for projects later than the one you
are currently working on.
.P
The main program will analyze the
.V= argv
array as appropriate and set up the various option flags.
\f[I]program\f[P]\f[CB].str\f[P],
depending on the name of the program source file.
Created files are always in the current directory,
regardless of where the input files are found.
Use
.V= getopt (3)
to analyze the options and arguments.
.P
The suffix is always added to the basename of the argument filename.
See
.V= basename (1).
The basename is the argument with all directory names removed
and with the suffix (if any) removed.
The suffix is everything from the final period onward.
Be careful to not to strip off periods
in the directory part of the name.
An error is produced if the input filename suffix is not
.V= \&.oc ,
or if there is no suffix,
in which case the compilation aborts with a message.
.E= Note\(::
This means that your program must accept source files from a
directory that you do not own and for which you have no write
permission, yet produce output files in the
.E= current
directory.
.H 1 "Organization"
The main program will call a test harness for the string set
ADT.
The test harness will work as follows\(::
after filtering the input through the C preprocessor,
read a line using
.V= fgets (3),
and tokenize it using 
.V= strtok_r (3),
with the string
.V= \[Dq]\[VS]\[rs]t\[rs]n\[Dq] ,
i.e., spaces, tabls, and newline characters,
and insert it into the string set.
After that,
the main program will call the string set ADT opration to
dump the string set into its trace file.
See the example in the subdirectory
.V= cppstrtok 
for an illustration of how to call the C preprocessor.
Your program will not read the raw file,
only the output of
.V= cpp .
.P
Do not confuse the program
.V= cpp ,
which is the C preprocessor with the suffix
.V= \&.cpp ,
commonly used to indicate a C++ program,
compiled via the
.V= g++
compiler.
.P
The purpose of the string set is to keep tracks of strings
in a unique manner.
For example,
if the string
.V= \[Dq]abc\[Dq]
is entered multiple times, it appears only once in the table.
This means that instead of using
.V= strcmp (3)
to determine if two entries in the hash table are the same,
one can simply compare the pointers.
.P
This assignment does
.E= *not*
involve writing a scanner.
Your dummy scanner,
part of the main program,
will just use
.V= fgets (3)
to read in a line from the program file,
and use
.V= strtok_r (3)
to tokenize it,
and then enter the token into the hash table.
.P
.DF L
.SP
.B1
.SP
.ft CR
hash[   3]:  2586491021746226264 0x2067528->"teststring"
.br
            12271277041006505511 0x2067288->"main.o"
.br
hash[  13]: 18201842504327843073 0x2067198->"Makefile"
.br
load_factor = 0.522
.br
bucket_count = 23
.br
max_bucket_size = 2
.SP
.B2
.FG "Example of stringset dump" "" 0 HASH_OUTPUT
.DE
.H 1 "The String Set ADT"
The string set will operate as a hash table
and have the interface in a file called
.V= stringset.h 
and the implementation in
.V= stringset.cpp .
As you develop your program, other functions may be needed.
Following is the interface specification.
You may alter it in minor ways as needed if you find the interface
to be somewhat inconvenient.
.BVL \n[Pi]
.V=LI "const string* intern_stringset (const char*);"
Insert a new string into the hash set and return a pointer to
the string just inserted.
If it is already there, 
nothing is inserted,
and the previously-inserted string is returned.
.V=LI "void dump_stringset (FILE*);"
Dumps out the string set in debug format, which might look
as illustrated in Figure \*[Figure_HASH_OUTPUT].
In other words,
print the hash header number followed by spaces,
then the hash number and then the address of the string followed
by the string itself.
In the this example, the two strings in bucket 3
have collided.
.LE
.H 1 "Filenames"
The following project organization rules apply to everything you
submit in this course,
in order to ensure consistency across all projects,
and to make it easier for the grader to figure out what your compiler
is doing (or not doing).
You may use any development environment you wish.
However,
the production environment is that available under
.V= unix.ic .
As regards grading,
whether or not your program works on the development environment is not
relevant.
The grader will use only
.V= unix.ic
to test your programs.
Use the Solaris submit command to submit your work.
.P
Any special notes or comments you want to make that the grader
should read first must be in a file called 
.V= README .
Spell it in upper case.
The minimum
.V= README
should contain your personal name and username,
and that of your team partner, if any.
.P
Use of
.V= flex
for the scanner and
.V= bison
for the parser is required.
.P
Compile your hand-coded programs with
.VTCODE* 1 "g++ -g -O0 -Wall -Wextra -std=gnu++14"
and make sure that the programs are fixed so that no warning messages
are generated.
Compile the programs generated by
.V= flex
and
.V= bison
using whatever options will cause a silent compilation.
Also see
.V= Examples/e08.expr-sm/Makefile .
Run
.V= valgrind
frequently to check for uninitialized variables.
.P
You must submit a
.V= Makefile
which will build the executable image from submitted source code.
If the
.V= Makefile
does not work or if there are any errors in your source code,
the result of which is a compilation failure,
you lose all of the points for program testing.
.P
The executable image for the compiler you are writing must be
called
.=V `` oc ''.
Use appropriate source file suffixes\(::
.nr TWOPi \n[Pi]*2
.VL \n[TWOPi] \n[Pi] 1
.V=LI .l
for
.V= flex
grammars
.V=LI .y
for
.V= bison
input grammars
.V=LI .h
for header files
.V=LI .cpp
for C++ source code
.LE
.P
.H 1 "Makefile"
You must submit a
.V= Makefile 
with the following targets\(::
.nr BIGPi \n[Pi]*3
.VL \n[BIGPi] \n[Pi]
.V=LI all:
Build the executable image,
all necessary object files,
and any required intermediate files.
This must be the first target in the Makefile,
so that the Unix command 
.V= gmake
means
.V= "gmake all" .
.V=LI clean:
Delete object files and generated intermediate files such as are
produced by flex and bison.
Do not delete the executable image.
.V=LI spotless:
Depends on 
.V= clean
and deletes the executable image as well.
.V=LI ci:
Checks in all source files (but not generated files) into the
.V= RCS
subdirectory.
Or you may use
.V= SCCS ,
.V= CVS ,
.V= SVN ,
.V= Git ,
or some other archival system that you find convenient.
.V=LI deps:
Recreates the dependencies.
.LE
.H 1 "Use of C++"
It is assumed that everyone entering this course has a good
knowledge of the C programming language,
and of how to use generic data structures in Java.
While the prerequisite for the course is a knowledge of C
rather than C++,
it is still possible to code mainly in C if you prefer,
and just use a C++ compiler.
C++ is (mostly) a superset of C.
The advantage of C++ over C is its extensive libraries
which make coding significantly easier.
C++ also has somewhat better type checking than C.
.ALX a ()
.LI
.V= string
replaces
.V= char*
and
.V= char[] .
C requires significantly more careful memory management.
.LI
.V= vector<T>
replaces C-style arrays and has a
.V= push_back
function which allows arrays to expand.
Otherwise, in C,
a $n$-way tree would need to be implemented as a list of
children or as an explicitly managed raw array.
.LI
.V= unordered_set<T>
and
.V= unordered_map<T>
are hash tables for quick information storage and retrieval,
with unit operations running in $ O ( 1 ) $ time.
In C,
there is no support for hash tables,
so the programmer must code them explicitly.
.LE
.P
The C++ library reference is at
.V= http://www.cplusplus.com/reference/ .
.H 1 "What to submit"
.V= README ,
.V= Makefile ,
and all C++ header and implementation files.
Ensure that all files needed to build the project are submitted.
In later projects,
.E= "do not"
submit files generated by
.V= flex
and
.V= bison .
When the grader types the command
.V= make
in the submit directory,
the executable binary
.V= oc
should be built.
No error messages or warnings should be printed.
.P
.E= Warning\(::
After you submit, you must verify that the submit has worked.
Make a new empty directory in your personal file space,
copy all files that you have submitted into this directory
from your working directory and
perform a build.
Failing to submit a working build will cost you 50% of
the points for an assignment.
.P
Also, use RCS, CVS, SVN, etc.,
or something similar to maintain backup copies of your source code.
You may wish to periodically archive your project into a
.V= tar.gz
in order to keep copies.
If you are working with a partner,
keep a backup copy in a place your partner has no access to.
If you partner accidentally deletes all source code on the due
date, you get a zero as well.
.FINISH
