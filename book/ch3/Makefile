fb3-1: fb3-1.l fb3-1.y fb3-1.h
	bison -d fb3-1.y
	flex -ofb3-1.lex.c fb3-1.l
	cc -o $@ fb3-1.tab.c fb3-1.lex.c fb3-1funcs.c

fb3-2:	fb3-2.l fb3-2.y fb3-2.h fb3-2func.c
	bison -d fb3-2.y && \
	flex -ofb3-2.lex.c fb3-2.l && \
	cc -g -o $@ fb3-2.tab.c fb3-2.lex.c fb3-2func.c -lm

clean:
	rm -f fb3-1 fb3-1.tab.c fb3-1.tab.h fb3-1.lex.c fb3-2 fb3-2.tab.c fb3-2.tab.h fb3-2.lex.c