/* Simplest version of a calculator */
%{
#include <stdio.h>
%}

/* Declare tokens */
%token NUMBER
%token ADD SUB MUL DIV ABS
%token E0L

%%

calclist: /* nothing */
    | calclist exp E0L { printf("= %d\n", $2); }
    ;

exp: factor
    | exp ADD factor { $$ = $1 + $3; }
    | exp SUB factor { $$ = $1 - $3; }
    ;
    
factor: term
    | factor MUL term { $$ = $1 * $3; }
    | factor DIV term { $$ = $1 / $3; }
    ;

term: NUMBER
    | ABS term { $$ = $2 >= 0? $2 : - $2; }
;

%%

main(int argc, char **argv)
{
    yyparse();
}

yyerror(char *s)
{
    fprintf(stderr, "error: %s\n", s);
}

// term, factor and exp tells bison how to handle ABS, MUL, DIV, ADD and SUB