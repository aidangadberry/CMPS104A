// $Id: parser.y,v 1.12 2015-07-09 14:52:13-07 - - $

%{

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "astree.h"
#include "lyutils.h"

%}

%debug
%defines
%error-verbose
%token-table
%verbose

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

%right  TOK_IF TOK_ELSE
%right  '='
%left   TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%left   '+' '-'
%left   '*' '/' '%'
%right  TOK_POS TOK_NEG '!' TOK_NEW TOK_ORD TOK_CHR
%left   TOK_ARRAY TOK_FIELD TOK_FUNCTION
%left   '[' '.'

%nonassoc   '('  


%start  start

%%

start       : program               { yyparse_astree = $1; }
            ;

program     : program structdef     { $$ = adopt1 ($1, $2); }
            | program function      { $$ = adopt1 ($1, $2); }
            | program statement     { $$ = adopt1 ($1, $2); }
            | program error '}'     { $$ = $1; }
            | program error ';'     { $$ = $1; }
            |                       { $$ = new_parseroot (); }
            ;

structdef   : TOK_STRUCT TOK_IDENT '{' '}'
                                    { 
                                      free_ast2 ($3, $4);
                                      $2 = swap_sym ($2, TOK_TYPEID);
                                      $$ = adopt1 ($1, $2);
                                    }
            | TOK_STRUCT TOK_IDENT statements '}'
                                    {
                                      free_ast ($4);
                                      $2 = swap_sym ($2, TOK_TYPEID);
                                      $$ = adopt2 ($1, $2, $3);
                                    }
            ;

statements  : '{' fielddecl ';'     { 
                                      free_ast ($3);
                                      $$ = adopt1 ($1, $2);
                                    }
            | statements fielddecl ';' 
                                    { 
                                      free_ast ($3);
                                      $$ = adopt1 ($1, $2);
                                    }
            ; 

fielddecl   : basetype TOK_ARRAY TOK_IDENT
                                    { 
                                      $3 = swap_sym ($3, TOK_FIELD);
                                      $$ = adopt2 ($2, $1, $3);
                                    }
            | basetype TOK_IDENT    { 
                                      $2 = swap_sym ($2, TOK_FIELD);
                                      $$ = adopt1 ($1, $2);
                                    }
            ;

basetype    : TOK_VOID              { $$ = $1; }
            | TOK_BOOL              { $$ = $1; }
            | TOK_CHAR              { $$ = $1; }
            | TOK_INT               { $$ = $1; }
            | TOK_STRING            { $$ = $1; }
            | TOK_IDENT             { $$ = swap_sym ($1, TOK_TYPEID); }
            ;

function    : identdecl '(' ')' ';' { 
                                      free_ast2 ($3, $4);
                                      $2 = swap_sym ($2, TOK_PARAMLIST);
                                      $$ = new astree (
                                             TOK_PROTOTYPE,
                                             $1->filenr,
                                             $1->linenr,
                                             $1->offset,
                                             ""
                                           );
                                      $$ = adopt2 ($$, $1, $2);
                                    }
            | identdecl '(' ')' block
                                    { 
                                      free_ast ($3);
                                      $2 = swap_sym ($2, TOK_PARAMLIST);
                                      $$ = new astree (
                                             TOK_FUNCTION,
                                             $1->filenr,
                                             $1->linenr,
                                             $1->offset,
                                             ""
                                           );
                                      $$ = adopt3 ($$, $1, $2, $4);
                                    }
            | identdecl params ')' ';'
                                    {
                                      free_ast2 ($3, $4);
                                      $$ = new astree (
                                             TOK_PROTOTYPE,
                                             $1->filenr,
                                             $1->linenr,
                                             $1->offset,
                                             ""
                                           );
                                      $$ = adopt2 ($$, $1, $2);
                                    }
            | identdecl params ')' block
                                    { 
                                      free_ast ($3);
                                      $$ = new astree (
                                             TOK_FUNCTION,
                                             $1->filenr,
                                             $1->linenr,
                                             $1->offset,
                                             ""
                                           );
                                      $$ = adopt3 ($$, $1, $2, $4);
                                    }
            ;

params      : '(' identdecl         {
                                      $1 = swap_sym ($1, TOK_PARAMLIST);
                                      $$ = adopt1 ($1, $2);
                                    }
            | params ',' identdecl  {
                                      free_ast ($2);
                                      $$ = adopt1 ($1, $3);
                                    }

            ; 

identdecl   : basetype TOK_ARRAY TOK_IDENT 
                                    { 
                                      $3 = swap_sym ($3, TOK_DECLID);
                                      $$ = adopt2 ($2, $1, $3);
                                    }
            | basetype TOK_IDENT    { 
                                      $2 = swap_sym ($2, TOK_DECLID);
                                      $$ = adopt1 ($1, $2);
                                    }
            ;

block       : '{' '}'               { 
                                      free_ast ($2);
                                      $$ = swap_sym ($1, TOK_BLOCK);
                                    }
            | body '}'              {
                                      free_ast ($2);
                                      $$ = swap_sym ($1, TOK_BLOCK);
                                    }
            ;

body        : '{' statement         { 
                                      $1 = swap_sym ($1, TOK_BLOCK);
                                      $$ = adopt1 ($1, $2);
                                    }
            | body statement        { $$ = adopt1 ($1, $2); }
            ;

statement   : block                 { $$ = $1; }
            | vardecl               { $$ = $1; }
            | while                 { $$ = $1; }
            | ifelse                { $$ = $1; }
            | return                { $$ = $1; }
            | expr ';'              {
                                      free_ast ($2);
                                      $$ = $1;
                                    }
            | ';'                   { $$ = $1; }
            ;

vardecl     : identdecl '=' expr ';'
                                    { 
                                      free_ast ($4);
                                      $2 = swap_sym ($2, TOK_VARDECL);
                                      $$ = adopt2 ($2, $1, $3);
                                    }
            ;

while       : TOK_WHILE '(' expr ')' statement
                                    { 
                                      free_ast2 ($2, $4);
                                      $$ = adopt2 ($1, $3, $5);
                                    }
            ;

ifelse      : TOK_IF '(' expr ')' statement TOK_ELSE statement
                                    {
                                      free_ast2 ($2, $4);
                                      swap_sym ($1, TOK_IFELSE);
                                      $$ = adopt3 ($1, $3, $5, $7);
                                    }
            | TOK_IF '(' expr ')' statement
                                    {
                                      free_ast2 ($2, $4);
                                      $$ = adopt2 ($1, $3, $5);
                                    }
            ;

return      : TOK_RETURN ';'        {
                                      free_ast ($2);
                                      $$ = swap_sym ($1,
                                                     TOK_RETURNVOID);
                                    }
            | TOK_RETURN expr ';'   { 
                                      free_ast ($3);
                                      $$ = adopt1 ($1, $2);
                                    }
            ;

expr        : expr binop expr       { $$ = adopt2 ($2, $1, $3); }
            | unop expr             { $$ = adopt1 ($$, $2); }
            | allocator             { $$ = $1; }
            | call                  { $$ = $1; }
            | '(' expr ')'          {
                                      free_ast2 ($1, $3);
                                      $$ = $2;
                                    }
            | variable              { $$ = $1; }
            | constant              { $$ = $1; }
            ;

binop       : TOK_EQ                { $$ = $1; }
            | TOK_NE                { $$ = $1; }
            | TOK_LT                { $$ = $1; }
            | TOK_LE                { $$ = $1; }
            | TOK_GT                { $$ = $1; }
            | TOK_GE                { $$ = $1; }
            | '+'                   { $$ = $1; }
            | '-'                   { $$ = $1; }
            | '*'                   { $$ = $1; }
            | '/'                   { $$ = $1; }
            | '='                   { $$ = $1; }
            ;

unop        : TOK_POS               { $$ = $1; }
            | TOK_NEG               { $$ = $1; }
            | '!'                   { $$ = $1; }
            | TOK_NEW               { $$ = $1; }
            | TOK_ORD               { $$ = $1; }
            | TOK_CHR               { $$ = $1; }
            ;

allocator   : TOK_NEW TOK_IDENT '(' ')'
                                    {
                                      free_ast2 ($3, $4);
                                      $2 = swap_sym ($2, TOK_TYPEID);
                                      $$ = adopt1 ($1, $2);
                                    }
            | TOK_NEW TOK_STRING '(' expr ')'
                                    {
                                      free_ast ($2);
                                      free_ast2 ($3, $5);
                                      $1 = swap_sym ($1, TOK_NEWSTRING);
                                      $$ = adopt1 ($1, $4);
                                    }
            | TOK_NEW basetype '[' expr ']'
                                    {
                                      free_ast2 ($3, $5);
                                      $1 = swap_sym ($1, TOK_NEWARRAY);
                                      $$ = adopt2 ($1, $2, $4);
                                    }
            ;

call        : TOK_IDENT '(' ')'     {
                                      free_ast ($3);
                                      $2 = swap_sym ($2, TOK_CALL);
                                      $$ = adopt1 ($2, $1);
                                    }
            | exprs ')'             {
                                      free_ast ($2);
                                      $$ = $1;
                                    }
            ;

exprs       : TOK_IDENT '(' expr    {
                                      $2 = swap_sym ($2, TOK_CALL);
                                      $$ = adopt2 ($2, $1, $3);
                                    }
            | exprs ',' expr        { 
                                      free_ast ($2);
                                      $$ = adopt1 ($1, $3);
                                    }
            ;

variable    : TOK_IDENT             { $$ = $1; }
            | expr '[' expr ']'     { 
                                      free_ast ($4);
                                      $2 = swap_sym ($2, TOK_INDEX);
                                      $$ = adopt2 ($2, $1, $3);
                                    }
            | expr '.' TOK_IDENT    {
                                      $3 = swap_sym ($3, TOK_FIELD);
                                      $$ = adopt2 ($2, $1, $3);
                                    }
            ;

constant    : TOK_INTCON            { $$ = $1; }
            | TOK_CHARCON           { $$ = $1; }
            | TOK_STRINGCON         { $$ = $1; }
            | TOK_FALSE             { $$ = $1; }
            | TOK_TRUE              { $$ = $1; }
            | TOK_NULL              { $$ = $1; }
            ;

%%

const char* get_yytname (int symbol) {
   return yytname [YYTRANSLATE (symbol)];
}

bool is_defined_token (int symbol) {
   return YYTRANSLATE (symbol) > YYUNDEFTOK;
}
