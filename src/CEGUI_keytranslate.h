/* Translate a windows scan code to ascii */

// did this because was having trouble with MapVirtualKeyEx()
// and it gives more control over what is accepted anyways

#ifndef __CEGUI_keytranslate_h_
#define __CEGUI_keytranslate_h_

#include "CEGUI.h"
#include <Shobjidl.h>

struct mapped_scan_code
{
  char scan_code;
  char ascii;
  char ascii_with_shift;
  char ascii_with_ctrl;
  char ascii_with_alt;
  CEGUI::utf32 utf;
  CEGUI::utf32 utf_with_shift;
};

mapped_scan_code mapped_scan_codes[] = {
  0x00,    0,    0,    0,    0,     0,   0,      
  0x01, 0x1B, 0x1B, 0x1B, 0x1B,    27,  27,     // ESC
  0x02, 0x31, 0x21,    0,    0,    49,  33,     // 1
  0x03, 0x32, 0x40,    0,    0,    50,  64,     // 2
  0x04, 0x33, 0x23,    0,    0,    51,  35,     // 3
  0x05, 0x34, 0x24,    0,    0,    52,  36,     // 4
  0x06, 0x35, 0x25,    0,    0,    53,  37,     // 5
  0x07, 0x36, 0x5E,    0,    0,    54,  94,     // 6
  0x08, 0x37, 0x26,    0,    0,    55,  38,     // 7
  0x09, 0x38, 0x2A,    0,    0,    56,  42,     // 8
  0x0A, 0x39, 0x28,    0,    0,    57,  40,     // 9
  0x0B, 0x30, 0x29,    0,    0,    48,  41,     // 0
  0x0C, 0x2D, 0x5F,    0,    0,    45,  95,     // -
  0x0D, 0x3D, 0x2B,    0,    0,    61,  43,     // =
  0x0E, 0x08, 0x08, 0x08, 0x08,     8,   8,     // BKSP
  0x0F, 0x09,    0,    0,    0,     9,   9,     // TAB
  0x10, 0x71, 0x51,    0,    0,   113,  81,     // Q
  0x11, 0x77, 0x57,    0,    0,   119,  87,     // W
  0x12, 0x65, 0x45,    0,    0,   101,  69,     // E
  0x13, 0x72, 0x52,    0,    0,   114,  82,     // R
  0x14, 0x74, 0x54,    0,    0,   116,  84,     // T
  0x15, 0x79, 0x59,    0,    0,   121,  89,     // Y
  0x16, 0x75, 0x55,    0,    0,   117,  85,     // U
  0x17, 0x69, 0x49,    0,    0,   105,  73,     // I
  0x18, 0x6F, 0x4F,    0,    0,   111,  79,     // O
  0x19, 0x70, 0x50,    0,    0,   112,  80,     // P
  0x1A, 0x5B, 0x7B,    0,    0,    91, 123,     // {
  0x1B, 0x5D, 0x7D,    0,    0,    93, 125,     // }
  0x1C, 0x0D, 0x0D, 0x0A,    0,    13,  13,     // ENTER
  0x1D,    0,    0,    0,    0,     0,   0,	     
  0x1E, 0x61, 0x41,    0,    0,    97,  65,     // A
  0x1F, 0x73, 0x53,    0,    0,   115,  83,     // S
  0x20, 0x64, 0x44,    0,    0,   100,  68,     // D
  0x21, 0x66, 0x46,    0,    0,   102,  70,     // F
  0x22, 0x67, 0x47,    0,    0,   103,  71,     // G
  0x23, 0x68, 0x48,    0,    0,   104,  72,     // H
  0x24, 0x6A, 0x4A,    0,    0,   106,  74,     // J
  0x25, 0x6B, 0x4B,    0,    0,   107,  75,     // K
  0x26, 0x6C, 0x4C,    0,    0,   108,  76,     // L
  0x27, 0x3B, 0x3A,    0,    0,    59,  58,     // :
  0x28, 0x27, 0x22,    0,    0,    39,  34,     // "
  0x29, 0x60, 0x7E,    0,    0,    96, 126,     // ~
  0x2A,    0,    0,    0,    0,     0,   0,	     
  0x2B, 0x5C, 0x7C,    0,    0,    92, 124,     // "\"
  0x2C, 0x7A, 0x5A,    0,    0,   122,  90,     // Z
  0x2D, 0x78, 0x58,    0,    0,   120,  88,     // X
  0x2E, 0x63, 0x43,    0,    0,    99,  67,     // C
  0x2F, 0x76, 0x56,    0,    0,   118,  86,     // V
  0x30, 0x62, 0x42,    0,    0,    98,  66,     // B
  0x31, 0x6E, 0x4E,    0,    0,   110,  78,     // N
  0x32, 0x6D, 0x4D,    0,    0,   109,  77,     // M
  0x33, 0x2C, 0x3C,    0,    0,    44,  60,     // <
  0x34, 0x2E, 0x3E,    0,    0,    46,  62,     // >
  0x35, 0x2F, 0x3F,    0,    0,    47,  63,     // ?
  0x36,    0,    0,    0,    0,     0,   0,     // SHIFT
  0x37,    0,    0,    0,    0,    42,  42,     // PRT SCREEN
  0x38,    0,    0,    0,    0,     0,   0,     // ALT
  0x39, 0x20, 0x20, 0x20, 0x20,    32,  32,     // SPACE
  0x3A,    0,    0,    0,    0,     0,   0,     // CAPS
  0x3B,    0,    0,    0,    0,     0,   0,     // F1
  0x3C,    0,    0,    0,    0,     0,   0,     // F2
  0x3D,    0,    0,    0,    0,     0,   0,     // F3
  0x3E,    0,    0,    0,    0,     0,   0,     // F4
  0x3F,    0,    0,    0,    0,     0,   0,     // F5
  0x40,    0,    0,    0,    0,     0,   0,     // F6
  0x41,    0,    0,    0,    0,     0,   0,     // F7
  0x42,    0,    0,    0,    0,     0,   0,     // F8
  0x43,    0,    0,    0,    0,     0,   0,     // F9
  0x44,    0,    0,    0,    0,     0,   0,     // F10
  0x45,    0,    0,    0,    0,     0,   0,     // NUM
  0x46,    0,    0,    0,    0,     0,   0,     // SCROLL
  0x47,    0, 0x37,    0,    0,     0,   0,     // HOME
  0x48,    0, 0x38,    0,    0,     0,   0,     // UP
  0x49,    0, 0x39,    0,    0,     0,   0,     // PGUP
  0x4A,    0, 0x2D,    0,    0,    45,  45,     // NUM -
  0x4B,    0, 0x34,    0,    0,     0,   0,     // LEFT
  0x4C,    0, 0x35,    0,    0,     0,   0,     // CENTER?
  0x4D,    0, 0x36,    0,    0,     0,   0,     // RIGHT
  0x4E,    0, 0x2B,    0,    0,    43,  43,     // NUM +
  0x4F,    0, 0x31,    0,    0,     0,   0,     // END
  0x50,    0, 0x32,    0,    0,     0,   0,     // DOWN
  0x51,    0, 0x33,    0,    0,     0,   0,     // PGDOWN
  0x52,    0, 0x30,    0,    0,     0,   0,     // INS
  0x53,    0, 0x2E,    0,    0,     0,   0,     // DEL
  0x54,    0,    0,    0,    0,     0,   0,	     
  0x55,    0,    0,    0,    0,     0,   0,	     
  0x56,    0,    0,    0,    0,    92, 124,     // NUM "/" 
  0x57,    0,    0,    0,    0,     0,   0,     // F11
  0x58,    0,    0,    0,    0,     0,   0      // F12
};

CEGUI::utf32 translate_scan_code(unsigned short key, bool shift)
{
  char rv;
  if (key > 0 && key < 0x59) {
    if (!shift)
      rv = mapped_scan_codes[key].utf;
    else
      rv = mapped_scan_codes[key].utf_with_shift;
    return rv;
  }
  return (char)0;
}

#endif // #ifndef __CEGUI_keytranslate_h_
