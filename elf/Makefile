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
	rm -rf *.o elffunctions hello injectme check_elfutils tinybin tinybin_usinglib \
	    check_elfhijacking libtiny.so

tinyelfheader: tinyelfheader.asm
	yasm "$<"

%.o: %.asm
	yasm -f elf64 -o "$@" "$^"

tinybin_usinglib.o: tinybin_usinglib.c
	gcc -c $^

tinybin: tinybin.o
	ld -o "$@" "$^"
	chmod +x "$@"

tinybin_usinglib: tinybin_usinglib.o libtiny.so
	gcc -nostartfiles -nostdlib -no-pie -ltiny -L$(shell pwd) -Wl,-rpath,$(shell pwd) \
	    -o "$@" "$<"
	chmod +x "$@"

libtiny.so: libtiny.o
	gcc -fpic -shared -o "$@" "$^"

check_elfutils: elfutils.o check_elfutils.o
	gcc -o "$@" -lcheck $^
	./$@

check_elfhijacking: elfutils.o elfhijacking.o check_elfhijacking.o
	gcc -o "$@" -lcheck $^
	./$@

test: check_elfhijacking check_elfutils

