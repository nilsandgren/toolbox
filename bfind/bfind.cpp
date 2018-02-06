//
// bfind
//
// A little command-line tool that scans an input file for ASCII, hex or binary
// patterns. View README.md for more details.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <libgen.h>

#include <iostream>

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) < (b) ? (b) : (a))

// color escape sequences
#define COLOR_FG_BLACK      "\033[30m"
#define COLOR_FG_RED        "\033[31m"
#define COLOR_FG_GREEN      "\033[32m"
#define COLOR_FG_YELLOW     "\033[33m"
#define COLOR_FG_BLUE       "\033[34m"
#define COLOR_FG_MAGENTA    "\033[35m"
#define COLOR_FG_CYAN       "\033[36m"
#define COLOR_FG_WHITE      "\033[37m"

#define COLOR_BG_BLACK      "\033[40m"
#define COLOR_BG_RED        "\033[41m"
#define COLOR_BG_GREEN      "\033[42m"
#define COLOR_BG_YELLOW     "\033[43m"
#define COLOR_BG_BLUE       "\033[44m"
#define COLOR_BG_MAGENTA    "\033[45m"
#define COLOR_BG_CYAN       "\033[46m"
#define COLOR_BG_WHITE      "\033[47m"

#define COLOR_RESET         "\033[0m"
#define CLEAR_SCREEN        "\033[2J"

enum status_code
{
    k_status_ok = 0,
    k_status_error
};

enum format
{
    k_ascii,   // ASCII text
    k_dec,     // decimal
    k_hex,     // hexadecimal
    k_bin,     // binary
};

class configuration
{
    public:
    configuration()
        : file(NULL)
        , pattern(NULL)
        , pattern_length(0)
        , pattern_format(k_ascii)
        , use_color(true)
        , case_sensitive(true)
        , file_offset_format(k_hex)
    {
    }

    FILE * file;               // The input file
    uint8_t * pattern;         // The pattern to search for
    uint64_t pattern_length;   // Search pattern length (bytes)
    format pattern_format;     // Search pattern format ASCII, Hex, or Binary
    bool use_color;            // Enables color printing
    bool case_sensitive;       // Enables case sensitive search
    format file_offset_format; // Format for printing file offsets
};

void print_hex(const configuration & config,
               uint64_t position,
               uint64_t start,
               uint64_t length,
               uint64_t match_length,
               const uint8_t * buffer)
{
    uint64_t i = 0;
    // bytes prior to match
    for (i = 0; i < position - start; i++)
    {
        printf("%02x ", buffer[i]);
    }

    // the match
    if(config.use_color)
    {
        printf(COLOR_FG_RED);
    }
    for (; i < match_length + (position - start); i++)
    {
        printf("%02x ", buffer[i]);
    }
    if(config.use_color)
    {
        printf(COLOR_RESET);
    }

    // bytes after match
    for (; i < length; i++)
    {
        printf("%02x ", buffer[i]);
    }
}

void print_ascii(const configuration & config,
                 uint64_t position,
                 uint64_t start,
                 uint64_t length,
                 uint64_t match_length,
                 int64_t trim,
                 const uint8_t * buffer)
{
    uint64_t i = 0;

    // bytes prior to match
    for (i = 0; i < position - start; i++)
    {
        if (isprint(buffer[i]))
        {
            printf("%c", buffer[i]);
        }
        else
        {
            printf(".");
        }
    }

    // the match
    if(config.use_color)
    {
        printf(COLOR_FG_RED);
    }
    for (; i < match_length + (position - start); i++)
    {
        if (isprint(buffer[i]))
        {
            printf("%c", buffer[i]);
        }
        else
        {
            printf(".");
        }
    }
    if(config.use_color)
    {
        printf(COLOR_RESET);
    }

    // bytes after match
    for (; i < length; i++)
    {
        if (isprint(buffer[i]))
        {
            printf("%c", buffer[i]);
        }
        else
        {
            printf(".");
        }
    }

    for (i = 0; i < (uint64_t)trim; i++)
    {
        printf(" ");
    }
}


void
print_match(const configuration & config,
            uint64_t position,
            uint64_t file_size,
            uint64_t match_length)
{
    int side_data = 6; // num bytes presented around match
    uint64_t start = max(((long long)position - side_data), 0);
    uint64_t stop = position + match_length + side_data;
    int64_t trim = stop - file_size; // How much output is trimmed
    trim = max(trim, 0);
    stop = min(((long long)(stop)), (long long)file_size);
    uint64_t length = stop - start;
    uint8_t * buffer = (uint8_t *) malloc (match_length + 2 * side_data);
    uint64_t position_backup = ftello(config.file);
    uint64_t i = 0;

    fseeko(config.file, start, SEEK_SET);
    size_t bytes_read = fread(buffer, 1, length, config.file);
    if (bytes_read != length)
    {
        std::cerr << "error: could not read " << length
                  << " bytes @ " << start << std::endl;
    }

    // hex in left column
    print_hex(config, position, start, length, match_length, buffer);

    for (i = 0; i < (uint64_t)trim; i++)
    {
        printf("   ");
    }

    printf(" | ");

    // ascii in right column
    print_ascii(config, position, start, length, match_length, trim, buffer);

    printf(" |");
    free(buffer);

    fseeko(config.file, position_backup, SEEK_SET);
}

bool
is_hex_character(uint8_t c)
{
    c = toupper(c);
    if (isalpha(c))
    {
        return c <= 'F' && c >= 'A';
    }
    else
    {
        return c <= '9' && c >= '0';
    }
}

uint8_t
nibble2byte(uint8_t c)
{
    uint8_t d = 0;
    c = toupper(c);

    if (isalpha(c))
    {
        d = c - 'A' + 10; // 'A' should equal 10
    }
    else
    {
        d = c - '0';      // the value of the digit
    }
    return d;
}

void
print_usage_short(char ** argv)
{
    std::string name = basename(argv[0]);
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  Search: " << name << " [OPTIONS] <string> <file> "
              << std::endl;
    std::cout << "  Help: : " << name << " --help" << std::endl;
    std::cout << std::endl;
}

void
print_usage(char ** argv)
{
    // indentation
    std::string i0 = "  ";
    std::string i1 = "     ";
    std::string i2 = "           ";
    std::string i3 = "             ";
    std::string name = basename(argv[0]);

    std::cout << std::endl;
    std::cout << i0 << "NAME" << std::endl;
    std::cout << i1 << name << " - search binary files quickly" << std::endl;
    std::cout << std::endl;
    std::cout << i0 << "SYNOPSIS" << std::endl;
    std::cout << i1 << name << " [OPTIONS] <string> <file> " << std::endl;
    std::cout << std::endl;
    std::cout << i0 << "DESCRIPTION" << std::endl;
    std::cout << i1 << "Search files for ASCII, hexadecimal, "
                    << "or binary search strings." << std::endl;
    std::cout << i1 << "The offset, and some neighboring data, of each match "
                    << "is printed to stdout.";
    std::cout << std::endl;
    std::cout << i1 << "All search patterns must start at a byte boundary."
                    << std::endl;
    std::cout << std::endl;
    std::cout << i1 << name << " returns 0 if at least one match is found, "
              << "and 1 otherwise." << std::endl;
    std::cout << std::endl;
    std::cout << i1 << "-h" << std::endl;
    std::cout << i1 << "--help" << std::endl;
    std::cout << i2 << "Display this help." << std::endl;
    std::cout << std::endl;
    std::cout << i1 << "-f <ascii|hex|bin>" << std::endl;
    std::cout << i1 << "--format <ascii|hex|bin>" << std::endl;
    std::cout << i2 << "Specify the format of the search string." << std::endl;
    std::cout << std::endl;
    std::cout << i3 << "ascii:  ASCII text search string, e.g. hello"
              << std::endl;
    std::cout << i3 << "  hex:  Hexadecimal search string, e.g. 4d5601c0"
              << std::endl;
    std::cout << i3 << "  bin:  Binary search string, e.g. 0110111011110100"
              << std::endl;
    std::cout << std::endl;
    std::cout << i2 << "The default format is ascii.";
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << i1 << "-c <yes|no>" << std::endl;
    std::cout << i1 << "--color <yes|no>" << std::endl;
    std::cout << i2 << "Print matching file content using ANSI color "
                    << "escape codes." << std::endl;
    std::cout << i2 << "This is the default." << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << i1 << "-o <dec|hex>" << std::endl;
    std::cout << i1 << "--offset <dec|hex>" << std::endl;
    std::cout << i2 << "Controls the format for printing file offsets.";
    std::cout << std::endl;
    std::cout << i3 << "  dec:  Prints file offsets in decimal format"
              << std::endl;
    std::cout << i3 << "  hex:  Prints file offsets in hexadecimal format"
              << std::endl;
    std::cout << i2 << "The default is hexadecimal." << std::endl;
    std::cout << std::endl;
    std::cout << i1 << "-i" << std::endl;
    std::cout << i1 << "--ignore-case" << std::endl;
    std::cout << i2 << "Enable case-insensitive search." << std::endl;
    std::cout << i2 << "Only applicable to ASCII search strings." << std::endl;
    std::cout << i2 << "Case-sensitive search is the default." << std::endl;
    std::cout << std::endl;
    std::cout << i0 << "EXAMPLES" << std::endl;
    std::cout << i1 << "Find the ASCII string banana in file.bin.";
    std::cout << std::endl;
    std::cout << i2 << name << " banana file.bin" << std::endl;
    std::cout << std::endl;
    std::cout << i1 << "Find the hexadecimal string 0a134b "
                    << "in file.bin." << std::endl;
    std::cout << i2 << name << " -f hex 0a134b file.bin" << std::endl;
    std::cout << std::endl;
    std::cout << i1 << "Search for the binary string 0110100110011001 "
                    << "in file.bin." << std::endl;
    std::cout << i2 << name << " -f bin 0110100110011001 file.bin";
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << i0 << "AUTHOR" << std::endl;
    std::cout << i1 << "Written by Nils Andgren, 2014." << std::endl;
    std::cout << std::endl;
}

status_code
apply_command_line_options(configuration * dst_conf, int argc, char * argv [])
{
    int index = 1;
    char * file_path = NULL;
    char * pattern = NULL;

    while (index < argc)
    {
        char * option = argv[index];
        if ((0 == strcmp(option, "-f") ||
             0 == strcmp(option, "--format")))
        {
            if (index + 1 >= argc)
            {
                std::cerr << "error: No argument given for "
                          << option << std::endl;
                continue;
            }

            index++;
            option = argv[index];
            if (0 == strcmp(option, "ascii"))
            {
                dst_conf->pattern_format = k_ascii;
            }
            else if (0 == strncmp(option, "hex", 3))
            {
                dst_conf->pattern_format = k_hex;
            }
            else if (0 == strncmp(option, "bin", 3))
            {
                dst_conf->pattern_format = k_bin;
            }
            else
            {
                std::cerr << "error: " << option
                          << " is not a valid pattern_format" << std::endl;
                return k_status_error;
            }
        }
        else if (0 == strcmp(option, "-h") ||
                 0 == strcmp(option, "--help"))
        {
            print_usage(argv);
            exit(0);
        }
        else if (0 == strcmp(option, "-c") ||
                 0 == strcmp(option, "--color"))
        {
            dst_conf->use_color = true;
            if (index + 1 < argc)
            {
                char * color = argv[index + 1];
                if (0 == strcmp(color, "no") ||
                    0 == strcmp(color, "off"))
                {
                    dst_conf->use_color = false;
                    index++;
                }
                else if (0 == strcmp(color, "yes") ||
                         0 == strcmp(color, "on"))
                {
                    dst_conf->use_color = true;
                    index++;
                }
            }
        }
        else if (0 == strcmp(option, "-o") ||
                 0 == strcmp(option, "--offset"))
        {
            dst_conf->file_offset_format = k_hex;
            if (index + 1 < argc)
            {
                char * offset_format = argv[index + 1];
                if (0 == strncmp(offset_format, "dec", 3))
                {
                    dst_conf->file_offset_format = k_dec;
                    index++;
                }
                else if (0 == strncmp(offset_format, "hex", 3))
                {
                    dst_conf->file_offset_format = k_dec;
                    index++;
                }
            }
        }
        else if (0 == strcmp(option, "-i") ||
                 0 == strcmp(option, "--ignore-case"))
        {
            dst_conf->case_sensitive = false;
        }
        else if (option[0] == '-')
        {
            std::cerr << "error: Unknown option - " << option << std::endl;
            return k_status_error;
        }
        else if (pattern == NULL)
        {
            // We assume the following command line format:
            //   bfind [options] pattern [file]
            pattern = option;
        }
        else if (file_path == NULL)
        {
            // We assume the following command line format:
            //   bfind [options] pattern file
            file_path = option;
        }
        else
        {
            std::cerr << "error: bad command line format" << std::endl;
            print_usage(argv);
            return k_status_error;
        }
        index++;
    }

    if (pattern == NULL)
    {
        std::cerr << "error: No search string specified" << std::endl;
        return k_status_error;
    }
    else
    {
        if (k_ascii == dst_conf->pattern_format)
        {
            uint64_t length = strlen(pattern);
            dst_conf->pattern = (uint8_t*) calloc(length+1, 1);
            memcpy(dst_conf->pattern, pattern, length);
            dst_conf->pattern_length = length;

            if (dst_conf->case_sensitive == false)
            {
                // We use lower case for the search string and for the file
                // buffer in the search() function.
                uint8_t * buffer = dst_conf->pattern;
                for (uint64_t i = 0; i < length; i++)
                {
                    buffer[i] = tolower(buffer[i]);
                }
            }

        }
        else if (k_hex == dst_conf->pattern_format)
        {
            uint64_t length = strlen(pattern);
            if (length & 1)
            {
                std::cerr << "error: Hexadecimal search string length should "
                          << "be a multiple of two." << std::endl;
                return k_status_error;
            }

            dst_conf->pattern = (uint8_t*) malloc (length/2);
            dst_conf->pattern_length = length/2;

            uint8_t msb = 0;
            uint8_t lsb = 0;
            uint64_t o = 0;
            for (uint64_t i = 0; i < length; i+=2, o++)
            {
                msb = pattern[i+0];
                lsb = pattern[i+1];

                if (!is_hex_character(msb) || !is_hex_character(lsb))
                {
                    std::cerr << "error: Non-hexadecimal character in search "
                              << "pattern" << std::endl;
                    return k_status_error;
                }

                dst_conf->pattern[o] = (nibble2byte(msb) << 4) |
                                        nibble2byte(lsb);
            }
        }
        else if (k_bin == dst_conf->pattern_format)
        {
            uint32_t length = strlen(pattern);
            if (length % 8)
            {
                std::cerr << "error: Binary search string length should "
                          << "be a multiple of eight." << std::endl;
                return k_status_error;
            }

            dst_conf->pattern = (uint8_t*) calloc(length/8, 1);
            dst_conf->pattern_length = length/8;

            uint64_t o = 0;
            for (uint64_t i = 0; i < length; i+=8, o++)
            {
                uint8_t dst_value = 0;
                for (uint64_t j = 0; j < 8; j++)
                {
                    dst_value <<= 1;
                    uint8_t bit_char = pattern[i + j];
                    if ('1' == bit_char)
                    {
                        dst_value |= 1;
                    }
                    else if ('0' != bit_char)
                    {
                        std::cerr << "error: Non-binary character in search "
                                  << "pattern" << std::endl;
                        return k_status_error;
                    }
                }
                dst_conf->pattern[o] = dst_value;
            }
        }
    }

    if (dst_conf->pattern_format != k_ascii &&
        dst_conf->case_sensitive == false)
    {
        std::cerr << "error: Case can only be ignored when searching "
                  << "for ASCII strings." << std::endl;
        return k_status_error;
    }

    if (file_path == NULL)
    {
        std::cerr << "No input file specified." << std::endl;
        return k_status_error;
    }
    else
    {
        dst_conf->file = fopen(file_path, "rb");
        if (dst_conf->file == NULL)
        {
            std::cerr << "error: Failed to open " << file_path << std::endl;
            return k_status_error;
        }
    }

    return k_status_ok;
}

status_code search(const configuration & config)
{
    status_code status = k_status_ok;
    const uint64_t k_buffer_size = 4096;
    uint8_t * pattern = config.pattern;
    uint64_t pattern_length = config.pattern_length;
    uint64_t buffer_bytes = k_buffer_size + pattern_length;
    uint8_t * buffer = (uint8_t*) malloc(buffer_bytes);
    uint64_t buffer_pos = 0;    // current read position
    uint64_t buffer_offset = 0; // where to write data in buffer

    uint8_t * match_buffer = (uint8_t*) malloc(buffer_bytes);

    bool match = true;   // match at this position
    int match_count = 0; // match count so far
    bool end_of_data = false;
    uint64_t bytes_read = 0;

    uint64_t file_pos = 0;
    uint64_t file_size = 0;

    FILE * search_file = config.file;

    fseeko(search_file, 0, SEEK_END);
    file_size = ftello(search_file);
    rewind(search_file);

    bytes_read = fread(buffer + buffer_offset,
                       1, k_buffer_size + pattern_length,
                       search_file);

    if (bytes_read < k_buffer_size)
    {
        end_of_data = true;
    }
    else
    {
        bytes_read = k_buffer_size;
    }

    do
    {
        if (bytes_read < k_buffer_size)
        {
            end_of_data = true;
        }

        if (config.case_sensitive == false)
        {
            for (uint64_t i = 0; i < bytes_read; i++)
            {
                buffer[i] = tolower(buffer[i]);
            }
        }

        // search through the buffer up to last possible complete match
        for (buffer_pos = 0; buffer_pos <= bytes_read; buffer_pos++)
        {
            uint8_t * buffer_ptr = buffer + buffer_pos;
            match = true;
            uint64_t pattern_pos = 0;
            for (; pattern_pos < pattern_length; pattern_pos++)
            {
                if (buffer_ptr[pattern_pos] != pattern[pattern_pos])
                {
                    match = false;
                    break;
                }
            }

            if (match)
            {
                uint64_t match_pos = file_pos + (uint64_t) buffer_pos;
                if (config.file_offset_format == k_hex)
                {
                    printf("match @ 0x%08lX  ", match_pos);
                }
                else
                {
                    printf("match @ %08ld  ", match_pos);
                }
                print_match(config, match_pos, file_size, pattern_length);
                printf("\n");
                match_count++;
            }
        }

        // copy tail to beginning and set offset for next read
        if (bytes_read > pattern_length)
        {
            memcpy(buffer, buffer + buffer_pos, pattern_length);
            buffer_offset = pattern_length;
        }

        file_pos += bytes_read;
        bytes_read = fread(buffer + buffer_offset, 
                           1, k_buffer_size, 
                           search_file);
    } 
    while (!end_of_data);

    free (match_buffer);
    free (buffer);
    
    if (!match_count)
    {
        std::cout << "No match found." << std::endl;
        status = k_status_error;
    }
    else
    {
        std::string es = match_count > 1 ? "es" : "";
        std::cout << match_count << " match" << es << " found." << std::endl;
        status = k_status_ok;
    }
    return status;
}


int
main (int argc, char * argv [])
{
    status_code status = k_status_ok;

    configuration config;
    status = apply_command_line_options(&config, argc, argv);
    if (k_status_ok != status)
    {
        std::cerr << "error: command line error" << std::endl;
        print_usage_short(argv);
        status = k_status_error;
        goto exit;
    }

    status = search(config);

    free(config.pattern);
    fclose(config.file);

    exit:

    return status;
}
