# Elftoy
Practicing "elfmaster" teachings for fun.

It's been tested only in my system.
This includes a python extension with high level functions (combination of many techniques).

## Requirements
- yasm
- gcc
- python

## Examples

### PT\_NOTE injection + entry point redirection
- Prepare examples
```bash
$ pip install -e . # install this (I recommend using a venv)
...

$ gcc -o hello elf/hello.c # example binary target
...

# check before modifying it
$ ./hello
Hello

$ yasm -o injectme elf/injectme.asm # example payload
```
- Do injection using python extension
```python
>>> import elftoy
>>> elftoy.elf.inject_PT_NOTE_highjack_entry('hello', 'injectme')
...

>>>
```
- Check that it worked
```bash
$ ./hello
Inject me
Hello
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
