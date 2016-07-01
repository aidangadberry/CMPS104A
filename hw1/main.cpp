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

const string CPP = "/usr/bin/cpp";
constexpr size_t LINESIZE = 1024;

// Chomp the last character from a buffer if it is delim.
void chomp (char* string, char delim) {
    size_t len = strlen (string);
    if (len == 0) return;
    char* nlpos = string + len - 1;
    if (*nlpos == delim) *nlpos = '\0';
}

// Run cpp against the lines of the file.
void cpplines (FILE* pipe, char* filename) {
    int linenr = 1;
    char inputname[LINESIZE];
    strcpy (inputname, filename);
    for (;;) {
        char buffer[LINESIZE];
        char* fgets_rc = fgets (buffer, LINESIZE, pipe);
        if (fgets_rc == NULL) break;
        chomp (buffer, '\n');
        // printf ("%s:line %d: [%s]\n", filename, linenr, buffer);
        // http://gcc.gnu.org/onlinedocs/cpp/Preprocessor-Output.html
        int sscanf_rc = sscanf (buffer, "# %d \"%[^\"]\"",
                                        &linenr, filename);
        if (sscanf_rc == 2) {
            /* printf ("DIRECTIVE: line %d file \"%s\"\n", linenr, 
                                                        filename); */
            continue;
        }
        char* savepos = NULL;
        char* bufptr = buffer;
        for (int tokenct = 1;; ++tokenct) {
            char* token = strtok_r (bufptr, " \t\n", &savepos);
            bufptr = NULL;
            if (token == NULL) break;
            // printf ("token %d.%d: [%s]\n",
                // linenr, tokenct, token);

            const string* tok = intern_stringset(token);
            // delete (tok);
            (void)tok;
        }
        ++linenr;
    }
}

int main (int argc, char* argv[]) {
    vector<string> args (&argv[1], &argv[argc]);
    int opt;

    string filename = argv[argc-1];
    char* filename2 = argv[argc-1];
    string command = CPP + " " + filename2;

    ofstream outfile;


    // check for correct usage, if correct then check for options 
    
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
                    cout << "Option" << opt << "used" << endl;
                    break;
                case 'y':
                    cout << "Option" << opt << "used" << endl;
                    break;
                case '@':
                    set_debugflags (optarg);
                    break;
                case 'D':
                    command = CPP + " -D" + optarg + " " + filename2;
                    break;
            }
        }
    }

    // filter input file through CPP, insert tokens into string set

    set_execname (argv[0]);

    // printf ("command=\"%s\"\n", command.c_str());
    FILE* pipe = popen (command.c_str(), "r");
    if (pipe == NULL) {
        cerr << (command.c_str()) << " is invalid";
        exit(EXIT_FAILURE);
    }else {
        cpplines (pipe, filename2);
        int pclose_rc = pclose (pipe);
        eprint_status (command.c_str(), pclose_rc);
        if (pclose_rc != 0) {
            exit(EXIT_FAILURE);
        }
    }


    // write string set to program.str

    string basefile(basename(const_cast<char*> (filename.c_str())));
    
    outfile.open(basefile.substr(0, basefile.length()-3) + ".str");
    for (const string& arg: args) {
        const string* str = intern_stringset (arg.c_str());
        outfile << "intern(" << arg << ") returned " << str << "->\"" 
                                            << *str << "\"" << endl;
        // free (const_cast<char*> (str));
    }

    dump_stringset (outfile);
    outfile.close();
    return EXIT_SUCCESS;
}
