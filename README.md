# Elftoy
Practicing "elfmaster" teachings for fun.

It's been tested only in my system.
This includes a python extension with high level functions (combination of many techniques).

## Requirements
- yasm
- gcc
- python

## Examples

### note segment injection + entry point redirection
- Prepare examples
```bash
$ gcc -o hello elf/hello.c # example binary target
...

# check before modifying it
$ ./hello
Hello

$ yasm -o injectme elf/injectme.asm # example payload
```
- Inject, hijack and patch using python extension
```python
>>> import elftoy
>>> addr = elftoy.elf.inject_note_segment('hello', 'injectme')
>>> orig_entry = elftoy.elf.hijack_entry('hello', addr)
>>> elftoy.elf.patch_jmp('hello', orig_entry, addr)
>>>
```
- Check that it worked
```bash
$ ./hello
Inject me
Hello
```
### note segment injection + Got poisoning

As this is done in the file, it will only work for non pie binaries

- Prepare examples
```bash
$ yasm -f elf64 -o libtiny.o elf/libtiny.asm
$ gcc -shared -fpic -o libtiny.so libtiny.o
$ gcc -nostartfiles -nostdlib -no-pie -ltiny -L$(pwd) -Wl,-rpath,$(pwd) -o tinybin_usinglib elf/tinybin_usinglib.c
yasm -o injectme elf/injectme.asm
$ ./tinybin_usinglib # example program calling get_val to return 23
$ echo $?
23
```
- Inject, hijack and patch using the python extension
```python
>>> import elftoy
>>> segment_addr = elftoy.elf.inject_note_segment('tinybin_usinglib', 'injectme')
>>> orig_addr = elftoy.elf.hijack_got('tinybin_usinglib', 'get_val', segment_addr)
>>> elftoy.elf.patch_jmp('tinybin_usinglib', orig_addr, segment_addr)
```
- Check that it worked
```bash
$ ./tinybin_usinglib
Inject me
$ echo $?
23
```

## Roadmap
### Injection Techniques
- [x] PT\_NOTE to PT\_LOAD segment conversion (easier than adding a new segment)
- [ ] Reverse Silvio Infection (more stealthy and segment is already executable, avoiding problems of data segment injections)
- [ ] .so injection with dl\_open() helper (easier than manually open/mmap)
- [ ] .so injection with LD\_PRELOAD helper (quick and dirty)
- [ ] ptrace injection at runtime

### Hijacking Techniques
- [x] Overwrite entry point
- [ ] Overwrite function pointers (constructor/destructor)
- [ ] Function trampolines
- [ ] Patch PLT jmp
- [x] GOT poisoning (as it is in a writable segment, it can be done at runtime)
- [ ] Infect data structures

### Analysis Utils
- [ ] Elf parser

### Patching Utils
- [x] Patch jmp in segment

### Debug Utils
- [ ] Binary reconstructor
