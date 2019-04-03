#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "roman.h"

uint64_t readRoman(const char * input)
{
    int64_t result = 0;
    int64_t current = 0;
    int64_t previous = 0;
    // Traverse from end
    for (int i = strlen(input) - 1; i >= 0; i--)
    {
        switch (toupper(input[i]))
        {
            case 'I': current = 1; break;
            case 'V': current = 5; break;
            case 'X': current = 10; break;
            case 'L': current = 50; break;
            case 'C': current = 100; break;
            case 'D': current = 500; break;
            case 'M': current = 1000; break;
            default:
            {
                fprintf(stderr, "\n");
                fprintf(stderr, "  Not a Roman numeral: %c\n", input[i]);
                return 0;
            }
        }
        result += current < previous ? -current : current;
        previous = current;
    }
    return (uint64_t)result;
}


// Print value as a string of Roman numerals
void printRoman(uint64_t value)
{
    struct RomanData
    {
        int64_t value;
        char const * numeral;
        size_t len;
    };

    static RomanData const romanData[] =
    { 
        1000, "M", 1,
        900, "CM", 2,
        500, "D", 1,
        400, "CD", 2,
        100, "C", 1,
        90, "XC", 2,
        50, "L", 1,
        40, "XL", 2,
        10, "X", 1,
        9, "IX", 2,
        5, "V", 1,
        4, "IV", 2,
        1, "I", 1,
        0, NULL, 0
    };

    const size_t maxLength = 1024;
    char output[maxLength];
    memset(output, 0, maxLength);
    size_t pos = 0;
    for (RomanData const* current = romanData; current->value > 0; ++current)
    {
        if (pos + 3 > maxLength)
        {
            return;
        }
        while (value >= current->value)
        {
            sprintf(output + pos, "%s", current->numeral);
            pos += current->len;
            value -= current->value;
        }
    }
    printf("  rom: %s\n", output);
}

