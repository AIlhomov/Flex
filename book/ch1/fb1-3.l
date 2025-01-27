/* Recognize tokens in the calculator input and print them */
%%
"+"    { printf("PLUS\n"); }
"-"    { printf("MINUS\n"); }
"*"    { printf("TIMES\n"); }
"/"    { printf("DIVIDE\n"); }
"|"    { printf("ABS\n"); }
[0-9]+ { printf("NUMBER %s\n", yytext); }
\n     { printf("NEWLINE\n"); }
[ \t]  { /* ignore whitespace */ }
.      { printf("Mystery character %s\n", yytext); }
%%

main(int argc, char **argv)
{
  yylex();
}