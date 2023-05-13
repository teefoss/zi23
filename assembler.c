//
//  compiler.c
//  zi23
//
//  Created by Thomas Foster on 1/28/23.
//
#include "common.h"
#include "assembler.h"
#include "symbol.h"
#include "vm.h"
#include "unresolved.h"
#include "executable.h"

#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TOK_INT = 258,
    TOK_STRING, // 260
    TOK_IDENTIFIER, // 261

    // Keywords
    TOK_KEYWORD_DB,
    TOK_KEYWORD_DW,
    TOK_KEYWORD_DS,
    TOK_KEYWORD_START,
    TOK_KEYWORD_PROC,
    TOK_KEYWORD_END,
    TOK_KEYWORD_BYTE,
    TOK_KEYWORD_WORD,

    // Instructions
    TOK_KEYWORD_NOP,
    TOK_KEYWORD_LOAD,
    TOK_KEYWORD_PUSH,
    TOK_KEYWORD_POP,
    TOK_KEYWORD_PUSHF,
    TOK_KEYWORD_POPF,
    TOK_KEYWORD_JUMP,
    TOK_KEYWORD_MOVE,
    TOK_KEYWORD_STORE,
    TOK_KEYWORD_STOP,
    TOK_KEYWORD_AND,
    TOK_KEYWORD_OR,
    TOK_KEYWORD_NOT,
    TOK_KEYWORD_XOR,
    TOK_KEYWORD_SHR,
    TOK_KEYWORD_SHL,
    TOK_KEYWORD_COMP,
    TOK_KEYWORD_INT,
    TOK_KEYWORD_INC,
    TOK_KEYWORD_DEC,
    TOK_KEYWORD_CALL,
    TOK_KEYWORD_RETURN,
    TOK_KEYWORD_JZ,
    TOK_KEYWORD_JNZ,
    TOK_KEYWORD_JM,
    TOK_KEYWORD_JMZ,
    TOK_KEYWORD_JP,
    TOK_KEYWORD_JPZ,
    TOK_KEYWORD_LEA,
    TOK_KEYWORD_ORIGIN,
    TOK_KEYWORD_LOOP,
    TOK_KEYWORD_ADD,
    TOK_KEYWORD_SUB,

    // Registers
    TOK_KEYWORD_A,
    TOK_KEYWORD_B,
    TOK_KEYWORD_C,
    TOK_KEYWORD_X,
    TOK_KEYWORD_Y,
    TOK_KEYWORD_Z,
    TOK_KEYWORD_SP,
    TOK_KEYWORD_IP,
} TokenType;

typedef enum {
    CAT_NONE,
    CAT_KEYWORD,
    CAT_REGISTER,
    CAT_INSTR,
    CAT_SIZE_SPEC,
} TokCat;

typedef struct {
    int type; // ASCII character or TokenType value.
    TokCat category;
    union {
        u16 int_value;
        u16 reg;
    };
    Symbol * symbol;
    char string[IDENT_MAX];
} Token;

const u8    zero8 = 0;
const u16   zero16 = 0;

char *      input;          // text of source code file
char *      _c;             // pointer to parse location in `input`
int         line_num = 1;   // current line number during parsing
Token       peek;           // the next token
Token       token;          // the current token

static FILE * output;       // program generated during parsing
const u16 program_start = 6; // source code location, after header
u16 program_counter = 0;

u16 entry_point = 0;        // code start, set upon finding .start
bool start_encountered = false; // found a .start directive?

char        printable[2];

int GetChar(void) {
    return *_c++;
}

void UngetChar(void) {
    if ( --_c < input ) {
        FatalError("unget underflow");
    }
}

const char * TokenName(TokenType type)
{
    if ( isprint(type) ) {
        printable[0] = type;
        return printable;
    }

    switch ( type ) {
        case TOK_INT:           return "integer";
        case TOK_STRING:        return "string";
        case TOK_IDENTIFIER:    return "identifier";
        case TOK_KEYWORD_DB:    return "keyword '.db'";
        case TOK_KEYWORD_DW:    return "keyword '.dw'";
        case TOK_KEYWORD_DS:    return "keyword '.ds'";
        case TOK_KEYWORD_START: return "keyword '.start'";
        case TOK_KEYWORD_PROC:  return "keyword 'proc'";
        case TOK_KEYWORD_END:   return "keyword 'end'";
        case TOK_KEYWORD_BYTE:  return "size specifier 'byte'";
        case TOK_KEYWORD_WORD:  return "size specifier 'word'";

        case TOK_KEYWORD_NOP:   return "instruction 'nop'";
        case TOK_KEYWORD_LOAD:  return "instruction 'load'";
        case TOK_KEYWORD_PUSH:  return "instruction 'push'";
        case TOK_KEYWORD_POP:   return "instruction 'pop'";
        case TOK_KEYWORD_PUSHF: return "instruction 'pushf'";
        case TOK_KEYWORD_POPF:  return "instruction 'popf'";
        case TOK_KEYWORD_JUMP:  return "instruction 'jump'";
        case TOK_KEYWORD_MOVE:  return "instruction 'move'";
        case TOK_KEYWORD_STORE: return "instruction 'store'";
        case TOK_KEYWORD_STOP:  return "instruction 'stop'";
        case TOK_KEYWORD_AND:   return "instruction 'and'";
        case TOK_KEYWORD_OR:    return "instruction 'or'";
        case TOK_KEYWORD_NOT:   return "instruction 'not'";
        case TOK_KEYWORD_XOR:   return "instruction 'xor'";
        case TOK_KEYWORD_SHR:   return "instruction 'shr'";
        case TOK_KEYWORD_SHL:   return "instruction 'shl'";
        case TOK_KEYWORD_INT:   return "instruction 'int'";
        case TOK_KEYWORD_COMP:  return "instruction 'comp'";
        case TOK_KEYWORD_INC:   return "instruction 'inc'";
        case TOK_KEYWORD_DEC:   return "instruction 'dec'";
        case TOK_KEYWORD_CALL:  return "instruction 'call'";
        case TOK_KEYWORD_RETURN: return "instruction 'return";
        case TOK_KEYWORD_JZ:    return "instruction 'jz'";
        case TOK_KEYWORD_JNZ:   return "instruction 'jnz'";
        case TOK_KEYWORD_JM:    return "instruction 'jm'";
        case TOK_KEYWORD_JMZ:   return "instruction 'jmz'";
        case TOK_KEYWORD_JP:    return "instruction 'jp'";
        case TOK_KEYWORD_JPZ:   return "instruction 'jpz'";
        case TOK_KEYWORD_LEA:   return "instruction 'lea'";
        case TOK_KEYWORD_ORIGIN: return "directive 'orgin'";
        case TOK_KEYWORD_LOOP:  return "instruction 'loop'";
        case TOK_KEYWORD_ADD:   return "instruction 'add'";
        case TOK_KEYWORD_SUB:   return "instruction 'sub'";

        case TOK_KEYWORD_A:     return "register A";
        case TOK_KEYWORD_B:     return "register B";
        case TOK_KEYWORD_C:     return "register C";
        case TOK_KEYWORD_X:     return "register X";
        case TOK_KEYWORD_Y:     return "register Y";
        case TOK_KEYWORD_Z:     return "register Z";
        case TOK_KEYWORD_SP:    return "register sp";
        case TOK_KEYWORD_IP:    return "register ip";
    }

    return NULL; // unreachable
}

void AdvanceToBoundary(int step)
{
    long position = ftell(output);
    long start = position;

    while ( position % step != 0 ) {
        position++;
    }

    if ( position != start ) {
        fseek(output, position, SEEK_SET);
    }
}

static void DebugPrintToken(void)
{
    printf("token: ");
    if ( isprint(peek.type) ) {
        printf("'%c'\n", peek.type);
    } else {
        printf("%s (\"%s\")\n", TokenName(peek.type), peek.string);
    }
}

bool TokenStrEq(const char * string)
{
    return strncmp(peek.string, string, sizeof(peek.string)) == 0;
}

// Populate `peek` with the next token.
static void GetToken(void)
{
    int c;
    peek.category = CAT_NONE;

    while ( isspace(c = GetChar()) ) {
        if ( c == '\n') {
            line_num++;
        }
    }

    if ( c == EOF ) {
        peek.type = EOF;
        return;
    }

    // Get a special keyword
    if ( c == '.' ) {
        char * p = peek.string;

        while ( isalpha(c = GetChar()) && c != EOF ) {
            *p++ = c;
        }
        UngetChar();
        *p = '\0';

        if ( TokenStrEq("DB") ) {
            peek.type = TOK_KEYWORD_DB;
        } else if ( TokenStrEq("DW") ) {
            peek.type = TOK_KEYWORD_DW;
        } else if ( TokenStrEq("DS") ) {
            peek.type = TOK_KEYWORD_DS;
        } else if ( TokenStrEq("START") ) {
            peek.type = TOK_KEYWORD_START;
        } else if ( TokenStrEq("PROCEDURE") ) {
            peek.type = TOK_KEYWORD_PROC;
        } else if ( TokenStrEq("END") ) {
            peek.type = TOK_KEYWORD_END;
        } else if ( TokenStrEq("ORIGIN") ) {
            peek.type = TOK_KEYWORD_ORIGIN;
        } else {
            FatalError("line %d: bad pseudo-op (%s)", line_num, peek.string);
        }

        peek.category = CAT_KEYWORD;
        return;
    }

    // Get a string literal
    if ( c == '"' ) {
        char * p = peek.string;
        while ( (c = GetChar()) != '"' ) {
            if ( !isprint(c) ) {
                FatalError("line %d: bad character in string literal", line_num);
            }
            *p++ = c;
        }

        *p = '\0';
        peek.type = TOK_STRING;
        return;
    }

    // Get character literal

    if ( c == '\'' ) {
        c = GetChar();
        if ( GetChar() != '\'' ) {
            FatalError("line %d: expected single quote", line_num);
        }

        peek.int_value = c;
        peek.type = CAT_NONE;
        peek.type = TOK_INT;
        return;
    }

    // Get number literal
    if ( isdigit(c) ) {
        char * p = peek.string;
        do {
            *p++ = c;
        } while ( (c = GetChar()) != EOF && (isxdigit(c) || c == 'x' || c == 'X') );

        UngetChar();
        *p = '\0';

        int i;
        if ( strchr(peek.string, 'x') || strchr(peek.string, 'X') ) {
            if ( sscanf(peek.string, "%x", &i) != 1 ) {
                FatalError("syntax: bad hex number");
            }
        } else {
            if ( peek.string[0] == '0' ) { // octal
                if ( sscanf(peek.string, "%o", &i) != 1 ) {
                    FatalError("syntax: bad octal number");
                }
            } else { // dec
                if ( sscanf(peek.string, "%d", &i) != 1 ) {
                    FatalError("syntax: bad decimal number");
                }
            }
        }
        peek.int_value = (uint16_t)i;
        peek.type = TOK_INT;
        peek.category = CAT_NONE;
        return;
    }

    // Get identifier or keyword
    if ( isalpha(c) || c == '_' ) {
        char * p = peek.string;

        do {
            *p++ = c;
            if ( p == &peek.string[IDENT_MAX] ) {
                break;
            }
        } while ( (c = GetChar()) != EOF && (isalnum(c) || c == '_') );

        UngetChar();
        *p = '\0';

        peek.type = TOK_IDENTIFIER;
        peek.symbol = LookupSymbol(peek.string);

        switch ( peek.string[0] ) {
            case 'A':
                if ( TokenStrEq("A") ) {
                    peek.type = TOK_KEYWORD_A;
                    peek.category = CAT_REGISTER;
                    peek.reg = A;
                } else if ( TokenStrEq("AND") ) {
                    peek.type = TOK_KEYWORD_AND;
                    peek.category = CAT_INSTR;
                } else if ( TokenStrEq("ADD") ) {
                    peek.type = TOK_KEYWORD_ADD;
                    peek.category = CAT_INSTR;
                }
                break;
            case 'B':
                if ( TokenStrEq("BYTE") ) {
                    peek.type = TOK_KEYWORD_BYTE;
                    peek.category = CAT_SIZE_SPEC;
                }
                else if ( TokenStrEq("B") ) {
                    peek.type = TOK_KEYWORD_B;
                    peek.category = CAT_REGISTER;
                    peek.reg = B;
                }
                break;
            case 'C':
                if ( TokenStrEq("CALL") ) {
                    peek.type = TOK_KEYWORD_CALL;
                    peek.category = CAT_INSTR;
                }
                else if ( TokenStrEq("COMP") ) {
                    peek.type = TOK_KEYWORD_COMP;
                    peek.category = CAT_INSTR;
                }
                else if ( TokenStrEq("C") ) {
                    peek.type = TOK_KEYWORD_C;
                    peek.category = CAT_REGISTER;
                    peek.reg = C;
                }
                break;
            case 'D':
                if ( TokenStrEq("DEC") ) {
                    peek.type = TOK_KEYWORD_DEC;
                    peek.category = CAT_INSTR;
                }
                break;
            case 'E':
                break;
            case 'I':
                if ( TokenStrEq("INC") ) {
                    peek.type = TOK_KEYWORD_INC;
                    peek.category = CAT_INSTR;
                }
                else if ( TokenStrEq("INT") ) {
                    peek.type = TOK_KEYWORD_INT;
                    peek.category = CAT_INSTR;
                }
                else if ( TokenStrEq("IP") ) {
                    peek.type = TOK_KEYWORD_IP;
                    peek.category = CAT_REGISTER;
                    peek.reg = IP;
                }
                break;
            case 'J':
                if ( TokenStrEq("JUMP") ) {
                    peek.type = TOK_KEYWORD_JUMP;
                    peek.category = CAT_INSTR;
                }
                else if ( TokenStrEq("JZ") || TokenStrEq("JE") ) {
                    peek.type = TOK_KEYWORD_JZ;
                    peek.category = CAT_INSTR;
                }
                else if ( TokenStrEq("JNZ") || TokenStrEq("JNE") ) {
                    peek.type = TOK_KEYWORD_JNZ;
                    peek.category = CAT_INSTR;
                }
                else if ( TokenStrEq("JM") || TokenStrEq("JL") ) {
                    peek.type = TOK_KEYWORD_JM;
                    peek.category = CAT_INSTR;
                }
                else if ( TokenStrEq("JMZ") || TokenStrEq("JLE") ) {
                    peek.type = TOK_KEYWORD_JMZ;
                    peek.category = CAT_INSTR;
                }
                else if ( TokenStrEq("JP") || TokenStrEq("JG") ) {
                    peek.type = TOK_KEYWORD_JP;
                    peek.category = CAT_INSTR;
                }
                else if ( TokenStrEq("JPZ") || TokenStrEq("JGE") ) {
                    peek.type = TOK_KEYWORD_JPZ;
                    peek.category = CAT_INSTR;
                }
                break;
            case 'L':
                if ( TokenStrEq("LEA") ) {
                    peek.type = TOK_KEYWORD_LEA;
                    peek.category = CAT_INSTR;
                }
                else if ( TokenStrEq("LOAD") ) {
                    peek.type = TOK_KEYWORD_LOAD;
                    peek.category = CAT_INSTR;
                }
                else if ( TokenStrEq("LOOP") ) {
                    peek.type = TOK_KEYWORD_LOOP;
                    peek.category = CAT_INSTR;
                }
                break;
            case 'M':
                if ( TokenStrEq("MOVE") ) {
                    peek.type = TOK_KEYWORD_MOVE;
                    peek.category = CAT_INSTR;
                }
                break;
            case 'N':
                if ( TokenStrEq("NOP") ) {
                    peek.type = TOK_KEYWORD_NOP;
                    peek.category = CAT_INSTR;
                }
                else if ( TokenStrEq("NOT") ) {
                    peek.type = TOK_KEYWORD_NOT;
                    peek.category = CAT_INSTR;
                }
                break;
            case 'O':
                if ( TokenStrEq("OR") ) {
                    peek.type = TOK_KEYWORD_OR;
                    peek.category = CAT_INSTR;
                }
                break;

            case 'P':
                if ( strcmp(peek.string, "PUSH") == 0 ) {
                    peek.type = TOK_KEYWORD_PUSH;
                    peek.category = CAT_INSTR;
                }
                else if ( strcmp(peek.string, "POP") == 0 ) {
                    peek.type = TOK_KEYWORD_POP;
                    peek.category = CAT_INSTR;
                }
                else if ( strcmp(peek.string, "PUSHF") == 0 ) {
                    peek.type = TOK_KEYWORD_PUSHF;
                    peek.category = CAT_INSTR;
                }
                else if ( strcmp(peek.string, "POPF") == 0 ) {
                    peek.type = TOK_KEYWORD_POPF;
                    peek.category = CAT_INSTR;
                }
                break;

            case 'R':
                if ( strcmp(peek.string, "RETURN") == 0 ) {
                    peek.type = TOK_KEYWORD_RETURN;
                    peek.category = CAT_INSTR;
                }
                break;

            case 'S':
                if ( strcmp(peek.string, "SHL") == 0 ) {
                    peek.type = TOK_KEYWORD_SHL;
                    peek.category = CAT_INSTR;
                }
                else if ( strcmp(peek.string, "SHR") == 0 ) {
                    peek.type = TOK_KEYWORD_SHR;
                    peek.category = CAT_INSTR;
                }
                else if ( strcmp(peek.string, "STOP") == 0 ) {
                    peek.type = TOK_KEYWORD_STOP;
                    peek.category = CAT_INSTR;
                }
                else if ( strcmp(peek.string, "STORE") == 0 ) {
                    peek.type = TOK_KEYWORD_STORE;
                    peek.category = CAT_INSTR;
                }
                else if ( TokenStrEq("SP") ) {
                    peek.type = TOK_KEYWORD_SP;
                    peek.category = CAT_REGISTER;
                    peek.reg = SP;
                }
                else if ( TokenStrEq("SUB") ) {
                    peek.type = TOK_KEYWORD_SUB;
                    peek.category = CAT_INSTR;
                }
                break;
            case 'W':
                if ( TokenStrEq("WORD") ) {
                    peek.type = TOK_KEYWORD_WORD;
                    peek.category = CAT_SIZE_SPEC;
                }
            case 'X':
                if ( TokenStrEq("X") ) {
                    peek.type = TOK_KEYWORD_X;
                    peek.category = CAT_REGISTER;
                    peek.reg = X;
                }
                else if ( strcmp(peek.string, "XOR") == 0 ) {
                    peek.type = TOK_KEYWORD_XOR;
                    peek.category = CAT_INSTR;
                }
                break;
            case 'Y':
                if ( TokenStrEq("Y") ) {
                    peek.type = TOK_KEYWORD_Y;
                    peek.category = CAT_REGISTER;
                    peek.reg = Y;
                }
                break;
            case 'Z':
                if ( TokenStrEq("Z") ) {
                    peek.type = TOK_KEYWORD_Z;
                    peek.category = CAT_REGISTER;
                    peek.reg = Z;
                }
                break;
            default:
                break;
        }

        return;
    }

    peek.type = c;
}

bool Accept(int token_type)
{
    if ( token_type == peek.type ) {
        token = peek;
        GetToken();
        DebugPrintToken();
        return true;
    }

    return false;
}

void Match(int token_type)
{
    if ( !Accept(token_type) ) {
        if ( isprint(token_type) ) {
            FatalError("line %d: expected '%c'", line_num, token_type);
        } else {
            FatalError("line %d: expected %s", line_num, TokenName(token_type));
        }
    }
}

bool AcceptCategory(int cat)
{
    if ( cat == peek.category ) {
        token = peek;
        GetToken();
        DebugPrintToken();
        return true;
    }

    return false;
}

void MatchCategory(TokCat cat)
{
    if ( !AcceptCategory(cat) ) {
        char * message = NULL;
        switch ( cat ) {
            case CAT_NONE: message = ""; break;
            case CAT_KEYWORD: message = "keyword"; break;
            case CAT_REGISTER: message = "register"; break;
            case CAT_INSTR: message = "instruction"; break;
            case CAT_SIZE_SPEC: message = "size specifier"; break;
        }
        FatalError("line %d: expected %s", line_num, message);
    }
}

#pragma mark -

#if 0
void Write(u16 value, int size)
{
    u8 byte;

    if ( size == 1 ) {
        byte = (u8)value;
        fwrite(&byte, sizeof(byte), 1, output);
    } else if ( size == 2 ) {
        fwrite(&value, sizeof(value), 1, output);
    } else {
        FatalError("Write: bad size (should be 1 or 2)");
    }

    program_counter += size;
}
#endif

void WriteWord(u16 value)
{
    fwrite(&value, 2, 1, output);
    program_counter += 2;
}

void WriteByte(u8 value)
{
    fwrite(&value, 1, 1, output);
    program_counter++;
}

int ParseSizeSpecifier(void)
{
    int bytes = 2;
    if ( Accept(TOK_KEYWORD_BYTE) ) {
        bytes = 1;
        Match(':');
    } else if ( Accept(TOK_KEYWORD_WORD) ) {
        // default, do nothing
        Match(':');
    }

    return bytes;
}

// Parsed a reference to a symbol. If the symbol is not found or not yet
// resolved, it's added to the unresolved list and zero is returned.
u16 GetSymbolLocation(const char * symbol_name)
{
    Symbol * sym = LookupSymbol(symbol_name);

    if ( sym && sym->resolved ) {
        return sym->address;
    } else {
        if ( sym == NULL ) {
            InstallSymbol(symbol_name, UNDEFINED, UNRESOLVED_LOCATION);
        }

        int file_position = (int)ftell(output);
        PushUnresolvedLocation(file_position, line_num, symbol_name);
        printf("unknown symbol '%s', at %d\n", symbol_name, file_position);
        return 0;
    }
}

typedef struct {
    enum { ARG_REG, ARG_INT, ARG_ID } type;
    union {
        u16 reg;
        u16 value;
    };
    int size;
    bool brackets;
} Arg;

Arg GetInstructionArg(void)
{
    Arg arg;

    arg.size = ParseSizeSpecifier();
    arg.brackets = Accept('[');

    if ( Accept(CAT_REGISTER) ) {
        arg.type = ARG_REG;
        arg.reg = token.reg;
    } else if ( Accept(TOK_INT) ) {
        arg.type = ARG_INT;
        arg.value = token.int_value;
    } else if ( Accept(TOK_IDENTIFIER) ) {
        arg.type = ARG_ID;
        arg.value = GetSymbolLocation(token.string);
    } else {
        FatalError("line %d: expected register, integer, or identifier", line_num);
    }

    if ( arg.brackets ) {
        Match(']');
    }

    return arg;
}

// Parse an integer literal or identifer
u16 ParseValueFromIntOrIdent(void)
{
    u16 value = 0;

    if ( Accept(TOK_INT) ) {
        value = token.int_value;
    } else if ( Accept(TOK_IDENTIFIER) ) {
        value = GetSymbolLocation(token.string);
    } else {
        FatalError("line %d: expected integer or identifier after ','", line_num);
    }

    return value;
}

typedef struct { u16 flags, value; } InstructionInfo_t;

#if 0
InstructionInfo_t ParseArg(bool accept_immediate)
{
    InstructionInfo_t info = { 0 };

    if ( Accept(TOK_INT) ) {
        if ( Accept('#') ) {
            info.flags |= FLAG_A;
            MatchCategory(CAT_VALUE);
            info.value = token.int_value;
            return info;
        }
    }

    int size = ParseSizeSpecifier();
    if ( size == 1 ) {
        info.flags |= FLAG_C;
    }

    if ( Accept('(') ) {
        info.flags |= FLAG_B;
    }

    info.value = ParseValueFromIntOrIdent();

    if ( info.flags & FLAG_B ) {
        Match(')');
    }

    return info;
}
#endif

#pragma mark -

/*
 load a, [b]
 load a, 0xFF       FF
 load a, (byte) [0xFF]     the contents of address FF
 load a, ident      load the contents of ident
 load a, [ident]    ident contains an address load its contents
 */
void ParseLoadInstruction(void)
{
    u16 i = OPCODE_LOAD;

    MatchCategory(CAT_REGISTER);
    i |= token.reg << REG0_SHIFT;
    Match(',');

    Arg arg = GetInstructionArg();

    if ( arg.size == 1 ) {
        i |= FLAG_C;
    }

    switch ( arg.type ) {
        case ARG_REG:
            if ( !arg.brackets ) {
                FatalError("line %d: expected '['", line_num);
            }
            i |= arg.reg << REG1_SHIFT;
            i |= FLAG_D;
            break;
        case ARG_INT:
            if ( !arg.brackets ) {
                i |= FLAG_A; // immediate value
            }
            break;
        case ARG_ID:
            if ( arg.brackets ) {
                i |= FLAG_B;
            }
            break;
        default:
            break;
    }

    WriteWord(i);
    WriteWord(arg.value);
}

/*
 store a, [b]
 store a, [0xFF]        store at address 0xFF
 store a, ident         store in ident
 store a, [ident]       store at address contained in ident
 */
void ParseStoreInstruction(void)
{
    u16 i = OPCODE_STORE;

    MatchCategory(CAT_REGISTER);
    i |= token.reg << REG0_SHIFT;
    Match(',');

    Arg arg = GetInstructionArg();

    if ( arg.size == 1 ) {
        i |= FLAG_C;
    }

    switch ( arg.type ) {
        case ARG_REG:
            if ( !arg.brackets ) {
                FatalError("line %d: expected '['", line_num);
            }
            i |= arg.reg << REG1_SHIFT;
            i |= FLAG_D;
            break;
        case ARG_INT:
            if ( !arg.brackets ) {
                FatalError("line %d: expected '['", line_num);
            }
            break;
        case ARG_ID:
            if ( arg.brackets ) {
                i |= FLAG_B;
            }
            break;
        default:
            FatalError("line %d: expected memory address", line_num);
            break;
    }

    WriteWord(i);
    WriteWord(arg.value);
}

u16 DefineNewSymbol(int type)
{
    Symbol * sym = LookupSymbol(token.string);

    if ( sym == NULL ) {
        InstallSymbol(token.string, type, program_counter);
    } else if ( !sym->resolved ) {
        // Symbol was previosly referenced, resolve it.
        sym->resolved = true;
        sym->address = program_counter;
    } else {
        FatalError("line %d: label '%s' already defined", token.string);
    }

    return program_counter;
}

void ParseLabel(void)
{
    DefineNewSymbol(LABEL);
    Match(':');
}

void ParsePushPopInstruction(Opcode opcode)
{
    u16 i = opcode;

    MatchCategory(CAT_REGISTER);
    i |= token.reg << REG0_SHIFT;

    WriteWord(i);
}

void ParseJumpInstruction(Opcode opcode)
{
    u16 op = opcode;
    WriteWord(op);

    u16 address = ParseValueFromIntOrIdent();
    WriteWord(address);
}

/*                  ABCD
 instr reg, reg     ---x
 instr reg, [reg]   -x-x
 instr reg, lit     x---
 instr reg, [lit]   ----
 instr reg, ident   ----
 instr reg, [ident] -x--
 */
void ParseBinaryOpInstruction(Opcode opcode)
{
    u16 i = opcode;
    u16 arg = 0;

    MatchCategory(CAT_REGISTER);
    i |= token.reg << REG0_SHIFT;

    Match(',');

    bool brackets = false;

    if ( Accept(TOK_KEYWORD_BYTE) ) {
        i |= FLAG_C; // byte at memory location
    }

    if ( Accept('[') ) {
        brackets = true;
    }

    if ( Accept(TOK_INT) ) {
        if ( !brackets ) {
            i |= FLAG_A; // immediate value
        }
        arg = token.int_value;
    } else if ( AcceptCategory(CAT_REGISTER) ) {
        if ( brackets ) {
            i |= FLAG_B; // indirect
        }
        i |= token.reg << REG1_SHIFT;
        i |= FLAG_D; // reg to reg (no arg)
    } else if ( Accept(TOK_IDENTIFIER) ) {
        if ( brackets ) {
            i |= FLAG_B;
        }
        arg = GetSymbolLocation(token.string);
    }

    if ( brackets ) {
        Match(']');
    }

    WriteWord(i);
    if ( !(i & FLAG_D) ) {
        WriteWord(arg);
    }
}

void ParseCompInstruction(void)
{
    u16 i = OPCODE_COMP;
    u16 arg = 0;

    MatchCategory(CAT_REGISTER);
    i |= token.reg << REG0_SHIFT;

    Match(',');

    if ( AcceptCategory(CAT_REGISTER) ) {
        i |= FLAG_D; // reg/reg flag
        i |= token.reg << REG1_SHIFT;
    } else if ( Accept(TOK_INT) ) {
        i |= FLAG_A; // immediate value
        arg = token.int_value;
    } else {
        int size = ParseSizeSpecifier();
        if ( size == 1 ) {
            i |= FLAG_C;
        }

        Match('[');

        if ( Accept('[') ) { // indirect
            i |= FLAG_B;
        }

        if ( Accept(TOK_IDENTIFIER) ) {
            arg = GetSymbolLocation(token.string);
        } else if ( Accept(TOK_INT) ) {
            arg = token.int_value;
        } else {
            FatalError("line %d: expected address or identifier");
        }

        if ( i & FLAG_B ) {
            Match(']');
        }

        Match(']');
    }

    WriteWord(i);

    if ( !(i & FLAG_D) ) {
        WriteWord(arg);
    }
}

// inc (byte) reg
// inc (byte) [reg]
// inc [int]
// inc id
// inc [id]

void ParseIncDecInstruction(Opcode opcode)
{
    u16 i = opcode;

    if ( AcceptCategory(CAT_REGISTER) ) {
        i |= FLAG_A;
        i |= (token.reg << REG0_SHIFT);
        WriteWord(i);
        return;
    }

    int size = ParseSizeSpecifier();

    if ( size == 1 ) {
        i |= FLAG_C;
    }

    if ( Accept('(') ) {
        i |= FLAG_B;
    }

    u16 address = ParseValueFromIntOrIdent();

    if ( i & FLAG_B ) {
        Match(')');
    }

    WriteWord(i);
    WriteWord(address);
}

// not (byte) reg
// not (byte) [reg]
// not (byte) int
// not (byte) [int]
// not (byte) id
// not (byte) [id]

void ParseNotInstruction(void)
{
    u16 i = OPCODE_NOT;
    Arg arg = GetInstructionArg();

    if ( arg.size == 1 ) {
        i |= FLAG_C;
    }

    if ( arg.type == ARG_REG ) {
        i |= token.reg << REG0_SHIFT;
        i |= FLAG_A;
        if ( arg.brackets ) {
            i |= FLAG_B;
        }
        WriteWord(i);
        return;
    } else if ( arg.type == ARG_INT) {
        if ( !arg.brackets ) {
            FatalError("line %d: expected '['", line_num);
        }
    } else if ( arg.type == ARG_ID ) {
        if ( arg.brackets ) {
            i |= FLAG_B;
        }
    }

    WriteWord(i);
    WriteWord(arg.value);
}

void ParseInstruction(void)
{
    u16 i16;

    switch ( token.type ) {

        case TOK_KEYWORD_ADD:
            ParseBinaryOpInstruction(OPCODE_ADD);
            break;

        case TOK_KEYWORD_AND:
            ParseBinaryOpInstruction(OPCODE_AND);
            break;

        case TOK_KEYWORD_CALL:
            i16 = OPCODE_CALL;
            WriteWord(i16);

            Match(TOK_IDENTIFIER);
            u16 location = GetSymbolLocation(token.string);
            WriteWord(location);
            break;

        case TOK_KEYWORD_COMP:
            ParseCompInstruction();
            break;

        case TOK_KEYWORD_DEC:
            ParseIncDecInstruction(OPCODE_DEC);
            break;

        case TOK_KEYWORD_INC:
            ParseIncDecInstruction(OPCODE_INC);
            break;

        case TOK_KEYWORD_INT:
            Match(TOK_INT);
            i16 = OPCODE_INT | (token.int_value << 8);
            WriteWord(i16);
            break;

        case TOK_KEYWORD_JUMP:
            ParseJumpInstruction(OPCODE_JUMP);
            break;

        case TOK_KEYWORD_JZ:
            ParseJumpInstruction(OPCODE_JZ);
            break;

        case TOK_KEYWORD_JNZ:
            ParseJumpInstruction(OPCODE_JNZ);
            break;

        case TOK_KEYWORD_JM:
            ParseJumpInstruction(OPCODE_JM);
            break;

        case TOK_KEYWORD_JMZ:
            ParseJumpInstruction(OPCODE_JMZ);
            break;

        case TOK_KEYWORD_JP:
            ParseJumpInstruction(OPCODE_JP);
            break;

        case TOK_KEYWORD_JPZ:
            ParseJumpInstruction(OPCODE_JPZ);
            break;

        case TOK_KEYWORD_LEA:
            i16 = OPCODE_LEA;
            MatchCategory(CAT_REGISTER);
            i16 |= token.reg << REG0_SHIFT;
            WriteWord(i16);

            Match(',');
            Match(TOK_IDENTIFIER);
            u16 address = GetSymbolLocation(token.string);
            WriteWord(address);
            break;

        case TOK_KEYWORD_LOAD:
            ParseLoadInstruction();
            break;

        case TOK_KEYWORD_LOOP: {
            i16 = OPCODE_LOOP;
            WriteWord(i16);
            u16 arg = 0;
            if ( Accept(TOK_IDENTIFIER) ) {
                arg = GetSymbolLocation(token.string);
            } else if ( Accept(TOK_INT) ) {
                arg = token.int_value;
            } else {
                FatalError("line %d: expected identifier or integer", line_num);
            }
            WriteWord(arg);
            break;
        }

        case TOK_KEYWORD_NOP:
            i16 = OPCODE_NOP;
            WriteWord(i16);
            break;

        case TOK_KEYWORD_NOT:
            ParseNotInstruction();
            break;

        case TOK_KEYWORD_OR:
            ParseBinaryOpInstruction(OPCODE_OR);
            break;

        case TOK_KEYWORD_PUSH:
            ParsePushPopInstruction(OPCODE_PUSH);
            break;

        case TOK_KEYWORD_PUSHF:
            i16 = OPCODE_PUSHF;
            WriteWord(i16);
            break;

        case TOK_KEYWORD_POP:
            ParsePushPopInstruction(OPCODE_POP);
            break;

        case TOK_KEYWORD_POPF:
            i16 = OPCODE_POPF;
            WriteWord(i16);
            break;

        case TOK_KEYWORD_MOVE:
            MatchCategory(CAT_REGISTER);
            i16 = OPCODE_MOVE;
            i16 |= token.reg << REG0_SHIFT;
            Match(',');
            MatchCategory(CAT_REGISTER);
            i16 |= token.reg << REG1_SHIFT;
            WriteWord(i16);
            break;

        case TOK_KEYWORD_RETURN:
            i16 = OPCODE_RETURN;
            WriteWord(i16);
            break;

        case TOK_KEYWORD_SHL:
            ParseBinaryOpInstruction(OPCODE_SHL);
            break;

        case TOK_KEYWORD_STORE:
            ParseStoreInstruction();
            break;

        case TOK_KEYWORD_SHR:
            ParseBinaryOpInstruction(OPCODE_SHR);
            break;

        case TOK_KEYWORD_STOP:
            i16 = OPCODE_STOP;
            WriteWord(i16);
            break;

        case TOK_KEYWORD_SUB:
            ParseBinaryOpInstruction(OPCODE_SUB);

        default:
            FatalError("programmer f*@ked up");
            break;
    }
}

void ParseDataDefine(void)
{
    int num_bytes = 0;

    if ( token.type == TOK_KEYWORD_DB ) {
        num_bytes = 1;
    } else if ( token.type == TOK_KEYWORD_DW ) {
        num_bytes = 2;
    }

    Match(TOK_IDENTIFIER);
    DefineNewSymbol(num_bytes == 1 ? DATA8 : DATA16);

    if ( num_bytes == 0 ) { // .DS (define space)
        Match(TOK_INT);
        for ( int i = 0; i < token.int_value; i++ ) {
            WriteByte(zero8);
        }
    } else { // .DB or .DW (define byte or word)
        // Parse comma-separated data list:
        do {
            if ( Accept(TOK_STRING) ) {
                if ( num_bytes != 1 ) {
                    FatalError("string literal must be in a .db statement");
                }

                for ( char * c = token.string; *c; c++ ) {
                    WriteByte(*c);
                }
            } else {
                Match(TOK_INT);
                if (   (num_bytes == 1 && token.int_value > 0xFF)
                    || (num_bytes == 2 && token.int_value > 0xFFFF) )
                {
                    FatalError("line %d: data item '%d' too large for"
                               "specified size.", token.int_value);
                }

                if ( num_bytes == 1 ) {
                    WriteByte((u8)token.int_value);
                } else {
                    WriteWord(token.int_value);
                }
            }
        } while ( Accept(',') );
    }
}

void CompleteSection(void)
{
    long current_position = ftell(output);

    Section * last = PeekSectionList();
    if ( last ) {
        // Set size for last section that was entered.
        last->size = current_position - last->file_location;
    }
}

bool ParseProgram(void)
{
    // instruction
    if ( AcceptCategory(CAT_INSTR) ) {
        ParseInstruction();
        return true;
    }

    if (   Accept(TOK_KEYWORD_DB)
        || Accept(TOK_KEYWORD_DW)
        || Accept(TOK_KEYWORD_DS) )
    {
        ParseDataDefine();
        return true;
    }

    if ( Accept(TOK_KEYWORD_END) ) {
        u16 instr = OPCODE_STOP;
        WriteWord(instr);

        CompleteSection();
        return false;
    }

    if ( Accept(TOK_KEYWORD_START) ) {
        if ( start_encountered ) {
            FatalError("line %d: multiple .start directives");
        }

        start_encountered = true;
        entry_point = program_counter;
        return true;
    }

    // label
    if ( Accept(TOK_IDENTIFIER) ) {
        ParseLabel();
        return true;
    }

    if ( Accept(EOF) ) {
        FatalError("missing keyword 'end'");
    }

    if ( Accept(TOK_KEYWORD_PROC) ) {
        Match(TOK_IDENTIFIER);
        DefineNewSymbol(LABEL);
        return true;
    }

    if ( Accept(TOK_KEYWORD_ORIGIN) ) {
        CompleteSection();

        Match(TOK_INT);
        program_counter = token.int_value;

        // Add a new section
        PushSection((int)ftell(output), program_counter);
        return true;
    }

    FatalError("line %d: syntax error", line_num);
    return false;
}

#pragma mark -

// Strip comments and make case-insensitive
void PrepareInputText(void)
{
    bool in_string = false; // or character literal

    int i, j;
    for ( i = 0, j = 0; input[i] != '\0'; i++ ) {
        if ( input[i] == '"' || input[i] == '\'') {
            in_string = !in_string;
        }

        if ( input[i] == ';' && !in_string ) {
            while ( input[i] != '\n' && input[i] != '\0' ) {
                ++i;
            }
        }

        if ( in_string ) {
            input[j++] = input[i];
        } else {
            input[j++] = toupper(input[i]);
        }
    }

    input[j] = '\0';
}

char * BufferTextFile(const char * file_name)
{
    FILE * file = fopen(file_name, "r");

    if ( file == NULL ) {
        FatalError("could not open '%s'.", file_name);
    }

    long size = FileSize(file);
    char * buffer = malloc(size + 1);
    if ( buffer ) {
        fread(buffer, 1, size, file);
        buffer[size] = '\0';
    }

    fclose(file);
    return buffer;
}

void Assemble(const char * file_name)
{
    input = BufferTextFile(file_name);
    if ( input == NULL ) {
        FatalError("could not allocate input file buffer");
    }

    // TODO: create temp file and dup on successful completion(?)
    output = fopen(OUT_NAME, "wb");
    if ( output == NULL ) {
        FatalError("could not create output program file.");
    }

    // Leave space for the header.
    fseek(output, sizeof(Header), SEEK_SET);

    // Setup parser.
    PrepareInputText();
    _c = input;
    GetToken();
    DebugPrintToken();
    PushSection(ftell(output), program_counter);

    while ( ParseProgram() )
        ;

    Header header = {
        .entry_point = entry_point,
        .section_list_offset = ftell(output),
        .num_sections = 0
    };

    // Write section table at end of file.
    Section sec;
    while ( PopSection(&sec) ) {
        header.num_sections++;
        fwrite(&sec, sizeof(sec), 1, output);
    }

    // The section at the bottom of the stack is the first section
    // in the program. Set the start of the section as the entry point if
    // no .start directive was encountered during parsing.
    if ( !start_encountered ) {
        header.entry_point = sec.mem_location;
    }

    // Write header at start of file.
    rewind(output);
    fwrite(&header, sizeof(header), 1, output);

    ResolveLocations(output);

    PrintSymbolTable();
    FreeSymbolTable();
    fclose(output);

#if 1
    output = fopen("out.zix", "rb");
    if ( output ) {
        FILE * log = fopen("log.txt", "w");
        fputs("program bytes:\n", log);
        int off = 0;
        u8 data;
        while ( fread(&data, 1, 1, output) == 1 ) {
            if ( isprint(data) ) {
                fprintf(log, "offset %3d: %3d (%s) %c\n",
                        off, data, BitString(data), data);
            } else {
                fprintf(log, "offset %3d: %3d (%s)\n",
                        off, data, BitString(data));
            }
            off++;
        }

        printf("offset: %d\n", off);
        fprintf(log, "\n\n");

        // Read out the program bytes - for pasting into code.c
        int i = 0;
        rewind(output);
        while ( !feof(output) ) {
            if ( fread(&data, 1, 1, output) != 1 ) {
                perror("Error reading file");
            }

            fprintf(log, "0x%02X, ", data);

            if ( (i + 1) % 8 == 0 ) {
                fprintf(log, "\n");
            }
            i++;
        }

        fprintf(log, "\n");
        fclose(log);
        fclose(output);
        system("open -a TextEdit log.txt");
    }
#endif

    free(input);
}
