
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <bitset>

#include "lyutils.h"
#include "astree.h"
#include "symtable.h"

extern FILE* symfile;
extern vector<symbol_table*> symbol_stack;
extern symbol_table* types;


vector<symbol_table*> symbol_stack = new vector<symbol_table*>;
symbol_table* types = new symbol_table;
symbol_stack.push_back (new symbol_table);

// ----------------------- SYMBOL TABLE CODE ----------------------- //

//symbol_table table;


char* get_attributes (attr_bitset attributes) {
    string str = "";

    if (attributes.test (ATTR_void))        str += "void ";
    if (attributes.test (ATTR_bool))        str += "bool ";
    if (attributes.test (ATTR_char))        str += "char ";
    if (attributes.test (ATTR_int))         str += "int ";
    if (attributes.test (ATTR_null))        str += "null ";
    if (attributes.test (ATTR_string))      str += "string ";
    if (attributes.test (ATTR_struct))      str += "struct ";
    if (attributes.test (ATTR_array))       str += "array ";
    if (attributes.test (ATTR_function))    str += "function ";
    if (attributes.test (ATTR_variable))    str += "variable ";
    if (attributes.test (ATTR_typeid))      str += "typeid ";
    if (attributes.test (ATTR_param))       str += "param ";
    if (attributes.test (ATTR_lval))        str += "lval ";
    if (attributes.test (ATTR_const))       str += "const ";
    if (attributes.test (ATTR_vreg))        str += "vreg ";
    if (attributes.test (ATTR_vaddr))       str += "vaddr ";

    return strdup(str->c_str());
}

symbol* new_symbol (astree* node) {
    symbol* symbol = new symbol();
    symbol->attributes = node->attributes;
    symbol->fields = nullptr;
    symbol->filenr = node->filenr;
    symbol->linenr = node->linenr;
    symbol->offset = node->offset;
    symbol->blocknr = node->blocknr;
    symbol->parameters = nullptr;
    return symbol;
}

void insert_symbol (symbol_table* table, astree* node) {
    symbol* symbol = new_symbol (node);

    if ((table!=NULL) && (node!=NULL)) {
        table->insert (symbol_entry (node->lexinfo, symbol));
    }
}

symbol* table_find_ident (symbol_table* table, astree* node) {
    if (table->count (node->lexinfo) == 0) {
        return nullptr;
    }
    return (table->find (node->lexinfo))->second;
}

void print_symbol (FILE* outfile, astree* node) {
    attr_bitset attributes = node->attributes;

    if (node->attributes[ATTR_struct]) {
        fprintf (outfile, "\n");
    } else {
        fprintf (outfile, "    ");
    }

    if (node->attributes[ATTR_field]) {
        fprintf (outfile, "%s (%zu.%zu.%zu) field {%s} ",
            (const<string*>node->lexinfo)->c_str(),
            node->linenr, node->filenr, node->offset,
            (const<string*>current_struct->lexinfo)->c_str());
    } else {
        fprintf (outfile, "%s (%zu.%zu.%zu) {%zu} ",
            (const<string*>node->lexinfo)->c_str(),
            node->linenr, node->filenr, node->offset,
            node->blocknr);
    }

    if (node->attributes[ATTR_struct]) {
        fprintf (outfile, "struct \"%s\" ", 
            (const<string*>node->lexinfo)->c_str());
        current_struct = node;
    }

    fprintf (outfile, "%s\n", get_attributes (node->attributes));
}

// ----------------------- SYMBOL STACK CODE ----------------------- //



void enter_block (){
    next_block++;
    symbol_stack.push_back (nullptr);
}

void leave_block (){
    symbol_stack.pop_back();
}

void define_ident (astree* node) {
    if (symbol_stack.back() == nullptr) {
        symbol_stack.back() = new symbol_table;
    }
    insert_symbol (symbol_stack.back(), node);
}

symbol* stack_find_ident (astree* node) {
    for (auto table : symbol_stack) {
        if (table != nullptr && !(table->empty())) {
            if (table_find_ident (table, node) != nullptr) {
                return table_find_ident (table, node);
            }
        }
    }
    return nullptr;
}

// ------------------------ TYPECHECK CODE ------------------------- //

astree* current_struct = nullptr;

bool check_primitive (astree* left, astree* right) {
    for (size_t i = 0; i < ATTR_function, i++) {
        if (left->attributes[i] == 1 && right->attributes[i] == 1) {
            return true;
        }
    }
    return false;
}

void inherit_attributes (astree* parent, astree* child) {
    for (size_t i = 0; i < ATTR_bitset_size; i++) {
        if (child->attributes[i] == 1) {
            parent->attributes.set (i);
        }
    }
}

void inherit_type (astree* parent, astree* child) {
    for (size_t i = 0; i < ATTR_function; i++) {
        if (child->attributes[i] == 1) {
            parent->attributes.set (i);
        }
    }
}

void check_block (astree* node) {
    if (node->symbol == TOK_BLOCK) {
        enter_block();
    }
    node->blocknr = next_block;
    for (auto child : node->children) {
        check_block (child);
    }
}

void check_prototype (FILE* outfile, astree* node) {
    node->children[0]->children[0]->attributes.set (ATTR_function);
    insert_symbol (symbol_stack[0], node->children[0]->children[0]);
    print_symbol (symbol_stack[0], node->children[0]->children[0]);
    enter_block();

    for (auto child : node->children[1]->children) {
        child->children[0]->attributes.set (ATTR_variable);
        child->children[0]->attributes.set (ATTR_lval);
        child->children[0]->attributes.set (ATTR_param);
        child->children[0]->blocknr = next_block;
        define_ident (child);
        print_symbol (outfile, child->children[0]);
    }

    leave_block();
}

void check_function (FILE* outfile, astree* node) {
    node->children[0]->children[0]->attributes.set (ATTR_function);
    insert_symbol (symbol_stack[0], node->children[0]->children[0]);
    print_symbol (symbol_stack[0], node->children[0]->children[0]);
    
    for (auto child : node->children[1]->children) {
        child->children[0]->attributes.set (ATTR_variable);
        child->children[0]->attributes.set (ATTR_lval);
        child->children[0]->attributes.set (ATTR_param);
        child->children[0]->blocknr = next_block;
        define_ident (child->children[0]);
        print_symbol (outfile, child->children[0]);
    }

    check_block (node->children[2]);
    leave_block();
}

void typecheck_node (FILE* outfile, astree* node) {
    astree* left;
    astree* right;
    symbol* symbol;

    if (node->children.size() > 0) {
        left = node->children[0];
    }
    if (node->children.size() > 1) {
        right = node->children[1];
    }

    switch (node->symbol) {
        case TOK_VOID:
            left->attributes.set (ATTR_void);               break;
        case TOK_BOOL:
            if (left == nullptr)                            break;
            left->attributes.set (ATTR_bool);
            inherit_type (node, left);                      break;
        case TOK_CHAR:
            if (left == nullptr)                            break;
            left->attributes.set (ATTR_char);
            inherit_type (node, left);                      break;
        case TOK_INT:
            if (left == nullptr)                            break;
            left->attributes.set (ATTR_int);
            inherit_type (node, left);                      break;
        case TOK_STRING:
            if (left == nullptr)                            break;
            left->attributes.set (ATTR_string);
            inherit_type (node, left);                      break;
        case TOK_IF:
        case TOK_IFELSE:
            if (!left->attributes[ATTR_bool]) {
                errprintf ("Error (%zu.%zu.%zu),
                    the expression operand of if
                    must be of type bool",
                    node->filenr, node->linenr,
                    node->offset);
            }                                               break;
        case TOK_WHILE:
            if (!left->attributes[ATTR_bool]) {
                errprintf ("Error (%zu.%zu.%zu),
                    the expression operand of 
                    while must be of type bool",
                    node->filenr, node->linenr,
                    node->offset);
            }                                               break;
        case TOK_RETURN:                                    break;
        case TOK_STRUCT:
            left->attributes.set (ATTR_struct);
            insert_symbol (types, left);
            print_symbol (outfile, left);
            symbol* symbol = 
                table_find_ident (types, left);
            symbol->fields = new symbol_table;
            for (auto n = node->children.begin()+1;
                n != node->children.end(); n++) {
                insert_symbol (symbol->fields, *n);
                print_symbol (
                    outfile, (*n)->children[0]);
            }                                               break;
        case TOK_FALSE:
        case TOK_TRUE:
            node->attributes.set (ATTR_bool);
            node->attributes.set (ATTR_const);              break;
        case TOK_NULL:
            node->attributes.set (ATTR_null);
            node->attributes.set (ATTR_const);              break;
        case TOK_NEW:
            inherit_attributes (node, left);                break;
        case TOK_ARRAY:
            left->attributes.set (ATTR_array);
            if (left == nullptr || 
                left->children.empty())                     break;
            left->children[0]->attributes.set (ATTR_array); break;
        case TOK_EQ:
        case TOK_NE:
        case TOK_LT:
        case TOK_LE:
        case TOK_GT:
        case TOK_GE:
            if (check_primitive (left, right)) {
                node->attributes.set (ATTR_bool);
            } else {
                errprintf ("Error (%zu.%zu.%zu),
                    incompatible types", node->filenr, 
                    node->linenr, node->offset);
            }                                               break;
        case TOK_IDENT:
            symbol = stack_find_ident (node);

            if (symbol == nullptr) {
                table_find_ident (types, node);
            }

            if (symbol == nullptr) {
                errprintf ("Error (%zu.%zu.%zu),
                    identifier \"%s\" not found",
                    node->filenr, node->linenr, 
                    node->offset, 
                    (const<string*>node->lexinfo
                        )->c_str());                        break;
            }

            node->attributes = symbol->attributes;          break;
        case TOK_INTCON:
            node->attributes.set (ATTR_int);
            node->attributes.set (ATTR_const);              break;
        case TOK_CHARCON:
            node->attributes.set (ATTR_char);
            node->attributes.set (ATTR_const);              break;
        case TOK_STRINGCON:
            node->attributes.set (ATTR_string);
            node->attributes.set (ATTR_const);              break;
        case TOK_BLOCK:
            check_block (node);
            leave_block();                                  break;
        case TOK_CALL:
            symbol = table_find_ident (symbol_stack[0],
                                node->children.back());
            if (symbol == nullptr) {
                errprintf ("Error (%zu.%zu.%zu),
                    function \"%s\" not found",
                    node->filenr, node->linenr, 
                    node->offset, 
            (const<string*>node->children.back()->lexinfo
                    )->c_str());                            break;
            }

            for (size_t i = 0; i < ATTR_function; i++) {
                if (symbol->attributes[i] == 1) {
                    node->attributes.set (i);
                }
            }                                               break;
        case TOK_NEWARRAY:
            node->attributes.set (ATTR_vreg);
            node->attributes.set (ATTR_array);
            inherit_type (node, left);                      break;
        case TOK_TYPEID:
            node->attributes.set (ATTR_typeid);             break;
        case TOK_FIELD:
            node->attributes.set (ATTR_field);
            if (left != nullptr) {
                left->attributes.set (ATTR_field);
                inherit_type (node, left);
            }                                               break;
        case TOK_ORD:
            node->attributes.set (ATTR_int);
            node->attributes.set (ATTR_vreg);
            if (!(left->attributes[ATTR_char])) {
                errprintf ("Error (%zu.%zu.%zu),
                    incorrect ord type", node->filenr,
                    node->linenr, node->offset);            break;
            }
        case TOK_CHR:
            node->attributes.set (ATTR_char);
            node->attributes.set (ATTR_vreg);
            if (!(left->attributes[ATTR_int])) {
                errprintf ("Error (%zu.%zu.%zu),
                    incorrect chr type", node->filenr,
                    node->linenr, node->offset);            break;
            }
        case TOK_ROOT:
        case TOK_PARAMLIST:                                 break;
        case TOK_PROTOTYPE:
            check_prototype (outfile, node);                break;
        case TOK_FUNCTION:
            enter_block();
            check_function (outfile, node);
            print_symbol (outfile, node);                   break;
        case TOK_DECLID:                                    break;
        case TOK_INDEX:
            node->attributes.set (ATTR_lval);
            node->attributes.set (ATTR_vaddr);              break;
        case TOK_NEWSTRING:
            node->attributes.set (ATTR_vreg);
            node->attributes.set (ATTR_string);             break;
        case TOK_RETURNVOID:                                break;
        case TOK_VARDECL:
            left->children[0]->attributes.set (ATTR_lval);
            left->children[0]->attributes.set (
                ATTR_variable);
            inherit_attributes (node, left);
            if (stack_find_ident (left->children[0])) {
                errprintf ("Error (%zu.%zu.%zu),
                    variable already declared",
                    node->filenr, node->linenr,
                    node->offset);
            }
            define_ident (left->children[0]);
            print_symbol (outfile, left->children[0]);      break;
        case '=':
            if (left == nullptr)                            break;
            if (left->attributes[ATTR_lval] && 
                right->attributes[ATTR_vreg]) {
                inherit_type (node, left);
                node->attributes.set (ATTR_vreg);
            } else {
                errprintf ("Error (%zu.%zu.%zu),
                    incompatible types", node->filenr, 
                    node->linenr, node->offset);
            }                                               break;
        case '+':
        case '-':
            node->attributes.set (ATTR_vreg);
            node->attributes.set (ATTR_int);
            if (right == nullptr) {
                if (left == nullptr)                        break;
                if (!(left->attributes[ATTR_int])) {
                    errprintf ("Error (%zu.%zu.%zu),
                        int type required", node->filenr, 
                        node->linenr, node->offset);
                }
            } else {
                if (!(left->attributes[ATTR_int]) ||
                    !(right->attributes[ATTR_int])) {
                    errprintf ("Error (%zu.%zu.%zu),
                        int type required", node->filenr, 
                        node->linenr, node->offset);
                }
            }                                               break;
        case '*':
        case '/':
        case '%':
            node->attributes.set (ATTR_vreg);
            node->attributes.set (ATTR_int);
            if (!(left->attributes[ATTR_int]) ||
                !(right->attributes[ATTR_int])) {
                errprintf ("Error (%zu.%zu.%zu),
                    int type required", node->filenr, 
                    node->linenr, node->offset);
            }                                               break;
        case '!':
            node->attributes.set (ATTR_vreg);
            node->attributes.set (ATTR_bool);
            if (!(left->attributes[ATTR_int])) {
                errprintf ("Error (%zu.%zu.%zu),
                    bool type required", node->filenr, 
                    node->linenr, node->offset);
            }                                               break;
        case '.':
            node->attributes.set (ATTR_lval);
            node->attributes.set (ATTR_vaddr);
            symbol = table_find_ident (types, node);
            inherit_type (node, left);                      break;
        default:
            errprintf ("Error, invalid token \"%s\"",
                get_yytname (node->symbol));
    }
}

void recursive_typecheck (FILE* outfile, astree* node) {
    for (auto child : node->children) {
        recursive_typecheck (outfile, child);
    }
    typecheck_node (outfile, node);
}

void typecheck (FILE* outfile, astree* root) {
    recursive_typecheck (outfile, root);
    while (!symbol_stack.empty()) {
        leave_block();
    }
}


/*
%token  TOK_VOID TOK_BOOL TOK_CHAR TOK_INT TOK_STRING
%token  TOK_IF TOK_ELSE TOK_WHILE TOK_RETURN TOK_STRUCT
%token  TOK_FALSE TOK_TRUE TOK_NULL TOK_NEW TOK_ARRAY
%token  TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%token  TOK_IDENT TOK_INTCON TOK_CHARCON TOK_STRINGCON

%token  TOK_BLOCK TOK_CALL TOK_IFELSE TOK_INITDECL
%token  TOK_POS TOK_NEG TOK_NEWARRAY TOK_TYPEID TOK_FIELD
%token  TOK_ORD TOK_CHR TOK_ROOT

%token  TOK_PARAMLIST TOK_PROTOTYPE TOK_FUNCTION TOK_DECLID
%token  TOK_INDEX TOK_NEWSTRING TOK_RETURNVOID TOK_VARDECL  

%right  '='
%left   '+' '-'
%left   '*' '/' '%'
%right  '!'
%left   '[' '.'

%nonassoc   '(' 
*/


