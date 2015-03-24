
comp: nodes.h tokens.h parse.c tada.lex print.c comp.c pretty.c
	flex tada.lex
	gcc -c lex.yy.c
	gcc -c parse.c
	gcc -c print.c
	gcc -c pretty.c
	gcc -c comp.c
	gcc -o comp print.o pretty.o lex.yy.o parse.o comp.o

clean:
	rm -rf *.o simp.c lex.yy.c comp
