#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <libgen.h>
#include <time.h>
#include <ctype.h>

// Read input text string and convert to a number.
// The prefixes b, o, d, and h are used to set the input format.
// If there is no prefix, decimal input is assumed.
uint64_t readInput(const char * input)
{
    size_t input_length = strlen(input);
    if (input_length < 1)
    {
        fprintf(stderr, "Malformed input\n");
        return 0;
    }

    char format = input[0];
    printf("\n");
    switch(format)
    {
        case 'b':
            printf("  input: bin\n");
            return strtoull(input + 1, NULL, 2);
        case 'o':
            printf("  input: oct\n");
            return strtoull(input + 1, NULL, 8);
        case 'd':
            printf("  input: dec\n");
            return strtoull(input + 1, NULL, 10);
        case 'h':
            printf("  input: hex\n");
            return strtoull(input + 1, NULL, 16);
        case '0':
            if (input_length > 2 && input[1] == 'x')
            {
                printf("  input: hex\n");
                return strtoull(input + 2, NULL, 16);
            }
        default:
            printf("  input: dec\n");
            return strtoull(input, NULL, 10);
    }
}


// Binary print of a value
void printBin(uint64_t value)
{
    // Skip leading 0s
    bool skip = true;
    uint64_t mask = 1LLU << 63;

    printf("  bin: ");
    for (uint64_t i = 0; i < 64; i++)
    {
        if (value & mask)
        {
            printf("1");
            skip = false;
        }
        else if (!skip)
        {
            printf("0");
        }
        value <<= 1;
    }
    if (skip)
    {
        printf("0");
    }
    printf("\n");
}

// Print value as a date time string
void printTime(uint64_t value)
{
    time_t tt((long) value);
    char timeString[256];

    tm * time = gmtime(&tt);
    strftime(timeString, 255, "%Y-%m-%d %H:%M:%S", time);
    printf("  gmt time: %s\n", timeString);

    time = localtime(&tt);
    strftime(timeString, 255, "%Y-%m-%d %H:%M:%S", time);
    printf("  loc time: %s\n", timeString);
}

// Print value in various forms
void
printOutput(uint64_t value)
{
    printf("\n");
    printBin(value);
    printf("  oct: %lo\n", value);
    printf("  dec: %lu\n", value);
    printf("  hex: %lx\n", value);
    if (value <= 0xff &&
        isprint(int(value)))
    {
        printf("  asc: '%c'\n", int(value));
    }
    printf("\n");
    printTime(value);
    printf("\n");
}

void
printUsage(char * prog)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "  Print numbers in various formats\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "  Usage\n");
    fprintf(stderr, "         %s <prefix><number>\n", basename(prog));
    fprintf(stderr, "\n");
    fprintf(stderr, "  Number prefix sets input format:\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "    b: binary\n");
    fprintf(stderr, "    o: octal\n");
    fprintf(stderr, "    d: decimal\n");
    fprintf(stderr, "    h: hexadecimal\n");
    fprintf(stderr, "   0x: hexadecimal\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "  Examples\n");
    fprintf(stderr, "         Read binary input:\n");
    fprintf(stderr, "         %s b1001110\n", basename(prog));
    fprintf(stderr, "\n");
    fprintf(stderr, "         Read hexadecimal input:\n");
    fprintf(stderr, "         %s h09fa37\n", basename(prog));
    fprintf(stderr, "         %s 0x20\n", basename(prog));
    fprintf(stderr, "\n");
    fprintf(stderr, "         Without prefix, decimal format is assumed:\n");
    fprintf(stderr, "         %s 157\n", basename(prog));
    fprintf(stderr, "\n");
}

int
main(int argc, char * argv[])
{
    if (argc != 2)
    {
        printUsage(argv[0]);
        return 1;
    }

    uint64_t value = readInput(argv[1]);
    printOutput(value);

    return 0;
}

