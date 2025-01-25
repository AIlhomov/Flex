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

0|[1-9][0-9]*           {if(USE_LEX_ONLY) {printf("INT ");} else {return yy::parser::make_INT(yytext);}}

[ \t\n\r]+              {}
"//"[^\n]*              {}
.                       { if(!lexical_errors) fprintf(stderr, "Lexical errors found! See the logs below: \n"); fprintf(stderr,"\t@error at line %d. Character %s is not recognized\n", yylineno, yytext); lexical_errors = 1;}
<<EOF>>                  {return yy::parser::make_END();}
%%