_baseaddr equ 0x400000
[BITS 64]
db 0x7f, 'ELF' ; ident=MAGIC
db 2,1,1       ; CLASS64,DATA2LDB,EV_CURRENT
times 9 db 0
dw 0x02        ; type=ET_EXEC
dw 0x3e        ; machine=EM_X86_64
dd 0x01        ; version=EV_CURRENT
dq _start+_baseaddr      ; entry
dq _phdr       ; phoff
dq 0           ; shoff=0 not needed
dd 0x0         ; flags=0
dw 0x40        ; ehsize
dw 0x38        ; phentsize
dw 0x01        ; phnum=1
dw 0           ; shentsize
dw 0           ; shnum
dw 0           ; shstrndx
_phdr:
dd 1           ; type=PT_LOAD
dd 5           ; flags=PF_X|PF_R
dq _start      ; offset
dq _start+_baseaddr      ; vaddr
dq _start      ; paddr
dq _filesize   ; filesz
dq _filesize   ; memsz
dq 0x1000      ; align
_start:
_filesize equ $-$$
