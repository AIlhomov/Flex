/* 

my own lexicon of errors:

segmentation faults: { } DO NOT USE EMPTY BRACKETS
warning: nonterminal useless in grammar: does not appear in any other rule, and is thus not needed for the grammar.
perhaps mention it ? like at the root.

*/

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
%token <std::string> DOT LENGTH EXCLAMATION_MARK LEFT_CURLY RIGHT_CURLY
/* not done */
%token <std::string> ASSIGN MORE_THAN_EQUAL LESS_THAN_EQUAL UNDER_SCORE BOOLEAN CLASS ELSE IF MAIN PUBLIC STRING VOID SYSTEM_OUT_PRINTLN WHILE EXTENDS
%token <std::string> RETURN STATIC SEMI_COLON COMMA DIVIDE
/* Regular Expressions: (REGEX) */
%token <std::string> IDENTIFIER
%token <std::string> INTEGER_LITERAL /* how could i forget this */

%token END 0 "end of file"

/* Operator precedence and associativity rules */
/* Used to resolve ambiguities in parsing expressions See https://www.gnu.org/software/bison/manual/bison.html#Precedence-Decl */ 

/* Sätt den med högst pritority längst ner */

%right ASSIGN
%left OR /* bitwise OR */
%left AND /* logical AND */
%right EXCLAMATION_MARK /* logical AND */

%left EQUAL /* equality */
%left LESS_THAN MORE_THAN /* relational */
%left PLUSOP MINUSOP /* additive */
%left MULTOP /* multiplicative */
%left LP RP LEFT_BRACKET RIGHT_BRACKET /* parentheses */


/* Specify types for non-terminals in the grammar */
/* The type specifies the data type of the values associated with these non-terminals */
%type <Node *> root expression factor identifier type statement reqStatement mainClass varDeclaration reqVarOrStmt reqMethodDeclaration argument_list
%type <Node *> methodDeclaration reqVarDeclaration classDeclaration parameters parameter_list goal singleClassDeclaration arguments 
%type <Node *> retSTMT dotExpTest
/* Grammar rules section */
/* This section defines the production rules for the language being parsed */
%%
root:        /* remove these later (debug only), have only goal left */
	goal {root = $1; }
		;
		
goal: 
mainClass  END { 
		$$ = new Node("goal", "", yylineno); 
		$$->children.push_back($1);
	}
|mainClass classDeclaration END { 
		$$ = new Node("goal", "", yylineno); 
		$$->children.push_back($1);
		$$->children.push_back($2);
	}
	;


classDeclaration: singleClassDeclaration  { 
			$$ = $1;
			}
				
				;
				

singleClassDeclaration: CLASS identifier LEFT_CURLY varDeclaration 
					methodDeclaration RIGHT_CURLY  {
						$$ = new Node("classDeclarations", "", yylineno);
						Node* classes =	new Node("classDeclaration", $2->value , yylineno);
						
						classes->children.push_back($2);
						classes->children.push_back($4);
						classes->children.push_back($5);

						$$->children.push_back(classes);
					}
					| // make a recursive

					singleClassDeclaration CLASS identifier LEFT_CURLY varDeclaration 
					reqMethodDeclaration RIGHT_CURLY {
						$$ = $1; // singleClassDeclaration
						//$$ = new Node("ClassDeclarationsHERERERERE", "", yylineno);

						Node* classes = new Node("classDeclaration", $3->value, yylineno);

						classes->children.push_back($3);
						classes->children.push_back($5);
						classes->children.push_back($6);

						$$->children.push_back(classes);
					}
					;








type: INT LEFT_BRACKET RIGHT_BRACKET { $$ = new Node("INT LB RB", "", yylineno); }
	| BOOLEAN { $$ = new Node("BOOLEAN", "", yylineno); }
	| INT { $$ = new Node("INT", "", yylineno); }
	| identifier { $$ = new Node("typechar", "", yylineno); $$->children.push_back($1); }
	;


/*

	LEFT_BRACKET [
	RIGHT_BRACKET ]
	LEFT_CURLY {
	RIGHT_CURLY }
	LP (
	RP )

*/

/* done */
mainClass: PUBLIC CLASS identifier LEFT_CURLY PUBLIC STATIC VOID MAIN LP STRING /* main class */
		   LEFT_BRACKET RIGHT_BRACKET identifier RP LEFT_CURLY reqStatement RIGHT_CURLY RIGHT_CURLY 
		   {
				$$ = new Node("MAIN CLASS", "", yylineno);

				Node* methods = new Node("MAIN METHOD", "", yylineno);

				

				$$->children.push_back($3);


				$$->children.push_back(methods);
				methods->children.push_back($13);
				methods->children.push_back($16);
				
				
		   }
		   ;
				/* 
				class fac { 
					int a; 
					
					public int a(int b, int c){
						int d;
						return false;
					}
				} 
				*/

				


varDeclaration: %empty { $$ = new Node("var declarations", "", yylineno); }
				| reqVarDeclaration { $$ = $1; }
				;
			;
			
reqVarDeclaration: type identifier SEMI_COLON {
		
				$$ = new Node("var declarations", "", yylineno);
				Node * varDec = new Node("var declaration", "", yylineno);
				varDec->children.push_back($1);
				varDec->children.push_back($2);

				$$->children.push_back(varDec);
			}
				| reqVarDeclaration type identifier SEMI_COLON {
				$$ = $1;
				Node* varDec = new Node("var declaration", "", yylineno);
				varDec->children.push_back($2);
				varDec->children.push_back($3);

				$$->children.push_back(varDec);
		
			}
				;


retSTMT: RETURN  { $$ = new Node("RETURN SEMI_COLON", "", yylineno); }
		| 
			RETURN expression SEMI_COLON { 
			$$ = new Node("RETURN", "", yylineno);
			$$->children.push_back($2);
		}
		
		;


/* the "?" is answered here, thank you */
reqVarOrStmt: %empty {	$$ = new Node("methodBody", "", yylineno); }
            | reqVarOrStmt type identifier SEMI_COLON {
				$$ = $1;
				Node* varDec = new Node("var declaration", "", yylineno);
				varDec->children.push_back($2);
				varDec->children.push_back($3);

				$$->children.push_back(varDec);
			
			} 



            | reqVarOrStmt statement { 
				$$ = $1;
				Node* varDec = new Node("statement", "", yylineno);
				varDec->children.push_back($2);

				$$->children.push_back(varDec);
	
			}
            ;



parameters: %empty { $$ = new Node("parameters", "", yylineno);  }
          | parameter_list { $$ = $1;}
          ;

parameter_list: type identifier { 
				$$ = new Node("parameters", "", yylineno);
				Node* params = new Node("parameter", "", yylineno);

				params->children.push_back($1);
				params->children.push_back($2);

				$$->children.push_back(params);
			}
              | parameter_list COMMA type identifier { 
				$$ = $1;
				Node* params = new Node("parameter", "", yylineno);

				params->children.push_back($3);
				params->children.push_back($4);
				
				$$->children.push_back(params);

			  }
              ;








methodDeclaration:  reqMethodDeclaration { $$ = $1; }
					;


reqMethodDeclaration: PUBLIC type identifier LP parameters
					RP LEFT_CURLY reqVarOrStmt 
					retSTMT RIGHT_CURLY
					{
						$$ = new Node("methodDeclarations", "", yylineno);
						Node * methDec = new Node("methodDec", $3->value, yylineno);
						methDec->children.push_back($2);
						methDec->children.push_back($3);
						methDec->children.push_back($5);
						methDec->children.push_back($8);
						methDec->children.push_back($9);

						$$->children.push_back(methDec);
						


					}
					| reqMethodDeclaration PUBLIC type identifier LP parameters
					RP LEFT_CURLY reqVarOrStmt 
					retSTMT RIGHT_CURLY
					{
						$$ = $1;
						Node * methDec = new Node("methodDec", $4->value, yylineno);
						methDec->children.push_back($3);
						methDec->children.push_back($4);
						methDec->children.push_back($6);
						methDec->children.push_back($9);
						methDec->children.push_back($10);

						$$->children.push_back(methDec);
					}
                    |
                    %empty { $$ = new Node("NOOOOOO methodDeclarations", "", yylineno); }
					;




statement: LEFT_CURLY reqStatement RIGHT_CURLY { /* recursive "*" */
				$$ = new Node("LC statement RC", "", yylineno);
				$$->children.push_back($2);


			}

			| IF LP expression RP statement {/* if without else */
				$$ = new Node("IF LP expression RP statement", "", yylineno);
            	$$->children.push_back($3);
            	$$->children.push_back($5);
			}
			| IF LP expression RP statement ELSE statement { /* special with "?" ? */
				$$ = new Node("IF LP expression RP statement ELSE statement", "", yylineno);
				$$->children.push_back($3);
				Node* stmt = new Node("statements", "", yylineno);
				stmt->children.push_back($5);
				$$->children.push_back(stmt);

			Node* secStmt = new Node("statements", "", yylineno);
				secStmt->children.push_back($7);
				$$->children.push_back(secStmt);

			}

			| IF LP expression RP statement ELSE LEFT_CURLY RIGHT_CURLY { /* special with "?" ? */
				$$ = new Node("IF LP expression RP statement ELSE statement", "", yylineno);
				$$->children.push_back($3);
				Node* stmt = new Node("statements", "", yylineno);
				stmt->children.push_back($5);
				$$->children.push_back(stmt);

			Node* secStmt = new Node("empty statements", "", yylineno);
				$$->children.push_back(secStmt);

			}
			| WHILE LP expression RP statement { /* ( " else " Statement ) ? */
				$$ = new Node("WHILE LP expression RP statement", "", yylineno);
				$$->children.push_back($3);
				$$->children.push_back($5);
			}
			| SYSTEM_OUT_PRINTLN LP expression RP SEMI_COLON {
				$$ = new Node("SIMPLE PRINT LOL", "", yylineno);
				$$->children.push_back($3);
			}
			| identifier ASSIGN expression SEMI_COLON {
				$$ = new Node("SOMETHING ASSIGNED = TO SOMETHING", "", yylineno);
				$$->children.push_back($1);
				$$->children.push_back($3);
			}
			| identifier LEFT_BRACKET expression RIGHT_BRACKET ASSIGN expression SEMI_COLON {
				$$ = new Node("SOMETHING [ASSIGNED] = TO SOMETHING", "", yylineno);
				$$->children.push_back($1);
				$$->children.push_back($3);
				$$->children.push_back($6);
			}
			;


reqStatement: statement{
				$$ = new Node("statements", "", yylineno);
				Node * reqSTMT = new Node("statement", "", yylineno);
				reqSTMT->children.push_back($1);
				$$->children.push_back(reqSTMT);
			}

			| reqStatement statement {
				$$ = $1;				
				Node * reqSTMT = new Node("statement", "", yylineno);
				reqSTMT->children.push_back($2);
				$$->children.push_back(reqSTMT);

			}
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



			/* | < INTEGER_LITERAL > */ /* done ? (down below) */

			| TRUE {
				$$ = new Node("TRUE", "True", yylineno);
			}
			| FALSE {
				$$ = new Node("FALSE", "False", yylineno);
			}

			/* fix this later (idk) */
			/* | identifier {
				$$ = new Node("Identifier", "", yylineno);
			} */

			| THIS {
				$$ = new Node("THIS", "", yylineno);
			}
			| NEW INT LEFT_BRACKET expression RIGHT_BRACKET {
				$$ = new Node("NEW INT LEFT_BRACKET expression RIGHT_BRACKET", "", yylineno);
				$$->children.push_back($4);
			}
			
			/* fixed? */
			/* | " new " Identifier " ( " " ) " */
			| NEW identifier LP RP { $$ = new Node("NEW identifier LP RP", "", yylineno);
									 $$->children.push_back($2);
			}


			| EXCLAMATION_MARK expression {
				$$ = new Node("EXCLAMATION_MARK expression", "", yylineno);
				$$->children.push_back($2);
			}

			/* should i have this ? (it exists in factor already) */
			/* | LP expression RP {
				$$ = new Node("LEFT_CURLY expression RIGHT_CURLY", "", yylineno);
				$$->children.push_back($2);
			} */

			/* Regular Expressions */
      		| factor      { $$ = $1; /* printf("r4 ");*/ } /* for integers */
			| identifier { /* empty because we have it in root */ $$ = $1;  } /* for chars */

			/* fix this later */
			/* | Expression " . " Identifier " ( " ( Expression ( " ," Expression ) * ) ? " ) " */
			| expression DOT identifier LP //wtf will happen now (req or smthn)
			arguments RP {
				$$ = new Node("exp DOT ident LP exp COMMA exp RP", "", yylineno);
				$$->children.push_back($1); /* `new A()` */
        		$$->children.push_back($3); /* `a2` */
				$$->children.push_back($5);
				
			}
			| expression DOT identifier LP //wtf will happen now (req or smthn)
			arguments RP dotExpTest {
				$$ = new Node("functionCall", "", yylineno);
				$$->children.push_back($1); /* `new A()` */
        		$$->children.push_back($3); /* `a2` */
				$$->children.push_back($5);
				$$->children.push_back($7);
				
			}
			/* hello.a(2,b,a,f,d,s,1,2,4,a,s) */
      		;


			

dotExpTest: DOT identifier LP arguments RP { $$ = new Node("DOT identifier LP arguments RP", "", yylineno);
											  $$->children.push_back($2);
											  $$->children.push_back($4);
											}
			|
			DOT identifier LP arguments RP dotExpTest { $$ = new Node("DOT identifier LP arguments RP dotExpTest", "", yylineno);
														$$->children.push_back($2);
														$$->children.push_back($4);
														$$->children.push_back($6);
				}
			|DOT LENGTH { $$ = new Node("DOT LENGTH", "", yylineno); }

			
			;
			

arguments: %empty { $$ = new Node("empty argument", "", yylineno); }
         | argument_list { $$ = $1; }
         ;

argument_list: expression { $$ = new Node("argument_list", "", yylineno);  $$->children.push_back($1);}
             | argument_list COMMA expression {
				$$ = $1;
				Node* arg = new Node("argument", "", yylineno);
				arg->children.push_back($3);
				$$->children.push_back(arg);
			 
			 }
             ;



identifier: IDENTIFIER { $$ = new Node("identifier", $1, yylineno); }
		    ; 





factor:     INTEGER_LITERAL           {  $$ = new Node("INT", $1, yylineno); /* printf("r5 ");  Here we create a leaf node Int. The value of the leaf node is $1 */}
			 
            | LP expression RP { $$ = $2; /* printf("r6 ");  simply return the expression */}
    ;