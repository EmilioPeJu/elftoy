
extern int get_val();
extern int get_val1();
extern int get_val2();
extern int get_val3();
extern int get_val4();

int _start()
{
    int result = get_val();
    if (!result)
        result = get_val1();
    if (!result)
        result = get_val2();
    if (!result)
        result = get_val3();
    if (!result)
        result = get_val4();
    asm("mov %%rax, %%rdi;"
        "mov $60, %%rax;"
        "syscall"
        :
        : "a"(result));
    return 0;
}

