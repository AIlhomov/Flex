#   include <stdio.h>
#   include <stdlib.h>
#   include <stdarg.h>
#   include <string.h>
#   include <math.h>
#   include "Node.h"

#define NHASH 9997

struct symbol symtab[NHASH];

struct symbol *lookup(char *s) {
    struct symbol *sp;
    for (sp = &symtab[hash(s)]; sp >= symtab; sp--) {
        if (sp->name && !strcmp(sp->name, s)) {
            return sp;
        }
        if (sp->name == 0) {
            sp->name = strdup(s);
            sp->value = 0;
            return sp;
        }
    }
    yyerror("symbol table overflow\n");
    abort();
}

void yyerror(char *s, ...) {
    va_list ap;
    va_start(ap, s);
    fprintf(stderr, "%d: error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
}

int hash(char *s) {
    int hashval;
    for (hashval = 0; *s; s++) {
        hashval = *s + 31 * hashval;
    }
    return hashval % NHASH;
}

void init() {
    struct symbol *sp;
    for (sp = symtab + NHASH - 1; sp >= symtab; sp--) {
        sp->name = 0;
    }
}

void dump() {
    struct symbol *sp;
    for (sp = symtab + NHASH - 1; sp >= symtab; sp--) {
        if (sp->name) {
            printf("name: %s, value: %f\n", sp->name, sp->value);
        }
    }
}

void setval(struct symbol *sp, double d) {
    sp->value = d;
}

double getval(struct symbol *sp) {
    return sp->value;
}

void yyerror(char *s, ...);

int yylex();

int yyparse();

void init();

void dump();

struct symbol *lookup(char *s);

void setval(struct symbol *sp, double d);

double getval(struct symbol *sp);

int hash(char *s);

int main() {
    init();
    yyparse();
    dump();
    return 0;
}

