#define _LARGEFILE64_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stddef.h>
#include <sys/types.h>

#define FILE_MAPS_PATH "/proc/%d/maps"
#define FILE_MEM_PATH "/proc/%d/mem"

/*
holds information about a processes's virtual memory mappings
e.g where the stack, heap is etc;
*/
typedef struct {
    char addr[50];
    char perms[50];
    char map_name[50];
} maps_content_t;


int main(int argc, char *argv[])
{
    (void)argc;

    int pid;
    const char *search_string, *replacement_string;

    char mem_path[20], maps_path[20];
    FILE *mem_fd, *maps_fd;
    maps_content_t m = {0};

    char *heap_start, *heap_end;
    unsigned long start_index;
    unsigned long end_index;
    size_t bytes_in_heap;

    pid = atoi(argv[1]);
    search_string = strdup(argv[2]);
    if (search_string == NULL)
    {
        perror("malloc search string");
        exit(1);
    }
    
    replacement_string = strdup(argv[3]);
    if (replacement_string == NULL)
    {
        perror("malloc replacement sttring");
        exit(1);
    }

    sprintf(maps_path, FILE_MAPS_PATH, pid);
    sprintf(mem_path, FILE_MEM_PATH, pid);

    maps_fd = fopen(maps_path, "r");
    if (maps_fd == NULL)
    {
        perror("opening /proc/pid/maps");
        exit(1);
    }

    mem_fd = fopen(mem_path, "rb+");
    if (mem_fd == NULL)
    {
        perror("opening /proc/pid/mem");
        exit(1);
    }

    while(1)
    {
        fscanf(maps_fd, "%s %s %*s %*s %*s %s",
                m.addr, m.perms, m.map_name);
        if (strcmp(m.map_name, "[heap]") == 0)
        {
            break;
        }
    }

    heap_start = strtok(m.addr, "-");
    heap_end = strtok(NULL, "-");

    printf("%s\n", m.map_name);
    printf("%s     %s\n", heap_start, heap_end);
   

    start_index = strtoul(heap_start, NULL, 16);
    end_index = strtoul(heap_end, NULL, 16);
    bytes_in_heap = end_index - start_index;

    printf("indeces: %lu    %lu\n", start_index, end_index);
    printf("bytes in heap: %lu\n", bytes_in_heap);
    
    if (fseek(mem_fd, start_index, SEEK_SET) != 0)
    {
        perror("seeking mapped memory");
        exit(1);
    }  

    char buf[bytes_in_heap];
    size_t b = fread(buf, sizeof(char), bytes_in_heap, mem_fd);
    if (b < bytes_in_heap) {
        perror("reading memory into buffer");
        exit(1);
    }

    printf("bytes read from heap: %lu\n", b);

    // for (size_t i = 0; i < bytes_in_heap; i++) {
    //     printf("%02x ", (unsigned char)buf[i]);
    //     if ((i + 1) % 16 == 0) {
    //         printf("\n");
    //     }
    // }
    // printf("\n");

    char *search_string_addr;
    size_t search_string_len = strlen(search_string);
    printf("ss: %s    rp: %s %lu\n", search_string, replacement_string, search_string_len);
    for (size_t i = 0; i < bytes_in_heap; i++)
    {
        // if ((i + search_string_len) >= bytes_in_heap)
        // {
        //     break;
        // }

        if (memcmp(buf+i, search_string, search_string_len) == 0)
        {
            printf("heyyy: %s\n", "ss");
            search_string_addr = (char *)(start_index + i);
            printf("search string found at: %p\n", search_string_addr);
            printf("found string: %s\n", buf+i);
            break;
        }
    }

    if (search_string_addr != NULL)
    {
        unsigned long ssa = (unsigned long)search_string_addr;
        if (fseek(mem_fd, ssa, SEEK_SET) != 0)
        {
            perror("seeking heap memory for writing");
            exit(1);
        }

        size_t b = fwrite(replacement_string, sizeof(char), search_string_len, mem_fd);
        if (b != search_string_len)
        {
            perror("writing to heap memory");
            exit(1);
        } 

        printf("bytes written to heap: %lu\n", b);
    }

    return 0;

}