//
//  symbol.c
//  zi23
//
//  Created by Thomas Foster on 1/28/23.
//
#include "symbol.h"
#include "common.h"

#include <stdio.h>
#include <string.h>

#define TABLE_SIZE 193 // 3079

static Symbol * table[TABLE_SIZE];

static unsigned StringHash(const char * key)
{
    /*
     Good hash table prime numbers from
     https://planetmath.org/goodhashtableprimes

     Use table size = num table entries * 1.3

     193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317,
     196613, 393241, 786433, 1572869, 3145739, 6291469,
     12582917, 25165843, 50331653, 100663319, 201326611,
     402653189, 805306457, 1610612741,
     */

    // djb2 string hash function:
    unsigned result = 5381;
    const char * p = key;

    while ( *p != '\0' ) {
        result = (result << 5) + result + *p;
        ++p;
    }

    return result;
}

static unsigned TableIndex(const char * key)
{
    return StringHash(key) % TABLE_SIZE;
}

Symbol * LookupSymbol(const char * name)
{
    int i = TableIndex(name);

    for ( Symbol * s = table[i]; s; s = s->next ) {
        if ( strcmp(s->name, name) == 0 ) {
            return s;
        }
    }

    return NULL;
}

Symbol * InstallSymbol(const char * name, int type, int location)
{
    Symbol * s = Allocate(sizeof(*s));

    size_t name_len = strlen(name);
    s->name = Allocate(name_len + 1);
    strcpy(s->name, name);
    s->type = type;
    
    if ( location == UNRESOLVED_LOCATION ) {
        s->resolved = false;
    } else {
        s->resolved = true;
        s->address = location;
    }

    // Insert into list.
    int i = TableIndex(name);
    s->next = table[i];
    table[i] = s;

    return s;
}

void FreeSymbolTable(void)
{
    for ( int i = 0; i < TABLE_SIZE; i++ ) {
        Symbol * s = table[i];
        while ( s ) {
            Symbol * temp = s;
            s = s->next;
            free(temp->name);
            free(temp);
        }
    }
}

void PrintSymbolTable(void)
{
    puts("\nSymbol Table:");
    for ( int i = 0; i < TABLE_SIZE; i++ ) {
        if ( table[i] ) {
            printf("table %3d: ", i);
            for ( Symbol * s = table[i]; s; s = s->next ) {
                printf("%s (loc %04X)", s->name, s->address);
                if ( s->next ) {
                    printf(", ");
                }
            }
            printf("\n");
        }
    }
}
