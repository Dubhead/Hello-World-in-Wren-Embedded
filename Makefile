# Makefile

all: hello

hello: hello.o libwren.a
	gcc hello.c libwren.a -o $@ -lm

hello.o: hello.c wren.h
	gcc -c hello.c

clean:
	rm -f hello hello.o
