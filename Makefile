jc : jc.c token.c LC4.c token.h LC4.h
	clang -Wall -g -o jc jc.c token.c LC4.c -lm

clean :
	rm -f jc jc.o