all: elffunctions

elffunctions: elfutils.o elfinjections.o elfhijacking.o elfpatching.o elffunctions.o
	gcc -DQUICK_TEST -g -o $@ $^

hello: hello.o
	gcc -g -o $@ $^

injectme: injectme.asm
	yasm -o $@ $^

%.o: %.c
	gcc -DQUICK_TEST -g -c $^

clean:
	rm -rf *.o elffunctions hello injectme check_elfutils

check_elfutils: elfutils.o check_elfutils.o
	gcc -o "$@" -lcheck $^
	./$@

test: hello injectme all
	gcc -o hello hello.c
	./elffunctions hello injectme
	./hello

