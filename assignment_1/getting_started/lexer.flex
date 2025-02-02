%top{
    #include "parser.tab.hh"
    #define YY_DECL yy::parser::symbol_type yylex()
    #include "Node.h"
    int lexical_errors = 0;
}
%option yylineno noyywrap nounput batch noinput stack 
%%

 /* expression */
"+"                     {if(USE_LEX_ONLY) {printf("PLUSOP ");} else {return yy::parser::make_PLUSOP(yytext);}}
"-"                     {if(USE_LEX_ONLY) {printf("SUBOP ");} else {return yy::parser::make_MINUSOP(yytext);}}
"*"                     {if(USE_LEX_ONLY) {printf("MULTOP ");} else {return yy::parser::make_MULTOP(yytext);}}
"&&"                    {if(USE_LEX_ONLY) {printf("AND");} else {return yy::parser::make_AND(yytext);}}
"||"                    {if(USE_LEX_ONLY) {printf("OR");} else {return yy::parser::make_OR(yytext);}}
"<"                     {if(USE_LEX_ONLY) {printf("LESS THAN");} else {return yy::parser::make_LESS_THAN(yytext);}}
">"                     {if(USE_LEX_ONLY) {printf("MORE THAN");} else {return yy::parser::make_MORE_THAN(yytext);}}
"<="                     {if(USE_LEX_ONLY) {printf("LESS THAN EQUAL");} else {return yy::parser::make_LESS_THAN_EQUAL(yytext);}}
">="                     {if(USE_LEX_ONLY) {printf("MORE THAN EQUAL");} else {return yy::parser::make_MORE_THAN_EQUAL(yytext);}}


"=="                    {if(USE_LEX_ONLY) {printf("EQUAL");} else {return yy::parser::make_EQUAL(yytext);}}
"true"                   {if(USE_LEX_ONLY) {printf("true");} else {return yy::parser::make_TRUE(yytext);}}
"false"                  {if(USE_LEX_ONLY) {printf("false");} else {return yy::parser::make_FALSE(yytext);}}
"this"                   {if(USE_LEX_ONLY) {printf("this");} else {return yy::parser::make_THIS(yytext);}}
"new"                    {if(USE_LEX_ONLY) {printf("new");} else {return yy::parser::make_NEW(yytext);}}
"."                     {if(USE_LEX_ONLY) {printf("DOT");} else {return yy::parser::make_DOT(yytext);}}
"length"                 {if(USE_LEX_ONLY) {printf("length");} else {return yy::parser::make_LENGTH(yytext);}}
"!"                     {if(USE_LEX_ONLY) {printf("EXCLAMATION MARK");} else {return yy::parser::make_EXCLAMATION_MARK(yytext);}}

 /* factor */
"("                     {if(USE_LEX_ONLY) {printf("LP ");} else {return yy::parser::make_LP(yytext);}}
")"                     {if(USE_LEX_ONLY) {printf("RP ");} else {return yy::parser::make_RP(yytext);}}



"["                     {if(USE_LEX_ONLY) {printf("LEFT BRACKET");} else {return yy::parser::make_LEFT_BRACKET(yytext);}}
"]"                     {if(USE_LEX_ONLY) {printf("RIGHT BRACKET");} else {return yy::parser::make_RIGHT_BRACKET(yytext);}}
"{"                     {if(USE_LEX_ONLY) {printf("LEFT CURLY");} else {return yy::parser::make_LEFT_CURLY(yytext);}}
"}"                     {if(USE_LEX_ONLY) {printf("RIGHT CURLY");} else {return yy::parser::make_RIGHT_CURLY(yytext);}}
";"                     {if(USE_LEX_ONLY) {printf("SEMI COLON");} else {return yy::parser::make_SEMI_COLON(yytext);}}
","                     {if(USE_LEX_ONLY) {printf("COMMA");} else {return yy::parser::make_COMMA(yytext);}}
"/"                     {if(USE_LEX_ONLY) {printf("DIVIDE");} else {return yy::parser::make_DIVIDE(yytext);}}
"="                     {if(USE_LEX_ONLY) {printf("ASSIGN");} else {return yy::parser::make_ASSIGN(yytext);}}
"_"                     {if(USE_LEX_ONLY) {printf("UNDER SCORE");} else {return yy::parser::make_UNDER_SCORE(yytext);}}



"int"                    {if(USE_LEX_ONLY) {printf("int");} else {return yy::parser::make_INT(yytext);}} 
"boolean"                {if(USE_LEX_ONLY) {printf("boolean");} else {return yy::parser::make_BOOLEAN(yytext);}}
"class"                  {if(USE_LEX_ONLY) {printf("class");} else {return yy::parser::make_CLASS(yytext);}}
"else"                   {if(USE_LEX_ONLY) {printf("else");} else {return yy::parser::make_ELSE(yytext);}}
"extends"                {if(USE_LEX_ONLY) {printf("extends");} else {return yy::parser::make_EXTENDS(yytext);}}
"if"                     {if(USE_LEX_ONLY) {printf("if");} else {return yy::parser::make_IF(yytext);}}
"main"                   {if(USE_LEX_ONLY) {printf("main");} else {return yy::parser::make_MAIN(yytext);}}
"public"                 {if(USE_LEX_ONLY) {printf("public");} else {return yy::parser::make_PUBLIC(yytext);}}
"return"                 {if(USE_LEX_ONLY) {printf("return");} else {return yy::parser::make_RETURN(yytext);}}
"static"                 {if(USE_LEX_ONLY) {printf("static");} else {return yy::parser::make_STATIC(yytext);}}
"String"                 {if(USE_LEX_ONLY) {printf("String");} else {return yy::parser::make_STRING(yytext);}}
"System.out.println"     {if(USE_LEX_ONLY) {printf("System.out.println");} else {return yy::parser::make_SYSTEM_OUT_PRINTLN(yytext);}}
"void"                   {if(USE_LEX_ONLY) {printf("void");} else {return yy::parser::make_VOID(yytext);}}
"while"                  {if(USE_LEX_ONLY) {printf("while");} else {return yy::parser::make_WHILE(yytext);}}






0|[1-9][0-9]*           {if(USE_LEX_ONLY) {printf("INTEGER_LITERAL ");} else {return yy::parser::make_INTEGER_LITERAL(yytext);}}
[a-zA-Z_][a-zA-Z0-9_]*               {if(USE_LEX_ONLY) {printf("CHAR ");} else {return yy::parser::make_IDENTIFIER(yytext);}}
[ \t\n\r]+              {printf("%s", yytext);}
"//"[^\n]*              {printf("%s", yytext);}
.                       { if(!lexical_errors) fprintf(stderr, "Lexical errors found! See the logs below: \n"); fprintf(stderr,"\t@error at line %d. Character %s is not recognized\n", yylineno, yytext); lexical_errors = 1;}
<<EOF>>                  {return yy::parser::make_END();}
%%
