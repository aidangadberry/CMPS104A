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
#include "symtable.h"

const string CPP = "/usr/bin/cpp";
string cpp_command;
FILE *tokfile;
FILE *symfile;
vector<symbol_table*> symbol_stack;
symbol_table* types;
size_t next_block = 1;


// Open a pipe from the C preprocessor.
// Exit failure if can't.
// Assigns opened pipe to FILE* yyin.
void cpp_popen () {
    yyin = popen (cpp_command.c_str(), "r");
    if (yyin == NULL) {
        syserrprintf (cpp_command.c_str());
    }else {
        if (yy_flex_debug) {
            fprintf (stderr, "-- popen (%s), fileno(yyin) = %d\n",
                        cpp_command.c_str(), fileno (yyin));
        }
        lexer_newfilename (cpp_command.c_str());
    }
}


// Close C preprocessor pipe.
void cpp_pclose() {
    int pclose_rc = pclose (yyin);
    eprint_status (cpp_command.c_str(), pclose_rc);
    if (pclose_rc != 0) exit (EXIT_FAILURE);
}


// Grab each token with yylex(), intern it into the stringset, write
// token output to program.tok.
void open_tokfile (string filename) {

    string basefile(basename(const_cast<char*> (filename.c_str())));

    char* tokenfile = strdup((basefile.substr(0, basefile.length()-3) + 
                                                       ".tok").c_str());

    tokfile = fopen (tokenfile, "w");
    if (tokfile == NULL) {
        cout << "Error opening file";
        exit (EXIT_FAILURE);
    }
}

void open_symfile (string filename) {

    string basefile(basename(const_cast<char*> (filename.c_str())));

    char* symbolfile = strdup((basefile.substr(0, basefile.length()-3) + 
                                                       ".sym").c_str());

    symfile = fopen (symbolfile, "w");
    if (symfile == NULL) {
        cout << "Error opening file";
        exit (EXIT_FAILURE);
    }
}


// Dump stringset to program.str.
void gen_stringset (int argc, char** argv, string filename) {
    vector<string> args (&argv[1], &argv[argc]);
    ofstream strfile;

    //set_execname(argv[0]);
    string basefile(basename(const_cast<char*> (filename.c_str())));

    strfile.open(basefile.substr(0, basefile.length()-3) + ".str");
    
    for (const string& arg: args) {
        const string* str = intern_stringset (arg.c_str());
        strfile << "intern(" << arg << ") returned " << str << "->\"" 
                                              << *str << "\"" << endl;
    }

    dump_stringset (strfile);
    strfile.close();
}

void gen_astree (string filename) {
    string basefile(basename(const_cast<char*> (filename.c_str())));
    char* astfile = strdup((basefile.substr(0, basefile.length()-3) + 
                                                       ".ast").c_str());

    FILE* outfile = fopen (astfile, "w");
    if (astfile == NULL) {
        cout << "Error opening file";
        exit (EXIT_FAILURE);
    }
    dump_astree (outfile, yyparse_astree);
    fclose (outfile);
}

// Scan and set options.
void scan_opts (int argc, char** argv, string filename) {
    int opt;
    size_t i = filename.rfind('.', filename.length());
    
    opterr = 0;
    set_execname(argv[0]);
    yy_flex_debug = 0;
    yydebug = 0;
    cpp_command = CPP + " " + filename.c_str();

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
}


int main (int argc, char* argv[]) {
    string filename = argv[argc-1];

    scan_opts (argc, argv, filename);
    
    open_tokfile (filename);
    open_symfile (filename);
    cpp_popen();
    yyparse();
    cpp_pclose();
    gen_stringset (argc, argv, filename);
    typecheck (symfile, yyparse_astree);
    gen_astree (filename);
    fclose (tokfile);
    fclose (symfile);

    return EXIT_SUCCESS;
}
