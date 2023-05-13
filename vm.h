//
//  vm.h
//  zi23
//
//  Created by Thomas Foster on 1/27/23.
//

#ifndef vm_h
#define vm_h

#include "common.h"
#include <stdint.h>
#include <stdbool.h>

//
// zi5 System File Extensions
//
#define ASM_EXT         ".zis"
#define LANG_EXT        ".zil"
#define EXE_EXT         ".zix"
#define DISK_EXT        ".zid"
#define OUT_NAME        "out"EXE_EXT""

//
// Instruction Encoding
//
// A B C D
// 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
// ------- ----- ----- -----------
// |       |     |     |
// flags   r1    r0    opcode
//
#define OPCODE_MASK     0x1F        // 00000000 00111111
#define REG0_MASK       0x1C0       // 00000001 11000000
#define REG0_SHIFT      6
#define REG1_MASK       0xE00        // 00001110 00000000
#define REG1_SHIFT      9
#define FLAG_A          (1 << 15)    // 10000000 00000000
#define FLAG_B          (1 << 14)    // 01000000 00000000
#define FLAG_C          (1 << 13)    // 00100000 00000000
#define FLAG_D          (1 << 12)    // 00010000 00000000
#define FLAGS_MASK      (FLAG_A | FLAG_B | FLAG_C | FLAG_D)

typedef enum {
    A,
    B,
    C,
    X,
    Y,
    Z,
    SP, // stack pointer
    IP, // instruction pointer
    FLAGS, // processor status
    NUM_REGS,
} Reg; // Processor registers

// https://www.atariarchives.org/mlb/appendix_a.php

typedef enum {
    /*
     *  NOP - No operation
     *  1 byte
     */
    OPCODE_NOP, // 0

    /*
     *  STOP - terminate program
     */
    OPCODE_STOP, // 1

    /*
     *  LOAD - move memory or immediate value to register
     *  STORE - move register to memory
     *  4 bytes
     *
     *  opcode reg, address (or immediate value, if load)
     *  opcode reg, s:(address)
     *
     *  s = b or w
     *  A: immediate value (load only)
     *  | B: indirect mode
     *  | | C: write byte
     *  | | |
     *  0 0 0 0 0 0 0 0 | 0 0 0 0 0 0 0 0 | 0 0 0 0 0 0 0 0 | 0 0 0 0 0 0 0 0
     *  -------           --- -----------   ---------------------------------
     *  |                 |   |             |
     *  flags             r0  opcode      arg (immediate value or address)
     */
    OPCODE_LOAD, // 2
    OPCODE_STORE, // 3

    /*
     *  LEA - load address of identifier into register
     *  3 bytes
     *
     *  lea a, ident
     *
     *  0 0 0 0 0 0 0 0 | 0 0 0 0 ...
     *  --- -----------   -----------
     *  |   |             |
     *  r0  opcode        address
     */
    OPCODE_LEA, // 4

    /*
     *  PUSH - push register to stack
     *  POP - pop stack to register
     *  1 byte
     *
     *  0 0 0 0 0 0 0 0
     *  --- -----------
     *  |   |
     *  r0  opcode
     */
    OPCODE_PUSH, // 5
    OPCODE_POP, // 6

    /*
     *  PUSHF and POPF - push/pop flags
     *  1 byte
     */
    OPCODE_PUSHF, // 7
    OPCODE_POPF, // 8

    /*
     *  MOVE - move register to register
     *  2 bytes
     *
     *  move a, x
     *
     *              dst   src
     *  0 0 0 0 0 0 0 0 | 0 0 0 0 0 0 0 0
     *              ---   --- -----------
     *              |     |   |
     *              r1    r0  opcode
     */
    OPCODE_MOVE, // 9

    /*
     *  AND, OR, XOR, SHR, SHL
     *  2 or 4 bytes
     *
     *  opcode reg1,  reg2    2 bytes
     *  opcode reg,   #imm    4 bytes
     *  opcode reg,   addr    4 bytes
     *  opcode reg,   (addr)  4 bytes
     *
     *  FLAG_A:    immediate value
     *  FLAG_B:    indirect
     *  FLAG_C:    byte at memory
     *  FLAG_D:    reg to reg
     *
     *  A: immediate value
     *  | B: indirect mode
     *  | | C: byte at location
     *  | | | D: reg to reg
     *  | | | |
     *  0 0 0 0 0 0 0 0 | 0 0 0 0 0 0 0 0 | 0 0 0 0 ...
     *  -------     ---   --- -----------   -----------
     *  |           |     |   |             |
     *  flags       r1    r0  opcode        arg
     */
    OPCODE_AND, // 10
    OPCODE_OR, // 11
    OPCODE_XOR, // 12
    OPCODE_SHR, // 13
    OPCODE_SHL, // 14
    OPCODE_ADD, // 15
    OPCODE_SUB, // 16

    /*
     *  NOT
     *  1 or 3 bytes
     *
     *  not reg
     *  not address
     *
     *  A: register (no operand)
     *  | B: indirect mode for address operand
     *  | | C: address operand is byte location
     *  | | |
     *  0 0 0 0 0 0 0 0 | 0 0 0 0 0 0 0 0 | 0 0 0 ...
     *  -------           --- -----------   -----
     *  |                 |   |
     *  flags             r0  opcode
     */
    OPCODE_NOT,

    // SUB
    // DIV
    // FDIV
    // MUL
    // FMUL


    /*
     *  INC - increment register or memory
     *  DEC - decrement register or memory
     *  2 or 4 bytes
     *
     *  inc reg    2 bytes
     *  inc addr   4 bytes
     *  inc (addr) 4 bytes
     *
     *  A: reg
     *  | B: indirect
     *  | | C: byte arg
     *  | | |
     *  0 0 0 0 0 0 0 0 | 0 0 0 0 0 0 0 0 | 0 0 0 ...
     *  -------           --- -----------
     *  |                 |   |
     *  flags             r0  opcode
     */
    OPCODE_INC,
    OPCODE_DEC,

    OPCODE_CALL,    // 19 `call label` 3 bytes
    OPCODE_RETURN,  // 20 `return` 1 byte

    /*
     * COMP - compare register with immediate value, register, or memory
     * 2 or 4 bytes
     *
     * comp reg, reg (2 bytes)
     * comp reg, imm (4 bytes)
     * comp reg, mem (4 bytes)
     *
     *  A: imm value
     *  | B: indirect
     *  | | C: byte location
     *  | | | D: reg to reg
     *  | | | |
     *  0 0 0 0 0 0 0 0 | 0 0 0 0 0 0 0 0 | 0 0 0 ...
     *  -------     ---   --- -----------
     *  |           |     |   |
     *  flags       r1    r0  opcode
     */
    OPCODE_COMP,

    /*
     *  Jump opcodes
     *  3 bytes
     *
     *  JUMP    - unconditional jump
     *  JZ/JE   - jump if zero / jump if equal
     *  JNZ/JNE - jump if not zero / jump if not equal
     *
     *  format:
     *      jump label
     */
    OPCODE_JUMP,
    OPCODE_JZ,
    OPCODE_JNZ,     // JNE  if not zero / not equal
    OPCODE_JM,      // JL   if minus / less than
    OPCODE_JMZ,     // JLE  if minor or zero / less than or equal than
    OPCODE_JP,      // JG   if positive / greater than
    OPCODE_JPZ,     // JGE  if positive or zero / greater or equal than

    /*
     *  LOOP - repeat while C > 0
     *  4 bytes
     *
     *  loop ident
     *  loop address
     */
    OPCODE_LOOP,

    /*
     *  INT - software interrupt
     *  2 bytes
     *
     *  0 0 0 0 0 0 0 0 | 0 0 0 0 0 0 0 0
     *      -----------   ---------------
     *      |             |
     *      opcode        interrupt number
     */
    OPCODE_INT,

    NUM_OPCODES,
} Opcode;

typedef struct {
    const char * program_file;
    bool no_startup;
    bool fullscreen;
    int border_size;
} StartupOptions;

void Run(StartupOptions options);

#endif /* vm_h */
