#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include <string>
#include <vector>
#include <bitset>
using namespace std;

#include "auxlib.h"
#include "lyutils.h"
#include "astree.h"


extern size_t next_block;


using symbol_entry = pair<const string*,symbol*>;

struct symbol {
    attr_bitset attributes; 
    symbol_table* fields; 
    size_t filenr, linenr, offset, blocknr; 
    vector<symbol*>* parameters; 
};



void typecheck (FILE* outfile, astree* root);

#endif
