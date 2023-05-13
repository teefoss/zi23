//
//  executable.c
//  zi23
//
//  Created by Thomas Foster on 2/23/23.
//

#include "executable.h"
#include <string.h>

typedef struct section_node {
    Section section;
    struct section_node * next;
} SectionNode;

// Storage used during parsing, later written to executable.
static SectionNode * section_list;

void PushSection(u16 file_location, u16 mem_location)
{
    SectionNode * node = malloc(sizeof(*node));
    node->section.file_location = file_location;
    node->section.mem_location = mem_location;

    // Updated upon reaching start of next section or end.
    node->section.size = 0;

    node->next = section_list;
    section_list = node;
}

Section * PeekSectionList(void)
{
    if ( section_list ) {
        return &section_list->section;
    }
    return NULL;
}

bool PopSection(Section * out)
{
    Section * sec = PeekSectionList();
    if ( sec == NULL ) {
        return false;
    }

    *out = *sec;

    SectionNode * temp = section_list;
    section_list = section_list->next;
    free(temp);

    return true;
}

u8 * BufferFile(const char * path)
{
    FILE * stream = fopen(path, "rb");
    if ( stream == NULL ) {
        FatalError("BufferFile: could not open '%s'", path);
    }

    fseek(stream, 0, SEEK_END);
    u16 size = (u16)ftell(stream);
    rewind(stream);

    u8 * buffer = malloc(size);
    fread(buffer, 1, size, stream);
    fclose(stream);

    return buffer;
}

u16 LoadExecutableFromBuffer(const u8 * buffer, u8 * vm_memory)
{
    // Get the header.
    Header header = *(Header *)&buffer[0];

    // Find the location of the section table.
    Section * sections = (Section *)&buffer[header.section_list_offset];

    // Copy each section to appropriate location in vm memory.
    for ( int i = 0; i < header.num_sections; i++ ) {
        memcpy(vm_memory + sections[i].mem_location,
               buffer + sections[i].file_location,
               sections[i].size);
    }

    return header.entry_point;
}

u16 LoadExecutableFromPath(const char * path, u8 * vm_memory)
{
    u8 * buffer = BufferFile(path);
    u16 entry_point = LoadExecutableFromBuffer(buffer, vm_memory);
    free(buffer);

    return entry_point;
}
