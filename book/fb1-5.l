%{
# include "fb1-5.tab.h"    
%}

%% /* same rules as before, and no code in the third section */
[0-9]+ { yylval = atoi(yytext); return NUMBER; }
"+"    { return ADD; }
"-"    { return SUB; }
"*"    { return MUL; }
"/"    { return DIV; }
"|"    { return ABS; }
\n     { return E0L; }
[ \t]  { /* ignore whitespace */ }
.      { printf("Mystery character %c\n", *yytext); }
%%
