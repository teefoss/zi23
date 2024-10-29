//
//  common.h
//  zi23
//
//  Created by Thomas Foster on 1/28/23.
//

#ifndef common_h
#define common_h

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#define NSEC_PER_SEC 1000000000L
#define CASE_RET_STR(enum_member) case enum_member: return #enum_member
#define IDENT_MAX 256
#define MAX(a, b) ((a > b) ? (a) : (b))
#define MIN(a, b) ((a < b) ? (a) : (b))

typedef int8_t      s8;
typedef int16_t     s16;
typedef int32_t     s32;
typedef int64_t     s64;

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef _Float16    f16;

void *  Allocate(size_t num_bytes);
char *  BitString(uint8_t n);
void    FatalError(const char *format, ...);
long    FileSize(FILE * file);
void    PrintUsage(const char * program_name);
s64     timespec_diff_ns(struct timespec start, struct timespec end);

#endif /* common_h */
