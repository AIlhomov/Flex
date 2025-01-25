/* Skeleton and definitions for generating a LALR(1) parser in C++ */
%skeleton "lalr1.cc" 
%defines
%define parse.error verbose
%define api.value.type variant
%define api.token.constructor

/* Required code included before the parser definition begins */
%code requires{
  #include <string>
  #include "Node.h"
  #define USE_LEX_ONLY false //change this macro to true if you want to isolate the lexer from the parser.
}

/* Code included in the parser implementation file */
%code{
  #define YY_DECL yy::parser::symbol_type yylex()
  YY_DECL;
  
  Node* root;
  extern int yylineno;
}

/* Token definitions for the grammar */
/* Tokens represent the smallest units of the language, like operators and parentheses */
%token <std::string> PLUSOP MINUSOP MULTOP INT LP RP 
%token END 0 "end of file"

/* Operator precedence and associativity rules */
/* Used to resolve ambiguities in parsing expressions See https://www.gnu.org/software/bison/manual/bison.html#Precedence-Decl */ 
%left PLUSOP MINUSOP
%left MULTOP

/* Specify types for non-terminals in the grammar */
/* The type specifies the data type of the values associated with these non-terminals */
%type <Node *> root expression factor

/* Grammar rules section */
/* This section defines the production rules for the language being parsed */
%%
root:       expression {root = $1;};

expression: expression PLUSOP expression {      /*
                                                  Create a subtree that corresponds to the AddExpression
                                                  The root of the subtree is AddExpression
                                                  The childdren of the AddExpression subtree are the left hand side (expression accessed through $1) and right hand side of the expression (expression accessed through $3)
                                                */
                            $$ = new Node("AddExpression", "", yylineno);
                            $$->children.push_back($1);
                            $$->children.push_back($3);
                            /* printf("r1 "); */
                          }
            | expression MINUSOP expression {
                            $$ = new Node("SubExpression", "", yylineno);
                            $$->children.push_back($1);
                            $$->children.push_back($3);
                            /* printf("r2 "); */
                          }
            | expression MULTOP expression {
                            $$ = new Node("MultExpression", "", yylineno);
                            $$->children.push_back($1);
                            $$->children.push_back($3);
                            /* printf("r3 "); */
                          }
            | factor      {$$ = $1; /* printf("r4 ");*/}
            ;

factor:     INT           {  $$ = new Node("Int", $1, yylineno); /* printf("r5 ");  Here we create a leaf node Int. The value of the leaf node is $1 */}
            | LP expression RP { $$ = $2; /* printf("r6 ");  simply return the expression */}
    ;
