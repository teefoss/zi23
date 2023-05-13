//
//  executable.h
//  zi23
//
//  Created by Thomas Foster on 2/23/23.
//

#ifndef executable_h
#define executable_h

#include "common.h"
#include <stdbool.h>

typedef struct {
    u16 entry_point;
    u16 num_sections; // Number of sections in section list
    u16 section_list_offset; // Resides at end of executable
} Header;

// TODO: __attribute__((packed)) ?
typedef struct {
    u16 file_location; // Start of this section in the executable
    u16 mem_location; // The absolute memory address of this section
    u16 size; // Size of section in bytes.
} Section;

void        PushSection(u16 file_location, u16 mem_location);
Section *   PeekSectionList(void);
bool        PopSection(Section * out);

/// Load program from path into VM memory.
/// - Returns: The VM memory execution entry point.
u16 LoadExecutableFromPath(const char * path, u8 * vm_memory);

/// Load program from buffer into VM memory.
/// - Returns: The VM memory execution entry point.
u16 LoadExecutableFromBuffer(const u8 * buffer, u8 * vm_memory);

#endif /* executable_h */
