all:
	gcc -o chao main.c parser.c lexer.c
	
clean:
	rm chao
