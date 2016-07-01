#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <libgen.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

using namespace std;

#include "stringset.h"
#include "auxlib.h"
#include "lyutils.h"
#include "astree.h"

const string CPP = "/usr/bin/cpp";
string cpp_command;
FILE *tok_file;

void cpp_pclose() {
    int pclose_rc = pclose (yyin);
    eprint_status (cpp_command.c_str(), pclose_rc);
    if (pclose_rc != 0) exit (EXIT_FAILURE);
}

// grab each token with yylex(), intern it into the stringset, as well
// as writing it to the program.tok file
void scan (string filename) {

    string basefile(basename(const_cast<char*> (filename.c_str())));

    char* tokenfile = strdup((basefile.substr(0, basefile.length()-3) + 
                        ".tok").c_str());


    tok_file = fopen (tokenfile, "w");
    if (tok_file == NULL) {
        cout << "Error opening file";
    } else {
        for (;;) {
            int token = yylex();

            if (yy_flex_debug) fflush (NULL);
            if (token == YYEOF) break;

            const string* tok = intern_stringset (yytext);
            (void)tok;

            DEBUGF('m', "token=%d", token);
        }
    }
    fprintf (tok_file, "test\n");
    fclose (tok_file);
    cpp_pclose ();
}

// Open a pipe from the C preprocessor.
// Exit failure if can't.
// Assigns opened pipe to FILE* yyin.
void cpp_popen (string filename) {
    yyin = popen (cpp_command.c_str(), "r");
    if (yyin == NULL) {
        syserrprintf (cpp_command.c_str());
    }else {
        if (yy_flex_debug) {
            fprintf (stderr, "-- popen (%s), fileno(yyin) = %d\n",
                        cpp_command.c_str(), fileno (yyin));
        }
        lexer_newfilename (cpp_command.c_str());
        scan (filename);
    }
}

// scan options, then proceed to file scanner
void scan_opts (int argc, char** argv, string filename) {
    int opt;

    opterr = 0;
    yy_flex_debug = 0;
    yydebug = 0;

    size_t i = filename.rfind('.', filename.length());

    if (i == string::npos || filename.substr(i, 
                filename.length()-i) != ".oc") {
        cerr << "Incorrect input file. Please provide a .oc file" 
                                                        << endl;
        exit(EXIT_FAILURE);
    } else {
        while ((opt = getopt (argc, argv, "@:D:ly")) != -1) {
            switch (opt) {
                case 'l':
                    yy_flex_debug = 1;
                    break;
                case 'y':
                    yydebug = 1;
                    break;
                case '@':
                    set_debugflags (optarg);
                    break;
                case 'D':
                    cpp_command = CPP + " -D" + optarg + " "
                                     + argv[argc-1];
                    break;
                default:
                    errprintf ("bad option (%c)\n", opt);
                    break;
            }
        }
    }

    if (optind > argc) {
        errprintf ("Usage: oc [-ly] [-@ flag ...] [-D string] %s\n",
                      "program.oc");
        exit (EXIT_FAILURE);
    }

    // open file by piping through CPP
    cpp_popen (filename);
}

int main (int argc, char* argv[]) {
    vector<string> args (&argv[1], &argv[argc]);

    string filename = argv[argc-1];
    char* filename2 = argv[argc-1];
    cpp_command = CPP + " " + filename2;
    ofstream strfile;
    set_execname(argv[0]);
    string basefile(basename(const_cast<char*> (filename.c_str())));
 
    // scan the options, then begin to scan input file
    scan_opts (argc, argv, filename);

    // open the program.str file to dump the stringset
    strfile.open(basefile.substr(0, basefile.length()-3) + ".str");
    
    for (const string& arg: args) {
        const string* str = intern_stringset (arg.c_str());
        strfile << "intern(" << arg << ") returned " << str << "->\"" 
                                            << *str << "\"" << endl;
    }
    dump_stringset (strfile);
    strfile.close();


    return EXIT_SUCCESS;
}
