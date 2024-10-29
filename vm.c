//
//  vm.c
//  zi23
//
//  Created by Thomas Foster on 1/27/23.
//

#include "vm.h"
#include "common.h"
#include "executable.h"
#include "keybuf.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <SDL.h>

#define ADDR_STARTUP        0x7000 // 512 bytes
#define ADDR_BORDER_COLOR   0x7200 // 1 byte
#define ADDR_FONT_WIDTH     0x7201 // 1 byte
#define ADDR_FONT_HEIGHT    0x7202 // 1 byte
#define ADDR_STACK          0x7400 // to 77FF (1,024 bytes)
#define ADDR_CHAR_MAPS      0x7800 // to 7FFF (2,048 bytes)
#define ADDR_VBUF           0x8000 // to FD00 (32,000 bytes)

#define CLOCK_SPEED_HZ 4770000L
#define CYCLES_PER_INSTRUCTION 6 // TODO: ?
#define STEP_TIME_NS (NSEC_PER_SEC / CLOCK_SPEED_HZ * CYCLES_PER_INSTRUCTION)

#define TOTAL_MEMORY 0x10000 // 16K
#define CALL_STACK_SIZE 1024
#define VBUF_SIZE (SCREEN_W * SCREEN_H / 2) // 4 bits per pixel
#define FONT_W mem[ADDR_FONT_WIDTH]
#define FONT_H mem[ADDR_FONT_HEIGHT]
#define VBUF_PIXEL(x, y) mem[ADDR_VBUF + (y * RES_W + x) / 2]
#define SCREEN_W 320 // actual screen area, inside border region
#define SCREEN_H 200
#define TEXTURE_W 640
#define TEXTURE_H 400

#define DEFAULT_TEXT_COLOR          0xF;
#define DEFAULT_BACKGROUND_COLOR    0x0;
#define DEFAULT_BORDER_COLOR        0x0;

// Processor status flags
typedef enum {
    FLAG_NEGATIVE   = 0x01,
    FLAG_ZERO       = 0x02,
} flags_t;

//
// Interrupt Register Rules
//
// - Single argument interrupts typically use Z
// - Multiple argument interrupts typically use X, Z or X, Y, Z etc.
// - Coordinate arguments go in X, Y
// - Count arguments go in C
//

//
// Interrupt Categories
// Video
// - Set mode (V)
// - TODO: Get video state (mode, cols/resolution) (V)
// - Get/Set window size (V)
// - Get/Set window position (V)
// - Set windows/fullscreen (V)
// - TODO: Get/Set pixel (set with XOR option) (V)
// - TODO: Set Palette substitution (Pal x uses Pal y) (V)
//
// - Write char at current position, no advance (C)
// - Write char+attr at current position, no advance (C)
// - Read char+attr (C)
// - Write String (C)
// - Write Integer (C)
// - Get/Set cursor (C)
// - Show/Hide cursor (C)
// - Scroll up/down (C)
//
// Input
// - TODO: Get mouse position (I)
// - Read keyboard (wait for key) (I)
//
// System
// - Sleep (S)


/// Set video mode, clear video buffer and reset console color and cursor.
/// - Z: mode number (0-3)
void InterruptSetMode(void);

/// Set monitor size. X, Y holds width and height.
void InterruptSetWindowSize(void);

/// Get monitor size. X holds width, Y holds height.
void InterruptGetWindowSize(void); // in X, Y

void InterruptSetFullscreen(void); // -
void InterruptSetWindowed(void); // -
void InterruptGetKeyState(void); // A: state

/// Write character with current console attribute. The cursor is advanced.
/// - Z: character to write
/// - C: count
void InterruptWriteChar(void);

void InterruptReadChar(void); // ?

/// - Z: character
/// - X: attribute
/// - C: number of times to write character
void InterruptWriteCharAttr(void);
void InterruptWriteString(void); // A: address of string
void InterruptWriteStringAttr(void);

void InterruptSetCursorVisibility(void); // A: 1 = yes, 0 = no
void InterruptSetCursorPosition(void); // X, Y: position

void InterruptScrollUp(void);
void InterruptScrollDown(void);
void InterruptWriteInteger(void); // A: integer to write
void InterruptGetCursorPosition(void);
void InterruptSleep(void);
void InterruptSetPixel(void);
void InterruptGetPixel(void);
void InterruptReadKeyboard(void);
void InterruptGetKeyboardStatus(void);
// TODO: void InterruperReadMouse(void);

void ScrollConsole(u8 num_lines, bool scroll_up, const u16 * clear_attribute);
void RenderScreen(void);

typedef enum {
    VIDEO_MODE_TEXT_40_COL, // 320 x 200
    VIDEO_MODE_TEXT_80_COL, // 640 x 400
    VIDEO_MODE_GRAPHICS_LO, // 160 x 100
    VIDEO_MODE_GRAPHICS_HI, // 320 x 200
    NUM_VIDEO_MODES,
} VideoMode;

typedef union {
    struct { int rows, cols; } console;
    struct { int width, height; } resolution;
} VideoModeInfo;

VideoModeInfo video_mode_info[NUM_VIDEO_MODES] = {
    [VIDEO_MODE_TEXT_40_COL] = { .console = { 25, 40 } },
    [VIDEO_MODE_TEXT_80_COL] = { .console = { 25, 80 } },
    [VIDEO_MODE_GRAPHICS_LO] = { .resolution = { 160, 100 } },
    [VIDEO_MODE_GRAPHICS_HI] = { .resolution = { 320, 200 } },
};

#define ROWS    video_mode_info[video_mode].console.rows
#define COLS    video_mode_info[video_mode].console.cols
#define RES_W   video_mode_info[video_mode].resolution.width
#define RES_H   video_mode_info[video_mode].resolution.height

void (* interrupts[])(void) = {
    [  0 ] = InterruptSetMode,
    [  1 ] = NULL,
    [  2 ] = NULL,
    [  3 ] = InterruptSetWindowSize,
    [  4 ] = InterruptGetWindowSize,
    [  5 ] = InterruptSetFullscreen,
    [  6 ] = InterruptSetWindowed,
    [  7 ] = InterruptGetKeyState,
    [  8 ] = InterruptSetCursorPosition,
    [  9 ] = InterruptWriteChar, // ASCII value in A
    [ 10 ] = InterruptWriteString, // address of string in A
    [ 11 ] = InterruptSetCursorVisibility,
    [ 12 ] = InterruptWriteCharAttr,
    [ 13 ] = InterruptReadChar,
    [ 14 ] = NULL,
    [ 15 ] = NULL,
    [ 16 ] = InterruptScrollUp, // A: num lines or 0, X: attr
    [ 17 ] = InterruptScrollDown,
    [ 18 ] = InterruptWriteInteger,
    [ 19 ] = InterruptGetCursorPosition,
    [ 20 ] = InterruptSleep,
    [ 21 ] = InterruptSetPixel,
    [ 22 ] = InterruptGetPixel,
    [ 23 ] = InterruptReadKeyboard,
    [ 24 ] = InterruptGetKeyboardStatus,
    [ 25 ] = InterruptWriteStringAttr,
};

// System
bool    running;
bool    executing; // status flag?
u16     regs[NUM_REGS];
u8      mem[TOTAL_MEMORY];
u16     call_stack[CALL_STACK_SIZE];
int     call_stack_top = -1;

// Screen
SDL_Window *    monitor; // 'monitor', because 'window' is a console viewport.
SDL_Renderer *  renderer;
u8              video_mode = VIDEO_MODE_TEXT_80_COL;
u8              border_size = 16;

struct {
    SDL_Texture * texture;
    SDL_Texture * scanlines;
    int pitch;
    SDL_PixelFormat * format;
} screen;

// Input

const u8 * key_state;
bool wait_for_key;

// Console
u8      cursor_x;
u8      cursor_y;
bool    show_cursor = true;
u8      blink;
bool    wait_for_refresh;
bool    character_display_sync = true;

SDL_Color pal[16] = {
    { 0x06, 0x06, 0x06 }, // 0 Black
    { 0xff, 0x00, 0x4d }, // 1 Red
    { 0xab, 0x52, 0x36 }, // 2 Brown
    { 0xff, 0x84, 0x26 }, // 3 Orange
    { 0xff, 0xdd, 0x34 }, // 4 Yellow
    { 0x50, 0xe1, 0x12 }, // 5 Lime Green
    { 0x3f, 0xa6, 0x6f }, // 6 Olive Green
    { 0x00, 0xff, 0xcc }, // 7 Teal
    { 0x36, 0x59, 0x87 }, // 8 Navy Blue
    { 0x00, 0x33, 0xff }, // 9 Electric Blue
    { 0x29, 0xad, 0xff }, // A Sky Blue
    { 0x43, 0x00, 0x67 }, // B Dark Purple
    { 0x94, 0x21, 0x6a }, // C Light Purple
    { 0x5f, 0x57, 0x4f }, // D Dark Grey
    { 0xc2, 0xc3, 0xc7 }, // E Light Grey
    { 0xff, 0xf1, 0xe8 }, // F White
};

extern u8 character_map[256 * 8];
//extern u8 atari[256 * 4];

u8 * font = character_map;

#pragma mark - DEBUG

static double SecondsToNanoseconds(double seconds) {
    return seconds * 1e9;
}

static const char * RegName(Reg r)
{
    static const char * names[] = { "A", "X", "Y", "SP", "IP", "FLAGS" };
    return names[r];
}

void PrintRegs(void)
{
    printf("registers:\n");
    for ( Reg r = 0; r < FLAGS; r++ ) {
        printf("  %-8s %04X (%d)\n", RegName(r), regs[r], regs[r]);
    }
    printf("  %-8s %s\n", RegName(FLAGS), BitString(regs[FLAGS]));
}

void PrintMemory(int start, int end)
{
    u8 m;

    if ( start == end ) {
        m = mem[start];
        printf("  byte at %04X: %3d %04X %s\n", start, m, m, BitString(m));
    } else {
        printf("\nmemory %04X-%04x:\n", start, end);
        for ( int i = start; i <= end; i++ ) {
            u8 m = mem[i];
            printf("  %04X(%+4d): %3d %02X %s (%04X)\n",
                   i, i - start, m, m, BitString(m), *(u16 *)&mem[i]);
        }
    }
}

void PrintMemoryRaw(int start, int end)
{
    if ( start == end ) {
        printf("%02X\n", mem[start]);
    } else {
        for ( int i = start; i <= end; i++ ) {
            printf("0x%02X, ", mem[i]);
            if ( (i + 1) % 8 == 0 ) {
                printf("\n");
            }
        }
        printf("\n");
    }
}

#pragma mark -

bool IsTextMode(void)
{
    return video_mode == VIDEO_MODE_TEXT_40_COL
        || video_mode == VIDEO_MODE_TEXT_80_COL;
}

/// Write character to console.
///
/// - parameter ch: character to print
/// - parameter x: console x locaiton.
/// - parameter y: console y location.
/// - parameter attr: attribute or NULL to use location's current attribute.
void WriteCharacter(int ch, int x, int y, const u8 * attr)
{
    u16 * vbuf = (u16 *)&mem[ADDR_VBUF];

    if ( IsTextMode() ) {
        if ( attr ) {
            vbuf[y * COLS + x] = ch | (*attr << 8);
        } else {
            // Preserve current attribute and Write character only.
            u16 * cell = &vbuf[y * COLS + x];
            *cell &= 0xFF00; // clear current character
            *cell |= ch;
        }

        if ( character_display_sync ) {
            wait_for_refresh = true;
        }
    } else {
        // TODO: graphics mode
    }
}

void NewLine(void)
{
    cursor_x = 0;
    if ( cursor_y == ROWS - 1 ) { // At the bottom.
        ScrollConsole(1, true, NULL);
    } else {
        ++cursor_y;
    }
}

void AdvanceCursor(void)
{
    if ( ++cursor_x == COLS ) {
        NewLine();
    }
}

void SetPixel(int x, int y, int color)
{
    u8 * ptr = &VBUF_PIXEL(x, y);

    if ( x % 2 == 0 ) {
        *ptr &= ~0xF0; // clear high nybble
        *ptr |= color << 4;
        printf("%d (%s)\n", *ptr, BitString(*ptr));
    } else {
        *ptr &= ~0x0F; // clear low nybble
        *ptr |= color;
        printf("%d (%s)\n", *ptr, BitString(*ptr));
    }
}

u8 GetPixel(int x, int y)
{
    u8 * byte = &VBUF_PIXEL(x, y);
    if ( x % 2 == 0 ) {
        return (*byte & 0xF0) >> 4;
    } else {
        return *byte & 0x0F;
    }
}

void ClearConsoleBuffer(void)
{
    u16 * cbuf = (u16 *)&mem[ADDR_VBUF];

    for ( int i = 0; i < 80 * ROWS; i++ ) {
        cbuf[i] = 0x0F00; // white on black
    }
}


#pragma mark -

void InterruptSetPixel(void)
{
    if ( !IsTextMode() ) {
        SetPixel(regs[X], regs[Y], regs[Z]);
    }
}

void InterruptGetPixel(void)
{
    regs[Z] = GetPixel(regs[X], regs[Y]);
}

void InterruptSetMode(void)
{
    u16 new_mode = regs[Z];
    
    if ( new_mode < 0 || new_mode >= NUM_VIDEO_MODES ) {
        FatalError("invalid video mode");
    }

    video_mode = new_mode;

    if ( IsTextMode() ) {
        ClearConsoleBuffer();
    } else {
        memset(&mem[ADDR_VBUF], 0, VBUF_SIZE);
    }

    cursor_x = 0;
    cursor_y = 0;
    show_cursor = true;
}

void InterruptSetWindowSize(void)
{
    SDL_SetWindowSize(monitor, regs[X], regs[Y]);
    SDL_PumpEvents();
}

void InterruptGetWindowSize(void)
{
    int x, y;
    SDL_GetWindowSize(monitor, &x, &y);
    regs[X] = (u16)x;
    regs[Y] = (u16)y;
}

void InterruptSetFullscreen(void)
{
    SDL_SetWindowFullscreen(monitor, SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void InterruptSetWindowed(void)
{
    SDL_SetWindowFullscreen(monitor, 0);
}

void InterruptSetCursorVisibility(void)
{
    show_cursor = regs[Z] != 0;
}

void InterruptSetCursorPosition(void)
{
    cursor_x = regs[X];
    cursor_y = regs[Y];
}

void InterruptGetKeyState(void)
{
    enum {
        KEY_ESCAPE      = 0x0001,
        KEY_LSHIFT      = 0x0002,
        KEY_RSHIFT      = 0x0004,
        KEY_LCONTROL    = 0x0008,
        KEY_RCONTROL    = 0x0010,
        KEY_LALT        = 0x0020,
        KEY_RALT        = 0x0040,
        KEY_LCOMMAND    = 0x0080,
        KEY_RCOMMAND    = 0x0100,
        KEY_UP          = 0x0200,
        KEY_DOWN        = 0x0400,
        KEY_LEFT        = 0x0800,
        KEY_RIGHT       = 0x1000,
    };

    u16 state = 0;

    if ( key_state[SDL_SCANCODE_ESCAPE] )
        state |= KEY_ESCAPE;
    if ( key_state[SDL_SCANCODE_LSHIFT] )
        state |= KEY_LSHIFT;
    if ( key_state[SDL_SCANCODE_RSHIFT] )
        state |= KEY_RSHIFT;
    if ( key_state[SDL_SCANCODE_LCTRL] )
        state |= KEY_LCONTROL;
    if ( key_state[SDL_SCANCODE_RCTRL] )
        state |= KEY_RCONTROL;
    if ( key_state[SDL_SCANCODE_LALT] )
        state |= KEY_LALT;
    if ( key_state[SDL_SCANCODE_RALT] )
        state |= KEY_RALT;
    if ( key_state[SDL_SCANCODE_LGUI] )
        state |= KEY_LCOMMAND;
    if ( key_state[SDL_SCANCODE_RGUI] )
        state |= KEY_RCOMMAND;
    if ( key_state[SDL_SCANCODE_UP] )
        state |= KEY_UP;
    if ( key_state[SDL_SCANCODE_DOWN] )
        state |= KEY_DOWN;
    if ( key_state[SDL_SCANCODE_LEFT] )
        state |= KEY_LEFT;
    if ( key_state[SDL_SCANCODE_RIGHT] )
        state |= KEY_RIGHT;

    regs[Z] = state;
}

void InterruptWriteChar(void)
{
    u8 ch = regs[Z];
    u8 count = regs[C];

    if ( IsTextMode() ) {
        switch ( ch ) {
            case '\n':
                NewLine();
                break;
            default:
                for ( int i = 0; i < count; i++ ) {
                    WriteCharacter(ch, cursor_x, cursor_y, NULL);
                    AdvanceCursor();
                    if ( cursor_x == 0 ) {
                        // The cursor was advanced to the next line, stop here.
                        break;
                    }
                }
                break;
        }
    } else {
        // TODO: graphics mode
    }
}

/// Read character and attribute at cursor.
void InterruptReadChar(void)
{
    u16 * cbuf = (u16 *)&mem[ADDR_VBUF];

    int index = cursor_y * COLS + cursor_x;
    regs[Z] = cbuf[index] & 0x00FF;         // character
    regs[X] = (cbuf[index] & 0x0F00) >> 8;  // foreground
    regs[Y] = (cbuf[index] & 0xF000) >> 12; // background
}

/// Write character with attribute at cursor position.
/// The cursor position is not advanced.
/// - Z: character
/// - X: text color
/// - Y: background color
/// - C: count
void InterruptWriteCharAttr(void)
{
    u8 ch = regs[Z];
    u8 attr = regs[X];
    u8 count = regs[C];

    if ( IsTextMode() ) {
        for ( int i = 0; i < count; i++ ) {
            int x = cursor_x + count;
            if ( x == COLS ) {
                break;
            }

            WriteCharacter(ch, cursor_x, cursor_y, &attr);
            AdvanceCursor();
        }
    } else {
        // TODO: graphics mode
    }
}

void WriteString(const u8 * string, const u8 * attr)
{
    while ( *string != 0 ) {
        if ( *string == '\n' ) {
            cursor_x = 0;
            cursor_y++;
        } else {
            WriteCharacter(*string, cursor_x, cursor_y, attr);
            AdvanceCursor();
        }
        string++;
    }
}

void InterruptWriteString(void)
{
    WriteString(&mem[regs[Z]], NULL);
}

void InterruptWriteStringAttr(void)
{
    u8 attr = regs[X];
    WriteString(&mem[regs[Z]], &attr);
}

void SetConsoleLines(int start, int end, u16 data)
{
    u16 *cbuf = (u16 *)&mem[ADDR_VBUF];

    for ( int y = start; y <= end; y++ ) {
        for ( int x = 0; x < COLS; x++ ) {
            cbuf[y * COLS + x] = data;
        }
    }
}

/// Erase all characters but keep current attributes
void ClearConsoleLines(int start, int end)
{
    u16 *cbuf = (u16 *)&mem[ADDR_VBUF];

    for ( int y = start; y <= end; y++ ) {
        for ( int x = 0; x < COLS; x++ ) {
            cbuf[y * COLS + x] &= 0xFF00;
        }
    }
}

void ScrollConsole(u8 num_lines, bool scroll_up, const u16 * clear_attribute)
{
    int row_bytes = COLS * 2;
    int gap_bytes = row_bytes * num_lines; // The exposed area after scroll.
    u16 data = 0;
    if ( clear_attribute ) {
        data = *clear_attribute << 8;
    }

    if ( num_lines == 0 ) {
        if ( clear_attribute ) {
            SetConsoleLines(0, ROWS - 1, data); // Blank entire screen
        } else {
            ClearConsoleLines(0, ROWS - 1); // Blank entire screen
        }
    } else {
        u8 * src;
        u8 * dst;
        if ( scroll_up ) {
            src = &mem[ADDR_VBUF] + gap_bytes;
            dst = &mem[ADDR_VBUF];
            memmove(dst, src, (ROWS - num_lines) * row_bytes);
            if ( clear_attribute ) {
                SetConsoleLines(ROWS - num_lines, ROWS - 1, data);
            } else {
                ClearConsoleLines(ROWS - num_lines, ROWS - 1);
            }
        } else {
            src = &mem[ADDR_VBUF];
            dst = &mem[ADDR_VBUF] + gap_bytes;
            memmove(dst, src, (ROWS - num_lines) * row_bytes);
            if ( clear_attribute ) {
                SetConsoleLines(0, num_lines - 1, data);
            } else {
                ClearConsoleLines(0, num_lines - 1);
            }
        }
    }
}

void InterruptScrollUp(void)
{
    ScrollConsole(regs[Z], true, &regs[X]);
}

void InterruptScrollDown(void)
{
    ScrollConsole(regs[Z], false, &regs[X]);
}

void InterruptWriteInteger(void)
{
    char buf[80];
    snprintf(buf, 80, "%d", regs[Z]);

    for ( char * c = buf; *c; c++ ) {
        WriteCharacter(*c, cursor_x, cursor_y, NULL);
        AdvanceCursor();
    }
}

void InterruptGetCursorPosition(void)
{
    regs[X] = cursor_x;
    regs[Y] = cursor_y;
}

// TODO: bool sleep and sleep_end_msec
void InterruptSleep(void)
{
    SDL_Delay(regs[Z]);
}

void InterruptReadKeyboard(void)
{
    SDL_Keysym keysym;

    if ( DequeueKey(&keysym) ) {
        regs[X] = keysym.sym;
        regs[Y] = keysym.scancode;
    } else {
        wait_for_key = true;
    }
}

void InterruptGetKeyboardStatus(void)
{
    if ( KeyBufferIsEmpty() ) {
        regs[Z] = 0;
    } else {
        regs[Z] = 1;

        SDL_Keysym keysym;
        DequeueKey(&keysym);
        regs[X] = keysym.sym;
        regs[Y] = keysym.scancode;
    }
}

#pragma mark -

void SetFlags(u16 value, size_t bytes)
{
    u16 sign_bit = bytes == 1 ? 0x80 : 0x8000;
    if ( value == 0 ) {
        regs[FLAGS] |= FLAG_ZERO;
        regs[FLAGS] &= ~FLAG_NEGATIVE;
    } else if ( value & sign_bit ) {
        regs[FLAGS] |= FLAG_NEGATIVE;
        regs[FLAGS] &= ~FLAG_ZERO;
    } else {
        regs[FLAGS] &= ~(FLAG_ZERO | FLAG_NEGATIVE);
    }
}

void SetFlagsSignBit(u16 reg, int sign_bit)
{
    if ( regs[reg] == 0 ) {
        regs[FLAGS] |= FLAG_ZERO;
        regs[FLAGS] &= ~FLAG_NEGATIVE;
    } else if ( regs[reg] & sign_bit ) {
        regs[FLAGS] |= FLAG_NEGATIVE;
        regs[FLAGS] &= ~FLAG_ZERO;
    } else {
        regs[FLAGS] &= ~(FLAG_ZERO | FLAG_NEGATIVE);
    }
}

float ScanLineIntensity(float coord, float resolution, float opacity)
{
    float intensity = sin(coord * resolution * M_PI * 2.0f);
    intensity = ((0.5f * intensity) + 0.5f) * 0.9f + 0.1f;
    return pow(intensity, opacity);
}

u32 PaletteTo32(int color, SDL_PixelFormat * format)
{
    u8 r = pal[color].r;
    u8 g = pal[color].g;
    u8 b = pal[color].b;

    return SDL_MapRGB(format, r, g, b);
}

void RefreshText(void)
{
    const int total_cells = ROWS * COLS;
    const int bytes_per_char = (FONT_W * FONT_H) / 8;

    u32 * pixels;
    SDL_LockTexture(screen.texture, NULL, (void **)&pixels, &screen.pitch);

    u16 * buf = (u16 *)&mem[ADDR_VBUF];

    for ( int i = 0; i < total_cells; i++ ) {

        u8 ch =  buf[i] & 0x00FF;        // ASCII value
        u8 fg = (buf[i] & 0x0F00) >> 8;  // palette index
        u8 bg = (buf[i] & 0xF000) >> 12; // palette index

        // Get console x, y location.
        int x = (i % COLS);
        int y = (i / COLS);

        u32 fg_pix, bg_pix;
        if ( show_cursor && x == cursor_x && y == cursor_y && blink ) {
            fg_pix = PaletteTo32(bg, screen.format);
            bg_pix = PaletteTo32(fg, screen.format);
        } else {
            fg_pix = PaletteTo32(fg, screen.format);
            bg_pix = PaletteTo32(bg, screen.format);
        }

        // Translate to pixel coordinate.
        x *= FONT_W;
        y *= FONT_H;

        const u8 * data = &font[ch * bytes_per_char];

        // Iterate over bits in character data.
        int bit = 7;
        for ( int row = 0; row < FONT_H; row++ ) {
            for ( int col = 0; col < FONT_W; col++ ) {
                int pixel_index = (y + row) * screen.pitch / 4 + (x + col);

                if ( *data & (1 << bit) ) {
                    pixels[pixel_index] = fg_pix;
                } else {
                    pixels[pixel_index] = bg_pix;
                }

                // Next byte?
                if ( --bit < 0 ) {
                    data++;
                    bit = 7;
                }
            }
        }
    }

    SDL_UnlockTexture(screen.texture);
}

void RefreshVideo(void)
{
    u32 * pixels;
    SDL_LockTexture(screen.texture, NULL, (void **)&pixels, &screen.pitch);

    // Loop video buffer.
    for ( int i = 0; i < RES_W * RES_H; i++ ) {
        u16 addr = ADDR_VBUF + (i / 2);

        u8 c; // palette index

        if ( i % 2 == 0 ) {
            c = (mem[addr] & 0xF0) >> 4;
        } else {
            c = mem[addr] & 0x0F;
        }

        int x = i % RES_W;
        int y = i / RES_W;

        u32 pixel = SDL_MapRGB(screen.format, pal[c].r, pal[c].g, pal[c].b);

        pixels[y * screen.pitch / sizeof(*pixels) + x] = pixel;
    }

    SDL_UnlockTexture(screen.texture);
}

#pragma mark -

bool InitScreen(bool fullscreen)
{
    if ( SDL_Init(SDL_INIT_EVERYTHING) != 0 ) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }

    // Window info
    const float scale = 2.0f;
    const int width = TEXTURE_W + border_size * 2;
    const int height = TEXTURE_H + border_size * 2;
    u32 flags = 0;
    if ( fullscreen ) {
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    monitor = SDL_CreateWindow
    (   "",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width * scale,
        height * scale,
        flags );

    if ( monitor == NULL ) goto error;

    renderer = SDL_CreateRenderer(monitor, -1, SDL_RENDERER_TARGETTEXTURE);
    if ( renderer == NULL ) goto error;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderSetLogicalSize(renderer, width, height);

    screen.texture = SDL_CreateTexture
    (   renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        TEXTURE_W,
        TEXTURE_H );

    u32 format_enum;
    SDL_QueryTexture(screen.texture, &format_enum, NULL, NULL, NULL);
    screen.format = SDL_AllocFormat(format_enum);

//    int scanline_scale = 2;
//    int scanline_texture_h = TEXTURE_H * scanline_scale;
    screen.scanlines = SDL_CreateTexture
    (   renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_TARGET,
        width,
        height ); // "subpixel" scanlines

    SDL_SetTextureBlendMode(screen.scanlines, SDL_BLENDMODE_BLEND);

    if ( screen.texture == NULL || screen.scanlines == NULL ) goto error;

    SDL_SetRenderTarget(renderer, screen.scanlines);
    for ( int y = 0; y < height; y += 2 ) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 16);
        SDL_RenderDrawLine(renderer, 0, y, width - 1, y);
    }

    for ( int x = 0; x < width; x += 2 ) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 16);
        SDL_RenderDrawLine(renderer, x, 0, x, height - 1);
    }

    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderClear(renderer);

    ClearConsoleBuffer();
    SDL_PumpEvents();
    RenderScreen();

    return true;
error:
    printf("InitScreen failed: %s\n", SDL_GetError());
    return false;
}

static void Push(u16 value)
{
    memcpy(&mem[regs[SP]], &value, sizeof(value));
    regs[SP] += 2;
}

static u16 Pop(void)
{
    regs[SP] -= 2;
    return *(u16 *)&mem[regs[SP]];
}

static void PushCallStack(u16 address)
{
    if ( call_stack_top + 1 == CALL_STACK_SIZE ) {
        FatalError("call stack overflow");
    }

    call_stack[++call_stack_top] = address;
}

static u16 PopCallStack(void)
{
    if ( call_stack_top == -1 ) {
        executing = false;
        return 0;
    }

    return call_stack[call_stack_top--];
}

/// Jump to destination on condition. If condition is false, advance to next
/// instruction.
void TryJumpOnCondition(bool condition, u16 destination)
{
    if ( condition ) {
        regs[IP] = destination;
    } else {
        regs[IP] += 4;
    }
}

void ProcessInstruction(void)
{
    // Decode instruction
    u16 instruction = *(u16 *)&mem[regs[IP]];
    Opcode opcode = instruction & OPCODE_MASK;
    Reg r0 = (instruction & REG0_MASK) >> REG0_SHIFT;
    Reg r1 = (instruction & REG1_MASK) >> REG1_SHIFT;
    u16 arg = *(u16 *)&mem[regs[IP] + 2];

#if 0
    printf("\n");
    printf("%04X: %s (%d)\n", regs[IP], OpcodeName(opcode), opcode);
    printf("  r0: %s\n", RegName(r0));
    printf("  r1: %s\n", RegName(r1));
    printf("  flags: %s\n", BitString(flags));
    printf("  arg1: 0x%04X (%d)\n", arg1, arg1);
    printf("  arg2: 0x%04X (%d)\n", arg2, arg2);
#endif

    switch ( opcode ) {

        case OPCODE_CALL:
            PushCallStack(regs[IP] + 4);
            regs[IP] = arg;
            break;

        case OPCODE_COMP: {
            u16 size = instruction & FLAG_C ? 1 : 2;
            u16 value = 0;
            u8 ip_step = 4;

            if ( instruction & FLAG_D ) { // reg/reg
                value = regs[r1];
                ip_step = 2;
            } else if ( instruction & FLAG_A ) { // immediate value
                value = arg;
            } else if ( instruction & FLAG_B ) { // indirect
                memcpy(&arg, &mem[*(u16 *)&mem[arg]], size);
            } else { // direct
                memcpy(&arg, &mem[arg], size);
            }

            SetFlags(regs[r0] - value, size);
            regs[IP] += ip_step;
            break;
        }

        case OPCODE_INC:
        case OPCODE_DEC: {
            int delta = opcode == OPCODE_INC ? 1 : -1;

            if ( instruction & FLAG_A ) {
                regs[r0] += delta;
                regs[IP] += 2;
                SetFlags(regs[r0], 2);
            } else {
                int size = instruction & FLAG_C ? 1 : 2;
                if ( instruction & FLAG_B ) {
                    if ( size == 1 ) {
                        mem[mem[arg]] += delta;
                    } else {
                        *(u16 *)&mem[*(u16 *)&mem[arg]] += delta;
                    }
                } else {
                    if ( size == 1 ) {
                        mem[arg] += delta;
                    } else {
                        (*(u16 *)&mem[arg]) += delta;
                    }
                }
                SetFlags(regs[r0], size);
                regs[IP] += 4;
            }
            break;
        }

        case OPCODE_INT:
            interrupts[(instruction & 0xFF00) >> 8]();
            regs[IP] += 2;
            break;

        case OPCODE_JZ:
            TryJumpOnCondition(regs[FLAGS] & FLAG_ZERO, arg);
            break;

        case OPCODE_JMZ: {
            bool is_minus = (regs[FLAGS] & FLAG_NEGATIVE) != 0;
            bool is_zero = (regs[FLAGS] & FLAG_ZERO) != 0;
            TryJumpOnCondition(is_minus || is_zero, arg);
            break;
        }

        case OPCODE_JNZ:
            TryJumpOnCondition(!(regs[FLAGS] & FLAG_ZERO), arg);
            break;

        case OPCODE_ADD:
        case OPCODE_SUB:
        case OPCODE_XOR:
        case OPCODE_OR:
        case OPCODE_SHL:
        case OPCODE_SHR:
        case OPCODE_AND: {
            u16 operand;
            int ip_step;

            if ( instruction & FLAG_D ) {
                if ( instruction & FLAG_B ) {
                    if ( instruction & FLAG_C ) {
                        operand = mem[regs[r1]];
                    } else {
                        operand = *(u16 *)&mem[regs[r1]];
                    }
                } else {
                    operand = regs[r1];
                }
                ip_step = 2;
            } else if ( instruction & FLAG_A ) {
                operand = arg;
                ip_step = 4;
            } else {
                // TODO: clean up handling size
                if ( instruction & FLAG_B ) {
                    if ( instruction & FLAG_C ) { // byte
                        u16 address = mem[arg];
                        operand = mem[address];
                    } else { // word
                        u16 address = *(u16 *)&mem[arg];
                        operand = *(u16 *)&mem[address];
                    }
                } else {
                    if ( instruction & FLAG_C ) { // byte
                        operand = mem[arg];
                    } else { // word
                        operand = *(u16 *)&mem[arg];
                    }
                }
                ip_step = 4;
            }

            switch ( opcode ) {
                case OPCODE_ADD:
                    regs[r0] += operand;
                    break;
                case OPCODE_SUB:
                    regs[r0] -= operand;
                    break;
                case OPCODE_AND:
                    regs[r0] &= operand;
                    break;
                case OPCODE_OR:
                    regs[r0] |= operand;
                    break;
                case OPCODE_XOR:
                    regs[r0] ^= operand;
                    break;
                case OPCODE_SHL:
                    regs[r0] <<= operand;
                    break;
                case OPCODE_SHR:
                    regs[r0] >>= operand;
                    break;
                default:
                    FatalError("programmer forgot to add a switch case!");
                    break;
            }

            SetFlagsSignBit(r0, instruction & FLAG_C ? 0x80 : 0x8000);
            regs[IP] += ip_step;
            break;
        }

        case OPCODE_LEA:
            regs[r0] = arg;
            regs[IP] += 4;
            break;

        case OPCODE_LOAD: {
            int size = instruction & FLAG_C ? 1 : 2;
            if ( instruction & FLAG_A ) { // immediate value
                regs[r0] = arg;
            } else if ( instruction & FLAG_D ) { // address in r1
                memcpy(&regs[r0], &mem[regs[r1]], size);
            } else if ( instruction & FLAG_B ) { // indirect
                memcpy(&regs[r0], &mem[mem[arg]], size);
            } else {
                memcpy(&regs[r0], &mem[arg], size);
            }

            SetFlags(regs[r0], size);
            regs[IP] += 4;
            break;
        }

        case OPCODE_LOOP:
            TryJumpOnCondition(--regs[C] != 0, arg);
            break;

        case OPCODE_NOP:
            regs[IP] += 2;
            break;

        case OPCODE_NOT:
            if ( instruction & FLAG_A ) {
                regs[r0] = ~regs[r0];
                regs[IP] += 2;
            } else {
                u8 * location;
                if ( instruction & FLAG_B ) {
                    location = &mem[mem[arg]]; // indirect
                } else {
                    location = &mem[arg]; // direct
                }

                if ( instruction & FLAG_C ) {
                    *location = ~(*location);
                } else {
                    u16 * word = (u16 *)location;
                    *word = ~(*word);
                }
                regs[IP] += 4;
            }
            break;

        case OPCODE_PUSH:
            Push(regs[r0]);
//            PrintMemory(ADDR_STACK, ADDR_STACK + 8);
            regs[IP] += 2;
            break;

        case OPCODE_PUSHF:
            memcpy(&mem[regs[SP]], &regs[FLAGS], sizeof(regs[0]));
            regs[SP] += 2;
            regs[IP] += 2;
//            PrintMemory(ADDR_STACK, ADDR_STACK + 8);
            break;

        case OPCODE_POP:
            regs[r0] = Pop();
//            PrintMemory(ADDR_STACK, ADDR_STACK + 8);
            regs[IP] += 2;
            break;

        case OPCODE_POPF:
            regs[SP] -= 2;
            memcpy(&regs[FLAGS], &mem[regs[SP]], 2);
//            PrintMemory(ADDR_STACK, ADDR_STACK + 8);
            regs[IP] += 2;
            break;

        case OPCODE_STOP:
            running = false;
            break;

        case OPCODE_JUMP:
            regs[IP] = arg;
            break;

        case OPCODE_MOVE:
            regs[r1] = regs[r0];
            SetFlags(regs[r1], 2);
            regs[IP] += 2;
            break;

        case OPCODE_RETURN:
            regs[IP] = PopCallStack();
            break;

        case OPCODE_STORE:
            if ( instruction & FLAG_B ) {
                if ( instruction & FLAG_C ) { // store byte
                    mem[mem[arg]] = (u8)regs[r1];
                } else {
                    u16 address = *(u16 *)&mem[arg];
                    memcpy(&mem[address], &regs[r1], 2);
                }
            } else {
                if ( instruction & FLAG_C ) {
                    mem[arg] = (u8)regs[r1];
                } else {
                    memcpy(&mem[arg], &regs[r1], 2);
                }
            }
            regs[IP] += 4;
            break;

        default:
            FatalError("unknown opcode (%d)", opcode);
            break;
    }
}

SDL_Color Lighten(SDL_Color color, int amount)
{
    SDL_Color new_color = color;
    new_color.r += amount;
    new_color.g += amount;
    new_color.b += amount;
    SDL_clamp(new_color.r, 0, 255);
    SDL_clamp(new_color.g, 0, 255);
    SDL_clamp(new_color.b, 0, 255);

    return new_color;
}

void SetColor(SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
}

void RenderScreen(void)
{
    SDL_Rect src = { .x = 0, .y = 0 };

    const SDL_Rect dst = {
        .x = border_size,
        .y = border_size,
        .w = TEXTURE_W,
        .h = TEXTURE_H
    };

    if ( IsTextMode() ) {
        src.w = COLS * FONT_W;
        src.h = 200;
        RefreshText();
    } else {
        src.w = RES_W;
        src.h = RES_H;
        RefreshVideo();
    }

    SetColor( pal[mem[ADDR_BORDER_COLOR]] );
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, screen.texture, &src, &dst);
//    SDL_RenderCopy(renderer, scanlines, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void Run(StartupOptions options)
{
    running = true;
    executing = true;
    border_size = options.border_size;

    if ( options.program_file ) {
        u16 entry = LoadExecutableFromPath(options.program_file, mem);
        PushCallStack(entry); // Return here after startup code runs.
    }

    // Load character maps into memory

    memcpy(&mem[ADDR_CHAR_MAPS], font, (FONT_W * FONT_H) / 8);
    FONT_W = 8;
    FONT_H = 8;

    // Set up pointer registers.

    if ( options.no_startup ) {
        regs[IP] = PopCallStack();
    } else {
        extern const unsigned char startup_code[];
        regs[IP] = LoadExecutableFromBuffer(startup_code, mem);
    }

    regs[SP] = ADDR_STACK;

    // Set up I/O

    if ( !InitScreen(options.fullscreen) ) {
        running = false;
    }
    key_state = SDL_GetKeyboardState(NULL);

    // Init timing.

    double instruction_hz = (double)CLOCK_SPEED_HZ / (double)CYCLES_PER_INSTRUCTION;
    double frame_time = SecondsToNanoseconds(1.0 / instruction_hz);
    printf("4.77 MHz instruction time: %lf ns\n", frame_time);

    int refresh_msec = 0;
    int input_msec = 0;
    int frame = 0;

    u64 frequency = SDL_GetPerformanceFrequency();
    u64 last = SDL_GetPerformanceCounter();
    double cycle_length_sec = 1.0 / (instruction_hz);
    printf("cycle len: %.50lf sec\n", cycle_length_sec);
    printf("cycle len: %.50lf nsec\n", SecondsToNanoseconds(cycle_length_sec));

    SDL_PumpEvents();

    //double frame_lengths[100];

    while ( running ) {

        u64 now = SDL_GetPerformanceCounter();
        double dt = (double)(now - last) / (double)frequency;
        if ( dt < cycle_length_sec ) {
            continue;
        }
        last = now;

        // DEBUG: check frame length
#if 0
        if ( frame <= 99 ) {
            frame_lengths[frame] = dt * NSEC_PER_SEC;
        } else {
            running = false;
        }
#endif

        int now_msec = SDL_GetTicks();
        frame++;

        if ( SDL_GetTicks() % 600 < 300 ) {
            blink = 0;
        } else {
            blink = 1;
        }

        if ( now_msec >= input_msec ) {
            input_msec = now_msec + 10;

            SDL_Event event;
            while ( SDL_PollEvent(&event) ) {
                switch ( event.type ) {
                    case SDL_QUIT:
                        running = false;
                        break;
                    case SDL_KEYDOWN:
                        if ( wait_for_key ) {
                            regs[X] = event.key.keysym.sym;
                            regs[Y] = event.key.keysym.scancode;
                            wait_for_key = false;
                        } else {
                            EnqueueKey(event.key.keysym);
                        }
                        break;
                    default:
                        break;
                }
            }
        }

        if ( executing && !wait_for_refresh && !wait_for_key ) {
            ProcessInstruction();
        }

        if ( now_msec >= refresh_msec ) {
            wait_for_refresh = false;
            refresh_msec = now_msec + (1000 / 50);
            RenderScreen();
        }
    }

    // DEBUG:
//    for ( int i = 0; i < 100; i++ ) {
//        printf("frame %d: %lf ns\n", i, frame_lengths[i]);
//    }
}
