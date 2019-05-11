[BITS 64]
global _start
section .text
_start:
; endbr64
; db 0xf3, 0x0f, 0x1e, 0xfa
push rax
push rdi
push rsi
push rdx
push rcx
push r11
mov rax, 1 ; write
mov rdi, 1 ; stdout
lea rsi, [rel msg] ; *buf
mov rdx, 10    ; len
syscall
pop r11
pop rcx
pop rdx
pop rsi
pop rdi
pop rax
jmp DWORD end ; for patching purposes
end           ; in case it's not patched, fallback to 
              ; exit syscall
mov rax, 60
mov rdi, 0
syscall
msg:
db 'Inject me',0xa
