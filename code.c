//
//  code.c
//  zi23
//
//  Created by Thomas Foster on 2/21/23.
//

#include <stdio.h>

const unsigned char startup_code[] = {
    0x6C, 0x70, 0x02, 0x00, 0x2E, 0x01, 0x7A, 0x69,
    0x35, 0x0A, 0x00, 0x56, 0x69, 0x72, 0x74, 0x75,
    0x61, 0x6C, 0x20, 0x52, 0x65, 0x74, 0x72, 0x6F,
    0x20, 0x43, 0x6F, 0x6D, 0x70, 0x75, 0x74, 0x65,
    0x72, 0x0A, 0x0A, 0x00, 0x52, 0x41, 0x4D, 0x3A,
    0x20, 0x00, 0x47, 0x6F, 0x2D, 0x6C, 0x69, 0x6E,
    0x65, 0x20, 0x70, 0x61, 0x6C, 0x65, 0x74, 0x74,
    0x65, 0x20, 0x62, 0x79, 0x20, 0x5A, 0x61, 0x63,
    0x68, 0x69, 0x65, 0x20, 0x50, 0x68, 0x6F, 0x74,
    0x6F, 0x6E, 0x0A, 0x0A, 0x00, 0x7A, 0x69, 0x35,
    0x20, 0x43, 0x6F, 0x70, 0x79, 0x72, 0x69, 0x67,
    0x68, 0x74, 0x20, 0x28, 0x43, 0x29, 0x20, 0x32,
    0x30, 0x32, 0x33, 0x20, 0x54, 0x68, 0x6F, 0x6D,
    0x61, 0x73, 0x20, 0x46, 0x6F, 0x73, 0x74, 0x65,
    0x72, 0x00, 0x42, 0x81, 0x00, 0x00, 0x1F, 0x0B,
    0x44, 0x01, 0x00, 0x70, 0xC2, 0x80, 0x0A, 0x00,
    0x1F, 0x19, 0x82, 0x80, 0x16, 0x00, 0x14, 0x00,
    0x10, 0x71, 0x44, 0x01, 0x05, 0x70, 0x1F, 0x0A,
    0x44, 0x01, 0x1E, 0x70, 0x1F, 0x0A, 0x02, 0x80,
    0x00, 0x00, 0x1F, 0x13, 0x09, 0x0A, 0x1F, 0x12,
    0x42, 0x81, 0x4B, 0x00, 0x82, 0x80, 0x01, 0x00,
    0x1F, 0x09, 0x12, 0x80, 0x1F, 0x08, 0x16, 0x80,
    0x40, 0x00, 0x1B, 0x00, 0x96, 0x70, 0x02, 0x81,
    0x13, 0x00, 0x14, 0x00, 0xE6, 0x70, 0x02, 0x81,
    0x14, 0x00, 0x14, 0x00, 0xE6, 0x70, 0x82, 0x80,
    0x20, 0x00, 0x14, 0x00, 0x10, 0x71, 0x44, 0x01,
    0x24, 0x70, 0xC2, 0x80, 0x0D, 0x00, 0x1F, 0x19,
    0x44, 0x01, 0x47, 0x70, 0x1F, 0x0A, 0x42, 0x81,
    0xE8, 0x03, 0x1F, 0x14, 0x42, 0x81, 0x01, 0x00,
    0x1F, 0x0B, 0x15, 0x00, 0xC2, 0x80, 0x00, 0x00,
    0x1F, 0x08, 0xC2, 0x80, 0x00, 0x00, 0x42, 0x81,
    0xDB, 0x00, 0x82, 0x80, 0x02, 0x00, 0x1F, 0x0C,
    0xD2, 0x80, 0xD6, 0x80, 0x10, 0x00, 0x19, 0x00,
    0xF0, 0x70, 0x42, 0x81, 0x0A, 0x00, 0x82, 0x80,
    0x01, 0x00, 0x1F, 0x09, 0x15, 0x00, 0xC2, 0x80,
    0x0D, 0x00, 0x42, 0x81, 0xC4, 0x00, 0x1F, 0x0C,
    0x42, 0x81, 0x0A, 0x00, 0x82, 0x80, 0x01, 0x00,
    0x1F, 0x09, 0x15, 0x00, 0x01, 0x00, 0x06, 0x00,
    0x00, 0x70, 0x28, 0x01, 0x06, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
};
