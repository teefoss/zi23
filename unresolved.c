//
//  unresolved.c
//  zi23
//
//  Created by Thomas Foster on 2/13/23.
//

#include "unresolved.h"

#include "common.h"
#include "symbol.h"
#include "vm.h"

#include <string.h>

static UnresolvedLocation * unresolved_locations; // Linked list

// offset: with program (as obtained by ftell(output))
void PushUnresolvedLocation(uint16_t file_offset, int line_num, const char * symbol_name)
{
    UnresolvedLocation * ul = malloc(sizeof(*ul));
    ul->file_offset = file_offset;
    ul->address = 0; // unknown, determined later with ResolveLocations
    ul->line_num = line_num;
    strcpy(ul->symbol_name, symbol_name);

    ul->next = unresolved_locations;
    unresolved_locations = ul;
}

void ResolveLocations(FILE * output)
{
    UnresolvedLocation * ul = unresolved_locations;

    while ( ul != NULL ) {
        Symbol * sym = LookupSymbol(ul->symbol_name);
        if ( sym == NULL || !sym->resolved ) {
            FatalError("line %d: undefined symbol (%s)",
                       ul->line_num,
                       ul->symbol_name);
            return;
        }

        printf("resolving symbol %s at offset %d\n", sym->name, ul->file_offset);
        fseek(output, ul->file_offset, SEEK_SET);
        fwrite(&sym->address, sizeof(sym->address), 1, output);

        UnresolvedLocation * temp = ul;
        ul = ul->next;
        free(temp);
    }
}
