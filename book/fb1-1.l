/* Just like Unix wc */

/* 

HOW TO RUN:
1. flex fb1-1.l
2. cc lex.yy.c -lfl
3. ./a.out

*/

%{
int chars = 0;
int words = 0;
int lines = 0;
%}

%%
[a-zA-Z]+ { words++; chars += strlen(yytext); }
\n       { chars++; lines++; }
.        { chars++; }

%%

main(int argc, char **argv)
{
  yylex();
  printf("%8d%8d%8d\n", lines, words, chars);
}

/* 
Example of usage:

ai@localhost:~/compilor/book$ ./a.out 
The boy stood on the burning deck 
shelling peanuts by the peck
       2      12      63

*/