%top{
    #include "parser.tab.hh"
    #define YY_DECL yy::parser::symbol_type yylex()
    #include "Node.h"
    int lexical_errors = 0;
}
%option yylineno noyywrap nounput batch noinput stack 
%%

"+"                     {if(USE_LEX_ONLY) {printf("PLUSOP ");} else {return yy::parser::make_PLUSOP(yytext);}}
"-"                     {if(USE_LEX_ONLY) {printf("SUBOP ");} else {return yy::parser::make_MINUSOP(yytext);}}
"*"                     {if(USE_LEX_ONLY) {printf("MULTOP ");} else {return yy::parser::make_MULTOP(yytext);}}
"("                     {if(USE_LEX_ONLY) {printf("LP ");} else {return yy::parser::make_LP(yytext);}}
")"                     {if(USE_LEX_ONLY) {printf("RP ");} else {return yy::parser::make_RP(yytext);}}

"["                     {if(USE_LEX_ONLY) {printf("LS ");} else {return yy::parser::make_LS(yytext);}}
"]"                     {if(USE_LEX_ONLY) {printf("RS ");} else {return yy::parser::make_RS(yytext);}}
"{"                     {if(USE_LEX_ONLY) {printf("LC ");} else {return yy::parser::make_LC(yytext);}}
"}"                     {if(USE_LEX_ONLY) {printf("RC ");} else {return yy::parser::make_RC(yytext);}}
","                     {if(USE_LEX_ONLY) {printf("COMMA ");} else {return yy::parser::make_COMMA(yytext);}}
";"                     {if(USE_LEX_ONLY) {printf("SEMICOLON ");} else {return yy::parser::make_SEMICOLON(yytext);}}
":"                     {if(USE_LEX_ONLY) {printf("COLON ");} else {return yy::parser::make_COLON(yytext);}}
"="                     {if(USE_LEX_ONLY) {printf("ASSIGN ");} else {return yy::parser::make_ASSIGN(yytext);}}
"=="                    {if(USE_LEX_ONLY) {printf("EQ ");} else {return yy::parser::make_EQ(yytext);}}
"!="                    {if(USE_LEX_ONLY) {printf("NEQ ");} else {return yy::parser::make_NEQ(yytext);}}
"<"                     {if(USE_LEX_ONLY) {printf("LT ");} else {return yy::parser::make_LT(yytext);}}
"<="                    {if(USE_LEX_ONLY) {printf("LEQ ");} else {return yy::parser::make_LEQ(yytext);}}
">"                     {if(USE_LEX_ONLY) {printf("GT ");} else {return yy::parser::make_GT(yytext);}}
">="                    {if(USE_LEX_ONLY) {printf("GEQ ");} else {return yy::parser::make_GEQ(yytext);}}
"&&"                    {if(USE_LEX_ONLY) {printf("AND ");} else {return yy::parser::make_AND(yytext);}}
"||"                    {if(USE_LEX_ONLY) {printf("OR ");} else {return yy::parser::make_OR(yytext);}}
"!"                     {if(USE_LEX_ONLY) {printf("NOT ");} else {return yy::parser::make_NOT(yytext);}}

"if"                    {if(USE_LEX_ONLY) {printf("IF ");} else {return yy::parser::make_IF(yytext);}}
"else"                  {if(USE_LEX_ONLY) {printf("ELSE ");} else {return yy::parser::make_ELSE(yytext);}}
"while"                 {if(USE_LEX_ONLY) {printf("WHILE ");} else {return yy::parser::make_WHILE(yytext);}}
"for"                   {if(USE_LEX_ONLY) {printf("FOR ");} else {return yy::parser::make_FOR(yytext);}}
"return"                {if(USE_LEX_ONLY) {printf("RETURN ");} else {return yy::parser::make_RETURN(yytext);}}
"int"                   {if(USE_LEX_ONLY) {printf("INT ");} else {return yy::parser::make_INT(yytext);}}
"char"                  {if(USE_LEX_ONLY) {printf("CHAR ");} else {return yy::parser::make_CHAR(yytext);}}
"bool"                  {if(USE_LEX_ONLY) {printf("BOOL ");} else {return yy::parser::make_BOOL(yytext);}}
"true"                  {if(USE_LEX_ONLY) {printf("TRUE ");} else {return yy::parser::make_TRUE(yytext);}}
"false"                 {if(USE_LEX_ONLY) {printf("FALSE ");} else {return yy::parser::make_FALSE(yytext);}}
"string"                {if(USE_LEX_ONLY) {printf("STRING ");} else {return yy::parser::make_STRING(yytext);}}
"array"                 {if(USE_LEX_ONLY) {printf("ARRAY ");} else {return yy::parser::make_ARRAY(yytext);}}
"var"                   {if(USE_LEX_ONLY) {printf("VAR ");} else {return yy::parser::make_VAR(yytext);}}
"const"                 {if(USE_LEX_ONLY) {printf("CONST ");} else {return yy::parser::make_CONST(yytext);}}

"func"                  {if(USE_LEX_ONLY) {printf("FUNC ");} else {return yy::parser::make_FUNC(yytext);}}
"void"                  {if(USE_LEX_ONLY) {printf("VOID ");} else {return yy::parser::make_VOID(yytext);}}
"main"                  {if(USE_LEX_ONLY) {printf("MAIN ");} else {return yy::parser::make_MAIN(yytext);}}
"print"                 {if(USE_LEX_ONLY) {printf("PRINT ");} else {return yy::parser::make_PRINT(yytext);}}
"scan"                  {if(USE_LEX_ONLY) {printf("SCAN ");} else {return yy::parser::make_SCAN(yytext);}}
"break"                 {if(USE_LEX_ONLY) {printf("BREAK ");} else {return yy::parser::make_BREAK(yytext);}}
"continue"              {if(USE_LEX_ONLY) {printf("CONTINUE ");} else {return yy::parser::make_CONTINUE(yytext);}}
"."                     {if(USE_LEX_ONLY) {printf("DOT ");} else {return yy::parser::make_DOT(yytext);}}

0|[1-9][0-9]*           {if(USE_LEX_ONLY) {printf("INT ");} else {return yy::parser::make_INT(yytext);}}


[a-zA-Z_][a-zA-Z0-9_]*   {if(USE_LEX_ONLY) {printf("ID ");} else {return yy::parser::make_ID(yytext);}}
[ \t\n\r]+              {printf("%s", yytext);}
"//"[^\n]*              {printf("%s", yytext);}
.                       { if(!lexical_errors) fprintf(stderr, "Lexical errors found! See the logs below: \n"); fprintf(stderr,"\t@error at line %d. Character %s is not recognized\n", yylineno, yytext); lexical_errors = 1;}
<<EOF>>                  {return yy::parser::make_END();}
%%