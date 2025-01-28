/* fb2-2.l read several files */

%option noyywrap

%{
int chars = 0;
int words = 0;
int lines = 0;

int totchars = 0;
int totwords = 0;
int totlines = 0;
%}

%%

[a-zA-Z]+   { words++; chars += strlen(yytext); }
\n          { chars++; lines++; }
.           { chars++; }

%%

main(argc, argv)
int argc;
char **argv;
{
    int i;

    if(argc < 2) { /* Just read stdin */
        yylex();
        printf("Lines: %d\nWords: %d\nChars: %d\n", lines, words, chars);
        return 0;
    }

    for(i = 1; i < argc; i++) {
        FILE *f = fopen(argv[i], "r");

        if(!f){
            perror(argv[i]);
            return (1);
        }
        yyrestart(f); /* Opens the file, makes it input to the scanner*/
        yylex();      /* Actually scans the file */
        fclose(f);
        printf("%s: Lines: %d\nWords: %d\nChars: %d\n", argv[i], lines, words, chars);
        totchars += chars; chars = 0;
        totwords += words; words = 0;
        totlines += lines; lines = 0;
    }
    if(argc > 1) /* print total if more than one file */
        printf("Total: Lines: %d\nWords: %d\nChars: %d\n", totlines, totwords, totchars);
    return 0;
}