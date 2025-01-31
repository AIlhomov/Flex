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
%token <std::string> PLUSOP MINUSOP MULTOP INT LP RP AND OR LESS_THAN MORE_THAN EQUAL TRUE FALSE THIS NEW LEFT_BRACKET RIGHT_BRACKET
	    /* make */
%token <std::string> DOT LENGTH EXCLAMATION_MARK
/* not done */
%token <std::string> ASSIGN MORE_THAN_EQUAL LESS_THAN_EQUAL UNDER_SCORE BOOLEAN CLASS ELSE IF MAIN PUBLIC STRING VOID SYSTEM_OUT_PRINTLN WHILE EXTENDS
%token <std::string> RETURN STATIC LEFT_CURLY RIGHT_CURLY SEMI_COLON COMMA DIVIDE 
%token <std::string> IDENTIFIER
%token END 0 "end of file"

/* Operator precedence and associativity rules */
/* Used to resolve ambiguities in parsing expressions See https://www.gnu.org/software/bison/manual/bison.html#Precedence-Decl */ 

/* Sätt den med högst pritority längst ner */

%left OR /* bitwise OR */
%left AND /* logical AND */
%left EQUAL /* equality */
%left LESS_THAN MORE_THAN /* relational */
%left PLUSOP MINUSOP /* additive */
%left MULTOP /* multiplicative */
%left LP RP LEFT_BRACKET RIGHT_BRACKET /* parentheses */


/* Specify types for non-terminals in the grammar */
/* The type specifies the data type of the values associated with these non-terminals */
%type <Node *> root expression factor identifier statement reqStatement

/* Grammar rules section */
/* This section defines the production rules for the language being parsed */
%%
root:       expression {root = $1;};

/* LEFT_BRACKET statement RIGHT_BRACKET */

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
            | expression AND expression {
				$$ = new Node("AND", "", yylineno);
				$$->children.push_back($1);
				$$->children.push_back($3);
			}
			| expression OR expression {
				$$ = new Node("OR", "", yylineno);
				$$->children.push_back($1);
				$$->children.push_back($3);
			}
			| expression LESS_THAN expression {
				$$ = new Node("LESS_THAN", "", yylineno);
				$$->children.push_back($1);
				$$->children.push_back($3);
			}
			| expression MORE_THAN expression {
				$$ = new Node("MORE_THAN", "", yylineno);
				$$->children.push_back($1);
				$$->children.push_back($3);
			}
			| expression EQUAL expression {
				$$ = new Node("EQUAL", "", yylineno);
				$$->children.push_back($1);
				$$->children.push_back($3);
			}
			| TRUE {
				$$ = new Node("TRUE", "", yylineno);
			}
			| FALSE {
				$$ = new Node("FALSE", "", yylineno);
			}
			| THIS {
				$$ = new Node("THIS", "", yylineno);
			}
			| NEW {
				$$ = new Node("NEW", "", yylineno);
			}
			| IDENTIFIER {
				$$ = new Node("IDENTIFIER", "", yylineno);
			}
			| expression DOT LENGTH {
				$$ = new Node("exp DOT LENGTH", "", yylineno); /* fix later */
        $$->children.push_back($1); /* expression */
			}
			| NEW INT LEFT_BRACKET expression RIGHT_BRACKET 
			| NEW identifier LP RP
			| EXCLAMATION_MARK {
				$$ = new Node("EXCLAMATION_MARK", "", yylineno);
			}
			| expression LEFT_BRACKET expression RIGHT_BRACKET {
				$$ = new Node("exp LEFT_BRACKET exp RIGHT_BRACKET", "", yylineno);
				$$->children.push_back($1); /* expression */
				$$->children.push_back($3); /* expression */
			}
			| expression DOT identifier LP expression
			| INT
      		| factor      {$$ = $1; /* printf("r4 ");*/}
      		;

reqStatement: %empty 
			| reqStatement statement {
				$$ = new Node("statement", "", yylineno);
				$$->children.push_back($1);
				$$->children.push_back($2);
			}
			;
statement: LEFT_BRACKET reqStatement RIGHT_BRACKET{
				$$ = new Node("statement", $2, yylineno);
			}
			| IDENTIFIER EQUAL expression SEMI_COLON {
				$$ = new Node("AssignmentStatement", "", yylineno);
				

			}
			;

goal: 

reqclassdeclaration: %empty
					| reqclassdeclaration classdeclaration

classdeclaration: CLASS identifier LEFT_CURLY vardeclaration methodDeclaration RIGHT_CURLY

reqvardeclaration: %empty
				| reqvardeclaration vardeclaration

vardeclaration: 

reqmethodDeclaration: %empty
					| reqmethodDeclaration methodDeclaration

methodDeclaration: 

mainclass: classdeclaration

identifier: IDENTIFIER {
			$$ = new Node("identifier", "", yylineno);
		}
		;

/* ez */
type: INT LEFT_BRACKET RIGHT_BRACKET
	| BOOLEAN
	| INT
	| identifier
	;

factor:     INT           {  $$ = new Node("Int", $1, yylineno); /* printf("r5 ");  Here we create a leaf node Int. The value of the leaf node is $1 */}
			 
            | LP expression RP { $$ = $2; /* printf("r6 ");  simply return the expression */}
    ;
