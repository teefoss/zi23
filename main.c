//
//  zi23.c
//  zi23
//
//  Created by Thomas Foster on 1/28/23.
//
#include "common.h"
#include "assembler.h"
#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static int     _argc;
static char ** _argv;

// Return the index of `arg` in argv or -1 if not present.
int GetArg(const char * arg) {
    for ( int i = 0; i < _argc; i++ ) {
        if ( strcmp(_argv[i], arg) == 0 ) {
            return i;
        }
    }

    return -1;
}

// Get the arg following the arg `option` or NULL if not present.
char * GetOptionArg(const char * option)
{
    int i = GetArg(option);

    if ( i != -1 && i + 1 < _argc ) {
        return _argv[i + 1];
    }

    return NULL;
}

int main(int argc, char ** argv)
{
    _argc = argc;
    _argv = argv;

    char * file = GetOptionArg("-s");
    if ( file ) {
        Assemble(file);
    }

    StartupOptions options = { .border_size = 16 };

    options.fullscreen = GetArg("-fullscreen") != -1;
    options.no_startup = GetArg("-no-startup") != -1;
    options.program_file = GetOptionArg("-r");
    options.border_size = 16; // default

    char * border_size_arg = GetOptionArg("-border");
    if ( border_size_arg ) {
        errno = 0;
        long size = strtol(border_size_arg, NULL, 10);
        if ( errno == 0 ) {
            options.border_size = (int)size;
        }
    }

    Run(options);

    return EXIT_SUCCESS;
}
