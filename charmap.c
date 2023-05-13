//
//  charmap.c
//  zi23
//
//  Created by Thomas Foster on 2/8/23.
//

#include "common.h"

u8 character_map[256 * 8] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x7e, 0x81, 0xa5, 0x81, 0xbd, 0x99, 0x81, 0x7e,
    0x7e, 0xff, 0xdb, 0xff, 0xc3, 0xe7, 0xff, 0x7e,
    0x6c, 0xfe, 0xfe, 0xfe, 0x7c, 0x38, 0x10, 0x00,
    0x10, 0x38, 0x7c, 0xfe, 0x7c, 0x38, 0x10, 0x00,
    0x3c, 0x3c, 0x18, 0xff, 0xe7, 0x18, 0x3c, 0x00,
    0x10, 0x38, 0x7c, 0xfe, 0xee, 0x10, 0x38, 0x00,
    0x00, 0x00, 0x18, 0x3c, 0x3c, 0x18, 0x00, 0x00,
    0xff, 0xff, 0xe7, 0xc3, 0xc3, 0xe7, 0xff, 0xff,
    0x00, 0x3c, 0x66, 0x42, 0x42, 0x66, 0x3c, 0x00,
    0xff, 0xc3, 0x99, 0xbd, 0xbd, 0x99, 0xc3, 0xff,
    0x1e, 0x06, 0x0a, 0x78, 0xcc, 0xcc, 0x78, 0x00,
    0x3c, 0x42, 0x42, 0x42, 0x3c, 0x18, 0x7e, 0x18,
    0x08, 0x0c, 0x0a, 0x0a, 0x08, 0x78, 0xf0, 0x00,
    0x18, 0x14, 0x1a, 0x16, 0x72, 0xe2, 0x0e, 0x1c,
    0x10, 0x54, 0x38, 0xee, 0x38, 0x54, 0x10, 0x00,
    0x80, 0xe0, 0xf8, 0xfe, 0xf8, 0xe0, 0x80, 0x00,
    0x02, 0x0e, 0x3e, 0xfe, 0x3e, 0x0e, 0x02, 0x00,
    0x18, 0x3c, 0x5a, 0x18, 0x5a, 0x3c, 0x18, 0x00,
    0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x66, 0x00,
    0x7f, 0xdb, 0xdb, 0xdb, 0x7b, 0x1b, 0x1b, 0x00,
    0x1c, 0x22, 0x38, 0x44, 0x44, 0x38, 0x88, 0x70,
    0x00, 0x00, 0x00, 0x00, 0x7e, 0x7e, 0x7e, 0x00,
    0x18, 0x3c, 0x5a, 0x18, 0x5a, 0x3c, 0x18, 0x7e,
    0x18, 0x3c, 0x5a, 0x18, 0x18, 0x18, 0x18, 0x00,
    0x18, 0x18, 0x18, 0x18, 0x5a, 0x3c, 0x18, 0x00,
    0x00, 0x18, 0x0c, 0xfe, 0x0c, 0x18, 0x00, 0x00,
    0x00, 0x30, 0x60, 0xfe, 0x60, 0x30, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xff, 0x00,
    0x00, 0x24, 0x42, 0xff, 0x42, 0x24, 0x00, 0x00,
    0x00, 0x10, 0x38, 0x7c, 0xfe, 0xfe, 0x00, 0x00,
    0x00, 0xfe, 0xfe, 0x7c, 0x38, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x30, 0x78, 0x78, 0x30, 0x30, 0x00, 0x30, 0x00,
    0x6c, 0x24, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x6c, 0x6c, 0xfe, 0x6c, 0xfe, 0x6c, 0x6c, 0x00,
    0x10, 0x7c, 0xd0, 0x7c, 0x16, 0xfc, 0x10, 0x00,
    0x00, 0x66, 0xac, 0xd8, 0x36, 0x6a, 0xcc, 0x00,
    0x38, 0x4c, 0x38, 0x78, 0xce, 0xcc, 0x7a, 0x00,
    0x30, 0x10, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x18, 0x30, 0x60, 0x60, 0x60, 0x30, 0x18, 0x00,
    0x60, 0x30, 0x18, 0x18, 0x18, 0x30, 0x60, 0x00,
    0x00, 0x66, 0x3c, 0xff, 0x3c, 0x66, 0x00, 0x00,
    0x00, 0x30, 0x30, 0xfc, 0x30, 0x30, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x10, 0x20,
    0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00,
    0x02, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0x00,
    0x7c, 0xce, 0xde, 0xf6, 0xe6, 0xe6, 0x7c, 0x00,
    0x18, 0x38, 0x78, 0x18, 0x18, 0x18, 0x7e, 0x00,
    0x7c, 0xc6, 0x06, 0x1c, 0x70, 0xc0, 0xfe, 0x00,
    0x7c, 0xc6, 0x06, 0x3c, 0x06, 0xc6, 0x7c, 0x00,
    0x1c, 0x3c, 0x6c, 0xcc, 0xfe, 0x0c, 0x1e, 0x00,
    0xfe, 0xc0, 0xfc, 0x06, 0x06, 0xc6, 0x7c, 0x00,
    0x7c, 0xc6, 0xc0, 0xfc, 0xc6, 0xc6, 0x7c, 0x00,
    0xfe, 0xc6, 0x86, 0x0c, 0x18, 0x30, 0x30, 0x00,
    0x7c, 0xc6, 0xc6, 0x7c, 0xc6, 0xc6, 0x7c, 0x00,
    0x7c, 0xc6, 0xc6, 0x7e, 0x06, 0xc6, 0x7c, 0x00,
    0x00, 0x30, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00,
    0x00, 0x30, 0x00, 0x00, 0x00, 0x30, 0x10, 0x20,
    0x0c, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0c, 0x00,
    0x00, 0x00, 0x7e, 0x00, 0x00, 0x7e, 0x00, 0x00,
    0x60, 0x30, 0x18, 0x0c, 0x18, 0x30, 0x60, 0x00,
    0x78, 0xcc, 0x0c, 0x18, 0x30, 0x00, 0x30, 0x00,
    0x7c, 0x82, 0x9e, 0xa6, 0x9e, 0x80, 0x7c, 0x00,
    0x7c, 0xc6, 0xc6, 0xfe, 0xc6, 0xc6, 0xc6, 0x00,
    0xfc, 0x66, 0x66, 0x7c, 0x66, 0x66, 0xfc, 0x00,
    0x7c, 0xc6, 0xc0, 0xc0, 0xc0, 0xc6, 0x7c, 0x00,
    0xfc, 0x66, 0x66, 0x66, 0x66, 0x66, 0xfc, 0x00,
    0xfe, 0x62, 0x68, 0x78, 0x68, 0x62, 0xfe, 0x00,
    0xfe, 0x62, 0x68, 0x78, 0x68, 0x60, 0xf0, 0x00,
    0x7c, 0xc6, 0xc6, 0xc0, 0xce, 0xc6, 0x7e, 0x00,
    0xc6, 0xc6, 0xc6, 0xfe, 0xc6, 0xc6, 0xc6, 0x00,
    0x3c, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00,
    0x1e, 0x0c, 0x0c, 0x0c, 0xcc, 0xcc, 0x78, 0x00,
    0xe6, 0x66, 0x64, 0x78, 0x6c, 0x66, 0xe6, 0x00,
    0xf0, 0x60, 0x60, 0x60, 0x62, 0x66, 0xfe, 0x00,
    0x82, 0xc6, 0xee, 0xfe, 0xd6, 0xc6, 0xc6, 0x00,
    0xc6, 0xe6, 0xf6, 0xde, 0xce, 0xc6, 0xc6, 0x00,
    0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00,
    0xfc, 0x66, 0x66, 0x7c, 0x60, 0x60, 0xf0, 0x00,
    0x7c, 0xc6, 0xc6, 0xc6, 0xd6, 0xde, 0x7c, 0x06,
    0xfc, 0x66, 0x66, 0x7c, 0x66, 0x66, 0xe6, 0x00,
    0x7c, 0xc6, 0xc0, 0x7c, 0x06, 0xc6, 0x7c, 0x00,
    0x7e, 0x5a, 0x5a, 0x18, 0x18, 0x18, 0x3c, 0x00,
    0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00,
    0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x6c, 0x38, 0x00,
    0xc6, 0xc6, 0xd6, 0xfe, 0xee, 0xc6, 0x82, 0x00,
    0xc6, 0x6c, 0x38, 0x38, 0x38, 0x6c, 0xc6, 0x00,
    0x66, 0x66, 0x66, 0x3c, 0x18, 0x18, 0x3c, 0x00,
    0xfe, 0xc6, 0x8c, 0x18, 0x32, 0x66, 0xfe, 0x00,
    0x78, 0x60, 0x60, 0x60, 0x60, 0x60, 0x78, 0x00,
    0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x02, 0x00,
    0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78, 0x00,
    0x10, 0x38, 0x6c, 0xc6, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
    0x30, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x78, 0x0c, 0x7c, 0xcc, 0x76, 0x00,
    0xe0, 0x60, 0x60, 0x7c, 0x66, 0x66, 0x7c, 0x00,
    0x00, 0x00, 0x7c, 0xc6, 0xc0, 0xc6, 0x7c, 0x00,
    0x1c, 0x0c, 0x0c, 0x7c, 0xcc, 0xcc, 0x76, 0x00,
    0x00, 0x00, 0x7c, 0xc6, 0xfe, 0xc0, 0x7c, 0x00,
    0x1c, 0x36, 0x30, 0x78, 0x30, 0x30, 0x78, 0x00,
    0x00, 0x00, 0x76, 0xcc, 0xcc, 0x7c, 0x0c, 0x78,
    0xe0, 0x60, 0x6c, 0x76, 0x66, 0x66, 0xe6, 0x00,
    0x18, 0x00, 0x38, 0x18, 0x18, 0x18, 0x3c, 0x00,
    0x00, 0x0c, 0x00, 0x1c, 0x0c, 0x0c, 0xcc, 0x78,
    0xe0, 0x60, 0x66, 0x6c, 0x78, 0x6c, 0xe6, 0x00,
    0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00,
    0x00, 0x00, 0xcc, 0xfe, 0xd6, 0xd6, 0xd6, 0x00,
    0x00, 0x00, 0xdc, 0x66, 0x66, 0x66, 0x66, 0x00,
    0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0x00,
    0x00, 0x00, 0xdc, 0x66, 0x66, 0x7c, 0x60, 0xf0,
    0x00, 0x00, 0x7c, 0xcc, 0xcc, 0x7c, 0x0c, 0x1e,
    0x00, 0x00, 0xde, 0x76, 0x60, 0x60, 0xf0, 0x00,
    0x00, 0x00, 0x7c, 0xc0, 0x7c, 0x06, 0x7c, 0x00,
    0x10, 0x30, 0xfc, 0x30, 0x30, 0x34, 0x18, 0x00,
    0x00, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0x76, 0x00,
    0x00, 0x00, 0xc6, 0xc6, 0xc6, 0x6c, 0x38, 0x00,
    0x00, 0x00, 0xc6, 0xd6, 0xd6, 0xfe, 0x6c, 0x00,
    0x00, 0x00, 0xc6, 0x6c, 0x38, 0x6c, 0xc6, 0x00,
    0x00, 0x00, 0xc6, 0xc6, 0xc6, 0x7e, 0x06, 0x7c,
    0x00, 0x00, 0xfc, 0x98, 0x30, 0x64, 0xfc, 0x00,
    0x0e, 0x18, 0x18, 0x30, 0x18, 0x18, 0x0e, 0x00,
    0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00,
    0xe0, 0x30, 0x30, 0x18, 0x30, 0x30, 0xe0, 0x00,
    0x76, 0xdc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x10, 0x38, 0x6c, 0xc6, 0xc6, 0xfe, 0x00,
    0x7c, 0xc6, 0xc0, 0xc0, 0xc6, 0x7c, 0x18, 0x70,
    0xcc, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0x76, 0x00,
    0x0e, 0x10, 0x7c, 0xc6, 0xfe, 0xc0, 0x7c, 0x00,
    0x7c, 0x82, 0x38, 0x0c, 0x7c, 0xcc, 0x76, 0x00,
    0xcc, 0x00, 0x78, 0x0c, 0x7c, 0xcc, 0x76, 0x00,
    0xe0, 0x10, 0x78, 0x0c, 0x7c, 0xcc, 0x76, 0x00,
    0x30, 0x30, 0x78, 0x0c, 0x7c, 0xcc, 0x76, 0x00,
    0x00, 0x00, 0x7c, 0xc0, 0xc0, 0x7c, 0x18, 0x70,
    0x7c, 0x82, 0x7c, 0xc6, 0xfe, 0xc0, 0x7c, 0x00,
    0xc6, 0x00, 0x7c, 0xc6, 0xfe, 0xc0, 0x7c, 0x00,
    0xe0, 0x10, 0x7c, 0xc6, 0xfe, 0xc0, 0x7c, 0x00,
    0x66, 0x00, 0x38, 0x18, 0x18, 0x18, 0x3c, 0x00,
    0x7c, 0x82, 0x38, 0x18, 0x18, 0x18, 0x3c, 0x00,
    0xe0, 0x10, 0x38, 0x18, 0x18, 0x18, 0x3c, 0x00,
    0xc6, 0x00, 0x7c, 0xc6, 0xfe, 0xc6, 0xc6, 0x00,
    0x38, 0x38, 0x7c, 0xc6, 0xfe, 0xc6, 0xc6, 0x00,
    0x0e, 0x10, 0xfe, 0x60, 0x78, 0x60, 0xfe, 0x00,
    0x00, 0x00, 0x7c, 0x12, 0x7e, 0xd0, 0x7e, 0x00,
    0x7e, 0xc8, 0xc8, 0xfe, 0xc8, 0xc8, 0xce, 0x00,
    0x7c, 0x82, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0x00,
    0xc6, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0x00,
    0xe0, 0x10, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0x00,
    0x7c, 0x82, 0xcc, 0xcc, 0xcc, 0xcc, 0x76, 0x00,
    0xe0, 0x10, 0xcc, 0xcc, 0xcc, 0xcc, 0x76, 0x00,
    0xc6, 0x00, 0xc6, 0xc6, 0xc6, 0x7e, 0x06, 0x7c,
    0xc6, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00,
    0xc6, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00,
    0x18, 0x7c, 0xd6, 0xd0, 0xd6, 0x7c, 0x18, 0x00,
    0x38, 0x6c, 0x60, 0xf0, 0x60, 0xf2, 0xdc, 0x00,
    0x66, 0x3c, 0x18, 0x7e, 0x18, 0x7e, 0x18, 0x00,
    0xf8, 0xcc, 0xf8, 0xc4, 0xcc, 0xde, 0xcc, 0x06,
    0x0e, 0x1b, 0x18, 0x3c, 0x18, 0x18, 0xd8, 0x70,
    0x0e, 0x10, 0x78, 0x0c, 0x7c, 0xcc, 0x76, 0x00,
    0x0e, 0x10, 0x38, 0x18, 0x18, 0x18, 0x3c, 0x00,
    0x0e, 0x10, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0x00,
    0x0e, 0x10, 0xcc, 0xcc, 0xcc, 0xcc, 0x76, 0x00,
    0x66, 0x98, 0xdc, 0x66, 0x66, 0x66, 0x66, 0x00,
    0x66, 0x98, 0xe6, 0xf6, 0xde, 0xce, 0xc6, 0x00,
    0x38, 0x0c, 0x3c, 0x34, 0x00, 0x7e, 0x00, 0x00,
    0x38, 0x6c, 0x6c, 0x38, 0x00, 0x7c, 0x00, 0x00,
    0x30, 0x00, 0x30, 0x60, 0xc6, 0xc6, 0x7c, 0x00,
    0x00, 0x00, 0xfe, 0xc0, 0xc0, 0xc0, 0x00, 0x00,
    0x00, 0x00, 0xfe, 0x06, 0x06, 0x06, 0x00, 0x00,
    0xc0, 0xc8, 0xd0, 0xfe, 0x46, 0x8c, 0x1e, 0x00,
    0xc0, 0xc8, 0xd0, 0xec, 0x5c, 0xbe, 0x0c, 0x00,
    0x18, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00,
    0x00, 0x36, 0x6c, 0xd8, 0x6c, 0x36, 0x00, 0x00,
    0x00, 0xd8, 0x6c, 0x36, 0x6c, 0xd8, 0x00, 0x00,
    0x00, 0x55, 0x00, 0x55, 0x00, 0x55, 0x00, 0x55,
    0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55,
    0xff, 0x55, 0xff, 0x55, 0xff, 0x55, 0xff, 0x55,
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18, 0xf8, 0x18, 0x18, 0x18,
    0x18, 0x18, 0xf8, 0x18, 0xf8, 0x18, 0x18, 0x18,
    0x36, 0x36, 0x36, 0x36, 0xf6, 0x36, 0x36, 0x36,
    0x00, 0x00, 0x00, 0x00, 0xfe, 0x36, 0x36, 0x36,
    0x00, 0x00, 0xf8, 0x18, 0xf8, 0x18, 0x18, 0x18,
    0x36, 0x36, 0xf6, 0x06, 0xf6, 0x36, 0x36, 0x36,
    0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
    0x00, 0x00, 0xfe, 0x06, 0xf6, 0x36, 0x36, 0x36,
    0x36, 0x36, 0xf6, 0x06, 0xfe, 0x00, 0x00, 0x00,
    0x36, 0x36, 0x36, 0x36, 0xfe, 0x00, 0x00, 0x00,
    0x18, 0x18, 0xf8, 0x18, 0xf8, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xf8, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18, 0x1f, 0x00, 0x00, 0x00,
    0x18, 0x18, 0x18, 0x18, 0xff, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18, 0x1f, 0x18, 0x18, 0x18,
    0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00,
    0x18, 0x18, 0x18, 0x18, 0xff, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x1f, 0x18, 0x1f, 0x18, 0x18, 0x18,
    0x36, 0x36, 0x36, 0x36, 0x37, 0x36, 0x36, 0x36,
    0x36, 0x36, 0x37, 0x30, 0x3f, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x3f, 0x30, 0x37, 0x36, 0x36, 0x36,
    0x36, 0x36, 0xf7, 0x00, 0xff, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xff, 0x00, 0xf7, 0x36, 0x36, 0x36,
    0x36, 0x36, 0x37, 0x30, 0x37, 0x36, 0x36, 0x36,
    0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00,
    0x36, 0x36, 0xf7, 0x00, 0xf7, 0x36, 0x36, 0x36,
    0x18, 0x18, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00,
    0x36, 0x36, 0x36, 0x36, 0xff, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xff, 0x00, 0xff, 0x18, 0x18, 0x18,
    0x00, 0x00, 0x00, 0x00, 0xff, 0x36, 0x36, 0x36,
    0x36, 0x36, 0x36, 0x36, 0x3f, 0x00, 0x00, 0x00,
    0x18, 0x18, 0x1f, 0x18, 0x1f, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x1f, 0x18, 0x1f, 0x18, 0x18, 0x18,
    0x00, 0x00, 0x00, 0x00, 0x3f, 0x36, 0x36, 0x36,
    0x36, 0x36, 0x36, 0x36, 0xff, 0x36, 0x36, 0x36,
    0x18, 0x18, 0xff, 0x18, 0xff, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18, 0xf8, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x1f, 0x18, 0x18, 0x18,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
    0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,
    0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x74, 0xcc, 0xc8, 0xdc, 0x76, 0x00,
    0x78, 0xcc, 0xd8, 0xcc, 0xc6, 0xc6, 0xdc, 0x40,
    0xfe, 0x62, 0x60, 0x60, 0x60, 0x60, 0xf0, 0x00,
    0x00, 0x02, 0x7e, 0xec, 0x6c, 0x6c, 0x48, 0x00,
    0xfe, 0x62, 0x30, 0x18, 0x30, 0x62, 0xfe, 0x00,
    0x00, 0x00, 0x7e, 0xd0, 0xc8, 0xc8, 0x70, 0x00,
    0x00, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0xf8, 0x80,
    0x00, 0x00, 0x7e, 0xd8, 0x18, 0x18, 0x10, 0x00,
    0x38, 0x10, 0x7c, 0xd6, 0xd6, 0x7c, 0x10, 0x38,
    0x7c, 0xc6, 0xc6, 0xfe, 0xc6, 0xc6, 0x7c, 0x00,
    0x7c, 0xc6, 0xc6, 0xc6, 0x6c, 0x28, 0xee, 0x00,
    0x3c, 0x22, 0x18, 0x7c, 0xcc, 0xcc, 0x78, 0x00,
    0x00, 0x00, 0x66, 0x99, 0x99, 0x66, 0x00, 0x00,
    0x00, 0x06, 0x7c, 0x9e, 0xf2, 0x7c, 0xc0, 0x00,
    0x00, 0x00, 0x7c, 0xc0, 0xf8, 0xc0, 0x7c, 0x00,
    0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x00,
    0x00, 0xfe, 0x00, 0xfe, 0x00, 0xfe, 0x00, 0x00,
    0x18, 0x18, 0x7e, 0x18, 0x18, 0x00, 0x7e, 0x00,
    0x30, 0x18, 0x0c, 0x18, 0x30, 0x00, 0x7c, 0x00,
    0x18, 0x30, 0x60, 0x30, 0x18, 0x00, 0x7c, 0x00,
    0x0e, 0x1b, 0x1b, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18, 0x18, 0xd8, 0xd8, 0x70,
    0x00, 0x18, 0x00, 0x7e, 0x00, 0x18, 0x00, 0x00,
    0x00, 0x76, 0xdc, 0x00, 0x76, 0xdc, 0x00, 0x00,
    0x38, 0x6c, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x0f, 0x0c, 0x0c, 0x0c, 0xec, 0x6c, 0x3c, 0x00,
    0xd8, 0x6c, 0x6c, 0x6c, 0x00, 0x00, 0x00, 0x00,
    0xf0, 0x30, 0xc0, 0xf0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

/*
 https://github.com/chocolate-doom/chocolate-doom/tree/master/textscreen/fonts

 Atari 4x8 font:
 Copyright (c) 1999, Thomas A. Fine

    License to copy, modify, and distribute for both commercial and
    non-commercial use is herby granted, provided this notice
    is preserved.

    Email to my last name at head.cfa.harvard.edu
    http://hea-www.harvard.edu/~fine/

 All font modifications and enhancements are:

   Copyright (C) 2005-2016 Simon Howard

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
 */

#define BYTES_PER_CHAR 4

u8 atari[BYTES_PER_CHAR * 256] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x72, 0x00,
    0x6f, 0xaf, 0x8d, 0xf6, 0x00, 0x57, 0x20, 0x00,
    0x00, 0x27, 0x20, 0x00, 0x02, 0x55, 0x27, 0x00,
    0x02, 0x77, 0x27, 0x00, 0x00, 0x06, 0x60, 0x00,
    0xff, 0xf9, 0x9f, 0xff, 0x00, 0x69, 0x96, 0x00,
    0xff, 0x96, 0x69, 0xff, 0x07, 0x2e, 0xae, 0x00,
    0x06, 0x96, 0xf6, 0x60, 0x03, 0x22, 0x2e, 0xc0,
    0x07, 0x55, 0x5d, 0xc0, 0x44, 0xe4, 0xe4, 0x44,
    0x08, 0xce, 0xc8, 0x00, 0x01, 0x37, 0x31, 0x00,
    0x02, 0x72, 0x27, 0x20, 0x05, 0x55, 0x05, 0x00,
    0x0f, 0x9d, 0x55, 0x00, 0x68, 0x69, 0x61, 0x60,
    0x00, 0x00, 0xff, 0x00, 0x02, 0x72, 0x27, 0x2f,
    0x02, 0x72, 0x22, 0x20, 0x02, 0x22, 0x27, 0x20,
    0x04, 0x2f, 0x24, 0x00, 0x02, 0x4f, 0x42, 0x00,
    0x00, 0x08, 0xe0, 0x00, 0x00, 0xaf, 0xa0, 0x00,
    0x00, 0x4e, 0xf0, 0x00, 0x00, 0xfe, 0x40, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x04, 0x44, 0x40, 0x40,
    0x0a, 0xa0, 0x00, 0x00, 0x0a, 0xea, 0xae, 0xa0,
    0x44, 0xa4, 0x2a, 0x44, 0x0a, 0x24, 0x48, 0xa0,
    0x4a, 0x4a, 0xac, 0x60, 0x04, 0x40, 0x00, 0x00,
    0x02, 0x44, 0x44, 0x20, 0x08, 0x44, 0x44, 0x80,
    0x0a, 0x4e, 0x4a, 0x00, 0x04, 0x4e, 0x44, 0x00,
    0x00, 0x00, 0x04, 0x48, 0x00, 0x0e, 0x00, 0x00,
    0x00, 0x00, 0x04, 0x40, 0x02, 0x24, 0x48, 0x80,
    0x04, 0xae, 0xaa, 0x40, 0x04, 0xc4, 0x44, 0xe0,
    0x04, 0xa2, 0x48, 0xe0, 0x0e, 0x24, 0x2a, 0x40,
    0x02, 0x6a, 0xe2, 0x20, 0x0e, 0x8c, 0x2a, 0x40,
    0x06, 0x8c, 0xaa, 0x40, 0x0e, 0x22, 0x44, 0x40,
    0x04, 0xa4, 0xaa, 0x40, 0x04, 0xaa, 0x62, 0xc0,
    0x00, 0x40, 0x04, 0x00, 0x00, 0x40, 0x04, 0x80,
    0x00, 0x24, 0x84, 0x20, 0x00, 0xe0, 0xe0, 0x00,
    0x00, 0x84, 0x24, 0x80, 0x04, 0xa2, 0x40, 0x40,
    0x04, 0xaa, 0x88, 0x60, 0x04, 0xaa, 0xea, 0xa0,
    0x0c, 0xac, 0xaa, 0xc0, 0x04, 0xa8, 0x8a, 0x40,
    0x0c, 0xaa, 0xaa, 0xc0, 0x0e, 0x8e, 0x88, 0xe0,
    0x0e, 0x8e, 0x88, 0x80, 0x04, 0xa8, 0xaa, 0x40,
    0x0a, 0xae, 0xaa, 0xa0, 0x0e, 0x44, 0x44, 0xe0,
    0x02, 0x22, 0x2a, 0x40, 0x0a, 0xac, 0xaa, 0xa0,
    0x08, 0x88, 0x88, 0xe0, 0x0a, 0xea, 0xaa, 0xa0,
    0x02, 0xae, 0xea, 0x80, 0x04, 0xaa, 0xaa, 0x40,
    0x0c, 0xaa, 0xc8, 0x80, 0x04, 0xaa, 0xac, 0x60,
    0x0c, 0xaa, 0xca, 0xa0, 0x04, 0xa4, 0x2a, 0x40,
    0x0e, 0x44, 0x44, 0x40, 0x0a, 0xaa, 0xaa, 0xe0,
    0x0a, 0xaa, 0xaa, 0x40, 0x0a, 0xaa, 0xae, 0xa0,
    0x0a, 0xa4, 0xaa, 0xa0, 0x0a, 0xa4, 0x44, 0x40,
    0x0e, 0x24, 0x48, 0xe0, 0x06, 0x44, 0x44, 0x60,
    0x08, 0x84, 0x42, 0x20, 0x0c, 0x44, 0x44, 0xc0,
    0x04, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f,
    0x08, 0x40, 0x00, 0x00, 0x00, 0xc2, 0x6a, 0x60,
    0x08, 0x8c, 0xaa, 0xc0, 0x00, 0x68, 0x88, 0x60,
    0x02, 0x26, 0xaa, 0x60, 0x00, 0x4a, 0xe8, 0x60,
    0x02, 0x4e, 0x44, 0x40, 0x00, 0x6a, 0xa6, 0x2c,
    0x08, 0x8c, 0xaa, 0xa0, 0x04, 0x0c, 0x44, 0xe0,
    0x02, 0x06, 0x22, 0x2c, 0x08, 0x8a, 0xca, 0xa0,
    0x04, 0x44, 0x44, 0x40, 0x00, 0xae, 0xaa, 0xa0,
    0x00, 0xca, 0xaa, 0xa0, 0x00, 0x4a, 0xaa, 0x40,
    0x00, 0xca, 0xac, 0x88, 0x00, 0x6a, 0xa6, 0x22,
    0x00, 0x68, 0x88, 0x80, 0x00, 0x68, 0x42, 0xc0,
    0x04, 0x4e, 0x44, 0x40, 0x00, 0xaa, 0xaa, 0xe0,
    0x00, 0xaa, 0xaa, 0x40, 0x00, 0xaa, 0xae, 0xa0,
    0x00, 0xaa, 0x4a, 0xa0, 0x00, 0xaa, 0xa6, 0x2c,
    0x00, 0xe2, 0x48, 0xe0, 0x24, 0x48, 0x44, 0x20,
    0x44, 0x40, 0x44, 0x40, 0x84, 0x42, 0x44, 0x80,
    0x05, 0xa0, 0x00, 0x00, 0x04, 0xaa, 0xae, 0x00,
    0x04, 0xa8, 0x8a, 0x4c, 0xa0, 0xaa, 0xaa, 0xe0,
    0x48, 0x4a, 0xe8, 0x60, 0x25, 0xc2, 0x6a, 0x60,
    0xa0, 0xc2, 0x6a, 0x60, 0x42, 0xc2, 0x6a, 0x60,
    0x4a, 0x42, 0x6a, 0x60, 0x00, 0x68, 0x88, 0x6c,
    0x4a, 0x4a, 0xe8, 0x60, 0xa0, 0x4a, 0xe8, 0x60,
    0x42, 0x4a, 0xe8, 0x60, 0x0a, 0x0c, 0x44, 0xe0,
    0x4a, 0x0c, 0x44, 0xe0, 0x42, 0x0c, 0x44, 0xe0,
    0xa0, 0x4a, 0xea, 0xa0, 0x4a, 0x4a, 0xea, 0xa0,
    0x24, 0xe8, 0xc8, 0xe0, 0x00, 0xb5, 0x7a, 0x70,
    0x07, 0xaf, 0xaa, 0xb0, 0x4a, 0x04, 0xaa, 0x40,
    0xa0, 0x4a, 0xaa, 0x40, 0x42, 0x4a, 0xaa, 0x40,
    0x4a, 0x0a, 0xaa, 0xe0, 0x42, 0x0a, 0xaa, 0xe0,
    0xa0, 0xaa, 0xa6, 0x2c, 0xa0, 0x4a, 0xaa, 0x40,
    0xa0, 0xaa, 0xaa, 0xe0, 0x04, 0x68, 0x86, 0x40,
    0x34, 0x4e, 0x44, 0xf0, 0x0a, 0xa4, 0xe4, 0x40,
    0x0c, 0xac, 0xab, 0xa0, 0x12, 0x27, 0x2a, 0x40,
    0x24, 0xc2, 0x6a, 0x60, 0x24, 0x0c, 0x44, 0xe0,
    0x48, 0x4a, 0xaa, 0x40, 0x24, 0x0a, 0xaa, 0xe0,
    0x5a, 0x0c, 0xaa, 0xa0, 0x5a, 0x09, 0xdb, 0x90,
    0x6a, 0x70, 0xf0, 0x00, 0x4a, 0x40, 0xe0, 0x00,
    0x04, 0x04, 0x8a, 0x40, 0x00, 0x0e, 0x88, 0x00,
    0x00, 0x07, 0x11, 0x00, 0x88, 0xa4, 0xb1, 0x23,
    0x88, 0xa4, 0x85, 0x71, 0x04, 0x04, 0x44, 0x40,
    0x00, 0x05, 0xa5, 0x00, 0x00, 0x0a, 0x5a, 0x00,
    0x14, 0x14, 0x14, 0x14, 0x5a, 0x5a, 0x5a, 0x5a,
    0xd7, 0xd7, 0xd7, 0xd7, 0x22, 0x22, 0x22, 0x22,
    0x22, 0x2e, 0x22, 0x22, 0x22, 0xe2, 0xe2, 0x22,
    0x55, 0x5d, 0x55, 0x55, 0x00, 0x0f, 0x55, 0x55,
    0x00, 0xe2, 0xe2, 0x22, 0x55, 0xd1, 0xd5, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x00, 0xf1, 0xd5, 0x55,
    0x55, 0xd1, 0xf0, 0x00, 0x55, 0x5f, 0x00, 0x00,
    0x22, 0xe2, 0xe0, 0x00, 0x00, 0x0e, 0x22, 0x22,
    0x22, 0x23, 0x00, 0x00, 0x22, 0x2f, 0x00, 0x00,
    0x00, 0x0f, 0x22, 0x22, 0x22, 0x23, 0x22, 0x22,
    0x00, 0x0f, 0x00, 0x00, 0x22, 0x2f, 0x22, 0x22,
    0x22, 0x32, 0x32, 0x22, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x54, 0x70, 0x00, 0x00, 0x74, 0x55, 0x55,
    0x55, 0xd0, 0xf0, 0x00, 0x00, 0xf0, 0xd5, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x00, 0xf0, 0xf0, 0x00,
    0x55, 0xd0, 0xd5, 0x55, 0x22, 0xf0, 0xf0, 0x00,
    0x55, 0x5f, 0x00, 0x00, 0x00, 0xf0, 0xf2, 0x22,
    0x00, 0x0f, 0x55, 0x55, 0x55, 0x57, 0x00, 0x00,
    0x22, 0x32, 0x30, 0x00, 0x00, 0x32, 0x32, 0x22,
    0x00, 0x07, 0x55, 0x55, 0x55, 0x5f, 0x55, 0x55,
    0x22, 0xf2, 0xf2, 0x22, 0x22, 0x2e, 0x00, 0x00,
    0x00, 0x03, 0x22, 0x22, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0xff, 0xff, 0xcc, 0xcc, 0xcc, 0xcc,
    0x33, 0x33, 0x33, 0x33, 0xff, 0xff, 0x00, 0x00,
    0x00, 0x05, 0xaa, 0x50, 0x0c, 0xac, 0xa9, 0xa0,
    0x0f, 0x98, 0x88, 0x80, 0x00, 0xfa, 0xaa, 0xa0,
    0x00, 0xe8, 0x48, 0xe0, 0x00, 0x7a, 0xaa, 0x40,
    0x00, 0x55, 0x74, 0x80, 0x00, 0x5a, 0x22, 0x20,
    0x0e, 0x4a, 0xa4, 0xe0, 0x04, 0xae, 0xaa, 0x40,
    0x06, 0x99, 0x66, 0xf0, 0x06, 0x84, 0xaa, 0x40,
    0x00, 0x6b, 0xd6, 0x00, 0x01, 0xf9, 0x9f, 0x80,
    0x06, 0x8e, 0x88, 0x60, 0x04, 0xaa, 0xaa, 0xa0,
    0x0e, 0x0e, 0x0e, 0x00, 0x04, 0xe4, 0x0e, 0x00,
    0x08, 0x42, 0x48, 0xe0, 0x02, 0x48, 0x42, 0xe0,
    0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0xa4, 0x00,
    0x00, 0x40, 0xe0, 0x40, 0x05, 0xa0, 0x5a, 0x00,
    0x4a, 0x40, 0x00, 0x00, 0x00, 0x06, 0x60, 0x00,
    0x00, 0x00, 0x40, 0x00, 0x32, 0x2a, 0x62, 0x00,
    0xea, 0xaa, 0x00, 0x00, 0xc2, 0x4e, 0x00, 0x00,
    0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
};
