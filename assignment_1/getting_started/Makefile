# Compiler executable
compiler: lex.yy.c parser.tab.o main.cc Interpreter.cc
	g++ -g -w -o compiler parser.tab.o lex.yy.c main.cc Interpreter.cc -std=c++14

parser.tab.o: parser.tab.cc
	g++ -g -w -c parser.tab.cc -std=c++14

parser.tab.cc: parser.yy
	bison parser.yy

lex.yy.c: lexer.flex parser.tab.cc
	flex lexer.flex

# Interpreter executable
interpreter: Interpreter.cc interpreter_main.cc
	g++ -g -w -o interpreter Interpreter.cc interpreter_main.cc -std=c++14

# Generate tree visualization
tree:
	dot -Tpdf tree.dot -otree.pdf

# Clean build artifacts
clean:
	rm -f parser.tab.* lex.yy.c* compiler interpreter stack.hh position.hh location.hh tree.dot tree.pdf cfg.dot tree2.pdf
	rm -R compiler.dSYM

# Generate CFG visualization
tree2:
	dot -Tpdf cfg.dot -otree2.pdf

# Run the compiler and generate byte-code
s:
	make compiler
	./compiler test.txt
	dot -Tpdf tree.dot -otree.pdf
	dot -Tpdf cfg.dot -otree2.pdf

# Run the interpreter
run-interpreter: interpreter
	./interpreter output.bytecode
