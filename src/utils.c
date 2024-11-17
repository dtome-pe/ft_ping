#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void    error(char *str)
{
    write(2, str, strlen(str));
    exit(1);
}