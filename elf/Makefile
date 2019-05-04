all: elffunctions

elffunctions: elfutils.o elfinjections.o elfhijacking.o elfpatching.o elffunctions.o
	gcc -g -o $@ $^

hello: hello.o
	gcc -g -o $@ $^

injectme: injectme.asm
	yasm -o $@ $^

%.o: %.c
	gcc -g -c $^

clean:
	rm -rf *.o elffunctions hello injectme check_elfutils tinybin \
	    check_elfhijacking

tinybin.o: tinybin.asm
	yasm -f elf64 -o "$@" "$^"

tinybin: tinybin.o
	ld -o "$@" "$^"

check_elfutils: elfutils.o check_elfutils.o
	gcc -o "$@" -lcheck $^
	./$@

check_elfhijacking: elfutils.o elfhijacking.o check_elfhijacking.o
	gcc -o "$@" -lcheck $^
	./$@

test: check_elfhijacking check_elfutils

