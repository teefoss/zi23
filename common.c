//
//  common.c
//  zi23
//
//  Created by Thomas Foster on 1/28/23.
//
#include "common.h"

#include <stdarg.h>

void FatalError(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    fprintf(stderr, "error: ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");

    va_end(args);
    exit(EXIT_FAILURE);
}

void PrintUsage(const char * program_name)
{
    fprintf(stderr, "usage:\n");
    fprintf(stderr, "  %s -c [source code file]\n", program_name);
    fprintf(stderr, "  %s -r [program name]\n", program_name);
}

void * Allocate(size_t num_bytes)
{
    void * memory = malloc(num_bytes);
    if ( memory == NULL ) {
        FatalError("out of memory");
    }

    return memory;
}

char * BitString(uint8_t n)
{
    static char bits[9] = { 0 };

    for (int i = 7; i >= 0; i--) {
        bits[7 - i] = (n >> i & 1) + '0';
    }

    return bits;
}

long FileSize(FILE * file)
{
    long current = ftell(file);
    fseek(file, 0, SEEK_END);
    long end = ftell(file);
    fseek(file, current, SEEK_SET);

    return end;
}

s64 timespec_diff_ns(struct timespec start, struct timespec end)
{
    long diff_ns;

    if ( end.tv_nsec - start.tv_nsec < 0 ) {
        diff_ns = 1000000000L + end.tv_nsec - start.tv_nsec;
    } else {
        diff_ns = end.tv_nsec - start.tv_nsec;
    }

    diff_ns += (end.tv_sec - start.tv_sec) * 1000000000L;
    return diff_ns;
}
