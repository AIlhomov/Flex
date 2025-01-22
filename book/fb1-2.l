/* English -> American */
/* Google translater wannabe */
%%
"colour" { printf("color"); }
"flavour" { printf("flavor"); }
"clever" { printf("smart"); }
"smart" { printf("elegant"); }
"conservative" { printf("traditional"); }
. { printf("%s", yytext); }
%%

main(int argc, char **argv)
{
  yylex();
}