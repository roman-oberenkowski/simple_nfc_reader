compile:
	gcc -o reader.o -lnfc reader.c
run: compile
	./reader.o