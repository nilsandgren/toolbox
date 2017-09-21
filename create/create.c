#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <stdint.h>

void print_usage(char * exec_path);
int write_file(FILE * output, int64_t size);

int main(int argc, char * argv[])
{
    if (argc != 3)
    {
        print_usage(argv[0]);
        return 1;
    }

    char * name = argv[1];
    int64_t size = atoll(argv[2]);
    if (size <= 0LL)
    {
        fprintf(stderr, "Invalid file size: %s\n", argv[2]);
        return 1;
    }

    FILE * output = fopen(name, "wb");

    if (write_file(output, size))
    {
        fprintf(stderr, "Error writing to file\n");
        fclose(output);
        return 1;
    }

    fclose(output);

    return 0;
}

int write_file(FILE * output, int64_t size)
{
    const size_t buffer_size = 2048;
    uint8_t buffer[buffer_size];

    int64_t chunks = size / buffer_size;
    int64_t rest = size % buffer_size;

    for (int i = 0; i < buffer_size; i++)
    {
        buffer[i] = i & 0xff;
    }

    for (int i = 0; i < chunks; i++)
    {
        fwrite(buffer, 1, buffer_size, output);
    }

    fwrite(buffer, 1, rest, output);

    return 0;
}

void print_usage(char * exec_path)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "  Usage: %s <name> <size>\n", basename(exec_path));
    fprintf(stderr, "\n");
}

