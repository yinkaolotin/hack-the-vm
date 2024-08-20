#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <error.h>

int main()
{
    char *str;

    char  s[] = "Hello, World!";
    size_t l =  strlen(s);
    size_t ll = l + 1;

    str = malloc(ll);
    if (str == NULL)
    {
        perror("allocating for string");
        exit(1);
    }
    
    for(unsigned long i = 0;;i++)
    {
        str[i % l] = s[i % l];
        str[l] = '\0'; 
        printf("%s          %p\n", str, (void*)str);
        sleep(2);
    }

    return 0;
}