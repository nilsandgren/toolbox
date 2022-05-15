#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <chrono>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <libgen.h>
#include <unistd.h>

/* Log line format (field 0..n) */
#define DATE_INDEX 3
#define STATUS_INDEX 8
#define BYTES_INDEX 9

/* Supported date formats */
const char* DATE_FORMATS[3] =
{
    "[%d/%b/%Y:%T",  /* E.g. [06/May/2022:14:12:03 */
    "[%Y-%m-%dT%T",  /* E.g. [2021-09-17T10:01:01 */
    NULL
};

enum status
{
    success,
    failure
};

enum mode
{
    follow,
    replay,
    scan
};

struct counters
{
    uint64_t codes[6];  /* 0xx, 1xx, 2xx, ... status codes */
    uint64_t requests;
    uint64_t bytes;
    double start_time;
    double end_time;
};

void reset_counters(counters* stats);
void print_counters(counters* stats, bool header);

double current_time();
const char* determine_date_format(const char* date_string);
double parse_date(const char* date_string, const char* format);
double get_time(mode mode, const char* date_string, const char* format);
void list_date_formats();
void human_print(uint64_t number, char* dst);

status process_input(FILE* input, mode mode, double interval);
size_t split_line(char** fields, size_t num_fields, char* line);
status add_counters(char** fields, size_t num_fields, counters* stats);


void print_usage(char* arg0)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "NAME\n");
    fprintf(stderr, "    %s - HTTP stats from access log\n", basename(arg0));
    fprintf(stderr, "\n");
    fprintf(stderr, "OPTIONS\n");
    fprintf(stderr, "    -h\n");
    fprintf(stderr, "        Print help and exit.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "    -f\n");
    fprintf(stderr, "        Follow mode (default)\n");
    fprintf(stderr, "          Tail stdin and use wall-clock as time.\n");
    fprintf(stderr, "          tail -F access.log | %s\n", basename(arg0));
    fprintf(stderr, "\n");
    fprintf(stderr, "    -r <logfile>\n");
    fprintf(stderr, "        Replay mode\n");
    fprintf(stderr, "          Read dates from log. Process one second of log per second.\n");
    fprintf(stderr, "          %s -r access.log\n", basename(arg0));
    fprintf(stderr, "\n");
    fprintf(stderr, "    -s <logfile>\n");
    fprintf(stderr, "        Scan mode\n");
    fprintf(stderr, "          Read dates from log. Process as fast as possible.\n");
    fprintf(stderr, "          %s -s access.log\n", basename(arg0));
    fprintf(stderr, "\n");
    fprintf(stderr, "Log line format (defined in %s):\n", __FILE__);
    fprintf(stderr, "    Date.........: field %d\n", DATE_INDEX + 1);
    fprintf(stderr, "    Status.......: field %d\n", STATUS_INDEX + 1);
    fprintf(stderr, "    Byte count...: field %d\n", BYTES_INDEX + 1);
    fprintf(stderr, "    Date formats.: ");
    list_date_formats();
    fprintf(stderr, "\n");
    fprintf(stderr, "\n");
}

FILE*
open_file(const char* path)
{
    FILE* f = fopen(path, "r");
    if (f != NULL)
        return f;
    fprintf(stderr, "error: Could not open %s\n", path);
    exit(failure);
}

int
main(int argc, char* argv[])
{
    mode mode = follow;
    FILE* input = stdin;

    if (argc == 3 && strcmp(argv[1], "-r") == 0)
    {
        input = open_file(argv[2]);
        mode = replay;
        fprintf(stderr,"info: replaying %s in real time, reading time from log.\n", argv[2]);
    }
    else if (argc == 3 && strcmp(argv[1], "-s") == 0)
    {
        input = open_file(argv[2]);
        mode = scan;
        fprintf(stderr, "info: scanning %s, reading time from log.\n", argv[2]);
    }
    else if (argc == 2 && strcmp(argv[1], "-h") == 0)
    {
        print_usage(argv[0]);
        return 0;
    }
    else
    {
        fprintf(stderr, "info: tailing stdin in real time\n");
    }

    double interval = 1.0;
    fprintf(stderr, "info: using interval of %.2f sec\n", interval);

    status status = process_input(input, mode, interval);
    fclose(input);
    return status;
}

void reset_counters(counters* stats)
{
    memset(stats, 0, sizeof(counters));
}

void print_counters(counters* stats, bool header)
{
    if (header)
    {
        printf(".-------.-------.-------.-------"
               ".-------.-------.-------.-------------.\n");
        printf("|   0xx |   1xx |   2xx |   3xx "
               "|   4xx |   5xx |   all |        rate |\n");
        printf("'-------'-------'-------'-------"
               "'-------'-------'-------'-------------'\n");
    }

    double duration = stats->end_time - stats->start_time;
    uint64_t bitrate = uint64_t(stats->bytes * 8 / duration);
    char bitrate_buf[32];
    human_print(bitrate, bitrate_buf);
    printf("| %5lu | %5lu | %5lu | %5lu "
           "| %5lu | %5lu | %5lu | %8sbps |\n",
           stats->codes[0], stats->codes[1], stats->codes[2],
           stats->codes[3], stats->codes[4], stats->codes[5],
           stats->requests, bitrate_buf);
}

void human_print(uint64_t number, char* dst)
{
    if (number < 1000)
        sprintf(dst, "%lu ", number);
    else if (number / 1000 < 1000)
        sprintf(dst, "%lu K", number / 1000);
    else if (number / uint64_t(pow(10, 6)) < 1000)
        sprintf(dst, "%.2f M", number / pow(10, 6));
    else if (number / uint64_t(pow(10, 9)) < 1000)
        sprintf(dst, "%.2f G", number / pow(10, 9));
    else
        sprintf(dst, "%.2f T", number / pow(10, 12));
}

double current_time()
{
    using namespace std::chrono;
    using time_type = duration<double>;

    auto time = system_clock::now();
    return duration_cast<time_type>(time.time_since_epoch()).count();
}

double parse_date(const char* date_string, const char* format)
{
    struct tm t;
    memset(&t, 0, sizeof(t));
    strptime(date_string, format, &t);
    return double(mktime(&t));
}

double get_time(mode mode, const char* date_string, const char* format)
{
    if (mode != follow)
        return parse_date(date_string, format);
    else
        return current_time();
}

void list_date_formats()
{
    for (size_t i = 0; ; i++)
    {
        if (!DATE_FORMATS[i])
            break;
        fprintf(stderr, "\"%s\"", DATE_FORMATS[i]);
        if (DATE_FORMATS[i + 1])
            fprintf(stderr, " or ");
    }
}

const char* determine_date_format(const char* date_string)
{
    for (size_t i = 0; ; i++)
    {
        if (!DATE_FORMATS[i])
            return NULL;
        if (parse_date(date_string, DATE_FORMATS[i]) > 1000000000.0)
            return DATE_FORMATS[i];
    }
}

status process_input(FILE* input, mode mode, double interval)
{
    status status = success;

    char* line_buf = NULL;
    size_t buf_len = 0;
    const size_t max_fields = 50;
    char* fields[max_fields];
    const char* date_format = NULL;

    counters stats;
    reset_counters(&stats);
    uint64_t output_count = 0;
    while (getline(&line_buf, &buf_len, input) != -1)
    {
        size_t num_fields = split_line(fields, max_fields, line_buf);
        status = add_counters(fields, num_fields, &stats);
        if (status != success)
            goto error;

        const char* date = fields[DATE_INDEX];
        if (stats.start_time == 0.0)
        {
            date_format = determine_date_format(date);
            if (date_format == NULL)
            {
                fprintf(stderr, "error: Unsupported date format\n");
                fprintf(stderr, "error: Could not read %s of field %u as a date\n",
                        date, DATE_INDEX + 1);
                exit(failure);
            }
            stats.start_time = get_time(mode, date, date_format);
        }

        stats.end_time = get_time(mode, date, date_format);
        if (stats.end_time - stats.start_time >= interval)
        {
            print_counters(&stats, output_count % 10 == 0);
            reset_counters(&stats);
            output_count++;
            stats.start_time = stats.end_time;
            if (mode == replay)
                usleep(1000000 * interval);
        }
    }

error:
    if (line_buf)
        free(line_buf);
    return status;
}

/* Split line at spaces. strtok is used so input is destroyed. */
size_t split_line(char** fields, size_t num_fields, char* line)
{
    size_t field = 0;
    char* token = strtok(line, " ");
    if (token == NULL)
        return 0;
    fields[field] = token;

    for (field = 1; field < num_fields; field++)
    {
        token = strtok(NULL, " ");
        if (token == NULL)
            break;
        fields[field] = token;
    }
    return field;
}

/* Read status code and byte count from fields and update counters. */
status add_counters(char** fields, size_t num_fields, counters* stats)
{
    /* Assuming status code at index 8 and byte count at index 9 */
    size_t min_fields = std::max(DATE_INDEX, std::max(STATUS_INDEX, BYTES_INDEX));
    if (num_fields < min_fields)
    {
        fprintf(stderr, "error: line has too few fields\n");
        return failure;
    }

    /* Determine the type of status code */
    char first_char = fields[STATUS_INDEX][0];
    if (first_char >= '0' && first_char <= '5')
    {
        stats->codes[first_char - '0']++;
        stats->requests++;
    }
    else
    {
        fprintf(stderr, "error: could not parse response code\n");
    }

    /* Read byte count */
    char* endptr = fields[BYTES_INDEX];
    uint64_t bytes = strtoul(fields[BYTES_INDEX], &endptr, 10);
    if (*endptr == '\0')
        stats->bytes += bytes;
    else
        fprintf(stderr, "error: could not parse byte count\n");

    return success;
}
