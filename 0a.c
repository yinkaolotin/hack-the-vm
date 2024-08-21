#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <error.h>

int main()
{
    char *str;

    str = strdup("Hello");
    if (str == NULL)
    {
        perror("allocating for string");
        exit(1);
    }

    printf("pid: %d\n", pid());
    
    for(;;)
    {
        printf("%s          %p\n", str, (void*)str);
        sleep(2);
    }

    return 0;
}