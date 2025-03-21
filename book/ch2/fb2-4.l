/* fb2-4 text concordance */

%option noyywrap nodefault yylineno case-insensitive

/* the symbol table */
%{
    struct symbol {           /* a word */
        char *name;
        struct ref *reflist;
    };

    struct ref {
        struct ref *next;
        char *filename;
        int flags;
        int lineno;
    };

    /* simple symtab of fixed size */
    #define NHASH 9997
    struct symbol symtab[NHASH];

    struct symbol *lookup(char*);
    void addref(int, char*, char*,int);

    char *curfilename;      /* current input file name */
%}
/* the rules */
/* skip common words */
%%


a |
an |
and |
are |
as |
at |
be |
but |
for |
in |
is |
it |
of |
on |
or |
that |
the |
this |
to      /* ignore */

[a-z]+(\'(s|t))? { addref(yylineno, curfilename, yytext, 0); }
.|\n           /* ignore */ 
%%

/* hash a symbol */
static unsigned
symhash(char *sym)
{
    unsigned int hash = 0;
    unsigned c;

    while(c = *sym++) hash = hash*9 ^ c;

    return hash;
}

struct symbol *
lookup(char* sym)
{
    struct symbol *sp = &symtab[symhash(sym)%NHASH];
    int scount = NHASH;     /* how many have we looked at */

    while(--scount >= 0) {
        if(sp->name && !strcmp(sp->name, sym)) return sp;

        if(!sp->name) {     /* new entry */
            sp->name = strdup(sym);
            sp->reflist = NULL;
            return sp;
        }

        if(++sp >= symtab+NHASH) sp = symtab; /* try the next entry */
    }
    fputs("symbol table overflow\n", stderr);
    abort();    /* tried them all, table is full */
}

void
addref(int lineno, char *filename, char *word, int flags)
{
    struct ref *r;
    struct symbol *sp = lookup(word);

    /* don't do dups of same line and file */
    if(sp->reflist &&
       sp->reflist->lineno == lineno &&
       sp->reflist->filename == filename) return;
    
    r = malloc(sizeof(struct ref));
    if(!r) {fputs("out of space\n", stderr); abort(); }
    r->next = sp->reflist;
    r->filename = filename;
    r->lineno = lineno;
    r->flags = flags;
    sp->reflist = r;
}

/* print the references
*  sort the table alphabetically
*  then flip each entry's reflist to get it into forward order
*  then print it
*/

/* aux function for sorting */
static int
symcompare(const void *xa, const void *xb)
{
    const struct symbol *a = xa;
    const struct symbol *b = xb;

    if(!a->name) {
        if(!b->name) return 0;  /* both empty */
        return 1;              /* put empties at the end */
    }
    if(!b->name) return -1;
    return strcmp(a->name, b->name);
}

void
printrefs()
{
    struct symbol *sp;
    qsort(symtab, NHASH, sizeof(struct symbol), symcompare); /* sort the symbol table */

    for(sp = symtab; sp->name && sp < symtab+NHASH; sp++) {
        char *prevfn = NULL;        /* alst printed filename, to skip dups */

        /* reverse the list of references */
        struct ref *rp = sp->reflist;
        struct ref *rpp = 0;       /* previous ref */
        struct ref *rpn;           /* next ref */

        do {
            rpn = rp->next;
            rp->next = rpp;
            rpp = rp;
            rp = rpn;
        } while(rp);

        /* now print the word and its references */
        printf("%10s", sp->name);
        for(rp = rpp; rp; rp = rp->next) {
            if(rp->filename == prevfn) {
                printf(" %d", rp->lineno);
            } else {
                printf(" %s:%d", rp->filename, rp->lineno);
                prevfn = rp->filename;
            }
        }
        printf("\n");

    }
}

/* concordance main routine */
main(argc, argv)
int argc;
char **argv;
{
    int i;

    if (argc < 2) {   /* just read stdin */
        curfilename = "(stdin)";
        yylineno = 1;
        yylex();
    } else
        for(i = 1; i < argc; i++){
            FILE *f = fopen(argv[i], "r");

            if(!f) {
                perror(argv[i]);
                return (1);
            }
            curfilename = argv[i];      /* for addref */
            
            yyrestart(f);
            yylineno = 1;
            yylex();
            fclose(f);
        }
    
    printrefs();
}