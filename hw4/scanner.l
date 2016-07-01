/* $Id: scanner.l,v 1.4 2015-11-13 13:50:39-08 - - $ */

%{

#include "lyutils.h"

#define YY_USER_ACTION { lexer_useraction(); }

#define RETURN(SYMBOL) { return yylval_token(SYMBOL); }

%}

%option 8bit
%option debug
%option nodefault
%option nounput
%option noyywrap
%option verbose
%option warn

LETTER          [A-Za-z_]
DIGIT           [0-9]

INTEGER         ({DIGIT}+)
CHAR            ('([^\\'\n]|\\[\\'"0nt])')
STRING          (\"([^\\"\n]|\\[\\'"0nt])*\")
IDENT           ({LETTER}({LETTER}|{DIGIT})*)


%%

"#".*           { lexer_include(); }
[ \t]+          { }
\n              { lexer_newline(); }


"void"          { RETURN (TOK_VOID); }
"bool"          { RETURN (TOK_BOOL); }
"char"          { RETURN (TOK_CHAR); }
"int"           { RETURN (TOK_INT); }
"string"        { RETURN (TOK_STRING); }
"struct"        { RETURN (TOK_STRUCT); }
"if"            { RETURN (TOK_IF); }
"else"          { RETURN (TOK_ELSE); }
"while"         { RETURN (TOK_WHILE); }
"return"        { RETURN (TOK_RETURN); }
"false"         { RETURN (TOK_FALSE); }
"true"          { RETURN (TOK_TRUE); }
"null"          { RETURN (TOK_NULL); }
"ord"           { RETURN (TOK_ORD); }
"chr"           { RETURN (TOK_CHR); }
"new"           { RETURN (TOK_NEW); }

"=="            { RETURN (TOK_EQ); }
"!="            { RETURN (TOK_NE); }
"<="            { RETURN (TOK_LE); }
">="            { RETURN (TOK_GE); }
"<"             { RETURN (TOK_LT); }
">"             { RETURN (TOK_GT); }

{CHAR}          { RETURN (TOK_CHARCON); }
{STRING}        { RETURN (TOK_STRINGCON); }
{INTEGER}       { RETURN (TOK_INTCON); }
{IDENT}         { RETURN (TOK_IDENT); }

"[]"            { RETURN (TOK_ARRAY); }
"("             { RETURN ('('); }
")"             { RETURN (')'); }
"["             { RETURN ('['); }
"]"             { RETURN (']'); }
"{"             { RETURN ('{'); }
"}"             { RETURN ('}'); }
";"             { RETURN (';'); }
","             { RETURN (','); }
"."             { RETURN ('.'); }
"="             { RETURN ('='); }
"+"             { RETURN ('+'); }
"-"             { RETURN ('-'); }
"*"             { RETURN ('*'); }
"/"             { RETURN ('/'); }
"%"             { RETURN ('%'); }
"!"             { RETURN ('!'); }

.               { lexer_badchar (*yytext); }

%%
