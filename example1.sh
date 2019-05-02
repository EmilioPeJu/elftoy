#!/bin/bash
gcc -o hello elf/hello.c
./hello
yasm -o injectme elf/injectme.asm
virtualenv test-venv
source test-venv/bin/activate
pip install -e .
python <<EOF
import elftoy
elftoy.elf.inject_PT_NOTE_hijack_entry('hello', 'injectme')
EOF
./hello
