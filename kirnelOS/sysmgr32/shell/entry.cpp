[[gnu::section(".text.shell_start")]]
void shell_start()
{
    volatile int a = 1, b = 0;
    a = a / b;

    while (1)
        ;
}
