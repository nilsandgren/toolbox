# bfind

This is the output from bfind --help

    NAME
       bfind - search binary files quickly

    SYNOPSIS
       bfind [OPTIONS] <string> <file> 

    DESCRIPTION
       Search files for ASCII, hexadecimal, or binary search strings.
       The offset, and some neighboring data, of each match is printed to stdout.
       All search patterns must start at a byte boundary.

       bfind returns 0 if at least one match is found, and 1 otherwise.

       -h, --help
             Display this help.

       -f ascii|hex|bin
       --format ascii|hex|bin
             Specify the format of the search string.

               ascii:  ASCII text search string, e.g. hello
                 hex:  Hexadecimal search string, e.g. 4d5601c0
                 bin:  Binary search string, e.g. 0110111011110100

             The default format is ascii.

       -c yes|no
       --color yes|no
             Print matching file content using ANSI color escape codes.

       -i
       --ignore-case
             Enable case-insensitive search.
             Only applicable to ASCII search strings.

    EXAMPLES
       Find the ASCII string banana in file.bin.
             bfind banana file.bin

       Find the hexadecimal string 0a134b in file.bin.
             bfind -f hex 0a134b file.bin

       Search for the binary string 0110100110011001 in file.bin.
             bfind -f bin 0110100110011001 file.bin

    AUTHOR
       Written by Nils Andgren, 2014.

