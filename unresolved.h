//
//  unresolved.h
//  zi23
//
//  Created by Thomas Foster on 2/13/23.
//

#ifndef unresolved_h
#define unresolved_h

#include "common.h"

/*
 *  An `UnresolvedLocation` is for the situation where
 *  a symbol was encountered that was:
 *    1. not present in the symbol table, or
 *    2. present, but is itself unresolved (was added, but with no location).
 */
typedef struct unresolved_location {
    char symbol_name[IDENT_MAX];

    // The offset in program at which the a symbol's
    // offset was needed, but was not known, and will
    // be revisited/written to at the end.
    u16 file_offset;

    // The actual address of this symbol as determined by the program counter.
    u16 address;

    // The line number of the unresolved symbol.
    int line_num;

    // Next in linked list.
    struct unresolved_location * next;
} UnresolvedLocation;

void PushUnresolvedLocation(uint16_t offset, int line_num, const char * symbol_name);
void ResolveLocations(FILE * output);

#endif /* unresolved_h */
