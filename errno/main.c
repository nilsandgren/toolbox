#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>

int main(int argc, char * argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "\n");
        fprintf(stderr, "  Usage: %s <errno>\n", basename(argv[0]));
        fprintf(stderr, "\n");
        return 1;
    }
    int error = atoi(argv[1]);
    fprintf(stdout, "%d => %s\n", error, strerror(error));

    return 0;
}
