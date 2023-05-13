//
//  symbol.h
//  zi23
//
//  Created by Thomas Foster on 1/28/23.
//
//  Symbol table.
//
#ifndef symbol_h
#define symbol_h

#include "common.h"
#include <stdbool.h>

#define UNRESOLVED_LOCATION 0x10000

typedef struct symbol {
    char *      name;
    enum        { UNDEFINED, LABEL, DATA8, DATA16 } type;
    u16         address;
    bool        resolved;

    struct symbol * next;
} Symbol;

void        FreeSymbolTable(void);
Symbol *    LookupSymbol(const char * name);
Symbol *    InstallSymbol(const char * name, int type, int offset);

void        PrintSymbolTable(void);

#endif /* symbol_h */
