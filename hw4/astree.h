#ifndef __ASTREE_H__
#define __ASTREE_H__

#include <string>
#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <bitset>
using namespace std;

#include "auxlib.h"

struct symbol;
using symbol_table = unordered_map<string*,symbol*>;

enum { 
    ATTR_void, ATTR_bool, ATTR_char, ATTR_int, ATTR_null,
    ATTR_string, ATTR_struct, ATTR_array, ATTR_function,
    ATTR_variable, ATTR_field, ATTR_typeid, ATTR_param, ATTR_lval,
    ATTR_const, ATTR_vreg, ATTR_vaddr, ATTR_bitset_size,
};

using attr_bitset = bitset<ATTR_bitset_size>;



struct astree {
    int symbol;                 // token code
    size_t filenr;              // index into filename stack
    size_t linenr;              // line number from source code
    size_t offset;              // offset of token with current line
    const string* lexinfo;      // pointer to lexical information
    vector<astree*> children;   // children of this n-way node
    attr_bitset attributes;     // attributes
    size_t blocknr;             // block number
    symbol_table* struct_table; // struct table node

    astree (int symbol, int filenr, int linenr,
              int offset, const char* clexinfo);
};

// Append one child to the vector of children.
astree* adopt1 (astree* root, astree* child);

// Append two children to the vector of children.
astree* adopt2 (astree* root, astree* left, astree* right);

// Append three children to the vector of children.
astree* adopt3 (astree* root, astree* left, astree* middle,
                                                astree* right);

// Swaps the symbol of a root out for a different symbol.
astree* swap_sym (astree* root, int symbol);

// Dump an astree to a FILE.
void dump_astree (FILE* outfile, astree* root);

// Debug print an astree.
void yyprint (FILE* outfile, unsigned short toknum,
                  astree* yyvaluep);

// Recursively free an astree.
void free_ast (astree* tree);

// Recursively free two astrees.
void free_ast2 (astree* tree1, astree* tree2);

#endif
