#!/bin/bash
gcc -o hello ../elf/hello.c
./hello
yasm -o injectme ../elf/injectme.asm
virtualenv test-venv
source test-venv/bin/activate
pip install -e ..
python <<EOF
import elftoy
addr = elftoy.elf.inject_note_segment('hello', 'injectme')
orig_entry = elftoy.elf.hijack_entry('hello', addr)
elftoy.elf.patch_jmp('hello', orig_entry, addr)
EOF
./hello
