#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[])
{
    FILE * input = NULL;

    long long int bytes = 0;
    long long int offset = 0;

    if (argc != 3)
    {
        fprintf(stderr, "\n  Read a chunk of data from an input file and print ");
        fprintf(stderr, "it to stdout.\n\n");
        fprintf(stderr, "  Usage:    chunk size@offset filename\n");
        fprintf(stderr, "  Example:  chunk 1024@123456 file.dat > chunk.dat\n\n");
        return 1;
    }

    input = fopen(argv[2], "r");
    if (input == NULL)
    {
        fprintf(stderr, "error: Could not open \"%s\"\n", argv[2]);
        return 1;
    }

    // Split bytes@offset
    {
        char * bytes_string = argv[1];
        char * offset_string = strtok(argv[1], "@");

        offset_string = strtok(NULL, "@");
        if (offset_string == NULL)
        {
            fprintf(stderr, "error: Could not parse size@offset argument\n");
            goto error_exit;
        }
        bytes = strtoull(bytes_string, NULL, 10);
        offset = strtoull(offset_string, NULL, 10);
        if (bytes <= 0)
        {
            fprintf(stderr, "error: Number of bytes is <= 0\n");
            goto error_exit;
        }
        if (offset < 0)
        {
            fprintf(stderr, "error: Offset is < 0\n");
            goto error_exit;
        }
    }

    // Read the input
    {
        long long int bytes_to_read = bytes;
        const size_t buffer_size = 2048;
        unsigned char buffer[buffer_size];
        size_t file_size = 0;

        fseek(input, 0L, SEEK_END);
        file_size = ftell(input);

        if (offset >= file_size)
        {
            fprintf(stderr, "error: Offset is beyond end of file.\n");
            goto error_exit;
        }

        fseek(input, offset, SEEK_SET);
        if (offset + bytes > file_size)
        {
            fprintf(stderr, "warning: Chunk is beyond end of file. Trimming.\n");
            bytes_to_read = file_size - offset;
        }

        while(bytes_to_read)
        {
            if (bytes_to_read >= buffer_size)
            {
                fread(buffer, 1, buffer_size, input);
                fwrite(buffer, 1, buffer_size, stdout);
                bytes_to_read -= buffer_size;
            }
            else
            {
                fread(buffer, 1, bytes_to_read, input);
                fwrite(buffer, 1, bytes_to_read, stdout);
                bytes_to_read = 0;
            }
        }
    }

    fclose(input);
    return 0;

error_exit:

    fclose(input);
    return 1;
}
