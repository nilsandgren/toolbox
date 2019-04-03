#include <stdint.h>

// Read a string of Roman numerals and return its value
// Symbol  I    V    X    L    C    D    M
// Value   1    5   10   50  100  500 1000
uint64_t readRoman(const char * input);

// Print value as a string of Roman numerals
void printRoman(uint64_t value);
