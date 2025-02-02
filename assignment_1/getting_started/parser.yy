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
%token <std::string> INTEGER_LITERAL /* how could i forget this */
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
%type <Node *> root expression factor identifier 

/* Grammar rules section */
/* This section defines the production rules for the language being parsed */
%%
root:       expression {root = $1;}
			;



/* LEFT_BRACKET statement RIGHT_BRACKET */

/* EXPRESSION ALL DONE (LOOK AT COMMENTS) */
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
			| expression LEFT_BRACKET expression RIGHT_BRACKET {
				$$ = new Node("expression LEFT_BRACKET expression RIGHT_BRACKET", "", yylineno);
				$$->children.push_back($1);
				$$->children.push_back($3);
			}
			| expression DOT LENGTH {
				$$ = new Node("expression DOT LENGTH", "", yylineno);
				$$->children.push_back($1);
			}

			/* fix this later */
			/* | Expression " . " Identifier " ( " ( Expression ( " ," Expression ) * ) ? " ) " */
			/* | < INTEGER_LITERAL > */

			| TRUE {
				$$ = new Node("TRUE", "", yylineno);
			}
			| FALSE {
				$$ = new Node("FALSE", "", yylineno);
			}

			/* fix this later */
			/* | Identifier */

			| THIS {
				$$ = new Node("THIS", "", yylineno);
			}
			| NEW INT LEFT_BRACKET expression RIGHT_BRACKET {
				$$ = new Node("NEW INT LEFT_BRACKET expression RIGHT_BRACKET", "", yylineno);
				$$->children.push_back($4);
			}

			/* fix this later */
			/* | " new " Identifier " ( " " ) " */

			| EXCLAMATION_MARK expression {
				$$ = new Node("EXCLAMATION_MARK expression", "", yylineno);
				$$->children.push_back($2);
			}
			| LEFT_CURLY expression RIGHT_CURLY {
				$$ = new Node("LEFT_CURLY expression RIGHT_CURLY", "", yylineno);
				$$->children.push_back($2);
			}
      		| factor      {$$ = $1; /* printf("r4 ");*/}
      		;


identifier: IDENTIFIER { $$ = new Node("identifier", "", yylineno); }
		    ; 





factor:     INTEGER_LITERAL           {  $$ = new Node("Int", $1, yylineno); /* printf("r5 ");  Here we create a leaf node Int. The value of the leaf node is $1 */}
			 
            | LP expression RP { $$ = $2; /* printf("r6 ");  simply return the expression */}
    ;




/* 
reqStatement: %empty 
			| reqStatement statement {
				$$ = new Node("statement", "", yylineno);
				$$->children.push_back($1);
				$$->children.push_back($2);
			}
			;
statement: LEFT_BRACKET reqStatement RIGHT_BRACKET{
				$$ = new Node("statement", "", yylineno);
			}
			| INT IDENTIFIER ASSIGN INTEGER_LITERAL SEMI_COLON {  int x = 5; 
				$$ = new Node("AssignmentStatement", "", yylineno);  come back later (WTF IS THIS)			
				$$->children.push_back(new Node("IDENTIFIER", $2, yylineno)); // Identifier: x
			}
			; */


/* reqclassdeclaration: %empty
					| reqclassdeclaration classdeclaration

classdeclaration: CLASS identifier LEFT_CURLY vardeclaration methodDeclaration RIGHT_CURLY

reqvardeclaration: %empty
				| reqvardeclaration vardeclaration

vardeclaration: 

reqmethodDeclaration: %empty
					| reqmethodDeclaration methodDeclaration

methodDeclaration: 

mainclass: classdeclaration */


/* ez */
/* type: INT LEFT_BRACKET RIGHT_BRACKET
	| BOOLEAN
	| INT
	| identifier
	; */	
