#!/bin/bash
yasm -f elf64 -o libtiny.o ../elf/libtiny.asm
gcc -shared -fpic -o libtiny.so libtiny.o
gcc -nostartfiles -nostdlib -no-pie -ltiny -L$(pwd) \
    -Wl,-rpath,$(pwd) -o tinybin_usinglib ../elf/tinybin_usinglib.c
yasm -o injectme ../elf/injectme.asm
./tinybin_usinglib
echo $?
virtualenv test-venv
source test-venv/bin/activate
pip install -e ..
python <<EOF
import elftoy
segment_addr = elftoy.elf.inject_note_segment('tinybin_usinglib', 'injectme')
orig_addr = elftoy.elf.hijack_got('tinybin_usinglib', 'get_val', segment_addr)
elftoy.elf.patch_jmp('tinybin_usinglib', orig_addr, segment_addr)
EOF
