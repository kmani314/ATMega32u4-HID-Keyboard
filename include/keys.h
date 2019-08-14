#ifndef KEYS_H
#define KEYS_H

// These are custom layer shift keys, they don't actually cause a keypress to be
// sent
#define KEY_MOD_MACRO 0x0100
#define KEY_LS_MACRO 0x1000

#define KEY_LAYER_UP 0x01
#define KEY_LAYER_DOWN 0x04
#define KEY_QUICK_FN 0x02

// I didn't write these defines because it's a waste of time, it's from here:
// https://gist.github.com/MightyPork/6da26e382a7ad91b5496ee55fdc73db2

/**
 * Modifier masks - used for the first byte in the HID report.
 * NOTE: The second byte in the report is reserved, 0x00
 */
#define KEY_MOD_LCTRL 0x01
#define KEY_MOD_LSHIFT 0x02
#define KEY_MOD_LALT 0x04
#define KEY_MOD_LMETA 0x08
#define KEY_MOD_RCTRL 0x10
#define KEY_MOD_RSHIFT 0x20
#define KEY_MOD_RALT 0x40
#define KEY_MOD_RMETA 0x80

/**
 * Scan codes - last N slots in the HID report (usually 6).
 * 0x00 if no key pressed.
 *
 * If more than N keys are pressed, the HID reports
 * KEY_ERR_OVF in all slots to indicate this condition.
 */

#define KEY_NONE 0x00  // No key pressed
#define KEY_ERR_OVF \
  0x01  //  Keyboard Error Roll Over - used for all slots if too many keys are
        //  pressed ("Phantom key")
// 0x02 //  Keyboard POST Fail
// 0x03 //  Keyboard Error Undefined
#define KEY_A 0x04  // Keyboard a and A
#define KEY_B 0x05  // Keyboard b and B
#define KEY_C 0x06  // Keyboard c and C
#define KEY_D 0x07  // Keyboard d and D
#define KEY_E 0x08  // Keyboard e and E
#define KEY_F 0x09  // Keyboard f and F
#define KEY_G 0x0a  // Keyboard g and G
#define KEY_H 0x0b  // Keyboard h and H
#define KEY_I 0x0c  // Keyboard i and I
#define KEY_J 0x0d  // Keyboard j and J
#define KEY_K 0x0e  // Keyboard k and K
#define KEY_L 0x0f  // Keyboard l and L
#define KEY_M 0x10  // Keyboard m and M
#define KEY_N 0x11  // Keyboard n and N
#define KEY_O 0x12  // Keyboard o and O
#define KEY_P 0x13  // Keyboard p and P
#define KEY_Q 0x14  // Keyboard q and Q
#define KEY_R 0x15  // Keyboard r and R
#define KEY_S 0x16  // Keyboard s and S
#define KEY_T 0x17  // Keyboard t and T
#define KEY_U 0x18  // Keyboard u and U
#define KEY_V 0x19  // Keyboard v and V
#define KEY_W 0x1a  // Keyboard w and W
#define KEY_X 0x1b  // Keyboard x and X
#define KEY_Y 0x1c  // Keyboard y and Y
#define KEY_Z 0x1d  // Keyboard z and Z

#define KEY_1 0x1e  // Keyboard 1 and !
#define KEY_2 0x1f  // Keyboard 2 and @
#define KEY_3 0x20  // Keyboard 3 and #
#define KEY_4 0x21  // Keyboard 4 and $
#define KEY_5 0x22  // Keyboard 5 and %
#define KEY_6 0x23  // Keyboard 6 and ^
#define KEY_7 0x24  // Keyboard 7 and &
#define KEY_8 0x25  // Keyboard 8 and *
#define KEY_9 0x26  // Keyboard 9 and (
#define KEY_0 0x27  // Keyboard 0 and )

#define KEY_ENTER 0x28       // Keyboard Return (ENTER)
#define KEY_ESC 0x29         // Keyboard ESCAPE
#define KEY_BACKSPACE 0x2a   // Keyboard DELETE (Backspace)
#define KEY_TAB 0x2b         // Keyboard Tab
#define KEY_SPACE 0x2c       // Keyboard Spacebar
#define KEY_MINUS 0x2d       // Keyboard - and _
#define KEY_EQUAL 0x2e       // Keyboard = and +
#define KEY_LEFTBRACE 0x2f   // Keyboard [ and {
#define KEY_RIGHTBRACE 0x30  // Keyboard ] and }
#define KEY_BACKSLASH 0x31   // Keyboard \ and |
#define KEY_HASHTILDE 0x32   // Keyboard Non-US # and ~
#define KEY_SEMICOLON 0x33   // Keyboard ; and :
#define KEY_APOSTROPHE 0x34  // Keyboard ' and "
#define KEY_GRAVE 0x35       // Keyboard ` and ~
#define KEY_COMMA 0x36       // Keyboard , and <
#define KEY_DOT 0x37         // Keyboard . and >
#define KEY_SLASH 0x38       // Keyboard / and ?
#define KEY_CAPSLOCK 0x39    // Keyboard Caps Lock

#define KEY_RIGHT 0x4f  // Keyboard Right Arrow
#define KEY_LEFT 0x50   // Keyboard Left Arrow
#define KEY_DOWN 0x51   // Keyboard Down Arrow
#define KEY_UP 0x52     // Keyboard Up Arrow

#define KEY_F1 0x3a   // Keyboard F1
#define KEY_F2 0x3b   // Keyboard F2
#define KEY_F3 0x3c   // Keyboard F3
#define KEY_F4 0x3d   // Keyboard F4
#define KEY_F5 0x3e   // Keyboard F5
#define KEY_F6 0x3f   // Keyboard F6
#define KEY_F7 0x40   // Keyboard F7
#define KEY_F8 0x41   // Keyboard F8
#define KEY_F9 0x42   // Keyboard F9
#define KEY_F10 0x43  // Keyboard F10
#define KEY_F11 0x44  // Keyboard F11
#define KEY_F12 0x45  // Keyboard F12

#define KEY_102ND 0x64    // Keyboard Non-US \ and |
#define KEY_COMPOSE 0x65  // Keyboard Application
#define KEY_POWER 0x66    // Keyboard Power
#define KEY_KPEQUAL 0x67  // Keypad =

#define KEY_F13 0x68  // Keyboard F13
#define KEY_F14 0x69  // Keyboard F14
#define KEY_F15 0x6a  // Keyboard F15
#define KEY_F16 0x6b  // Keyboard F16
#define KEY_F17 0x6c  // Keyboard F17
#define KEY_F18 0x6d  // Keyboard F18
#define KEY_F19 0x6e  // Keyboard F19
#define KEY_F20 0x6f  // Keyboard F20
#define KEY_F21 0x70  // Keyboard F21
#define KEY_F22 0x71  // Keyboard F22
#define KEY_F23 0x72  // Keyboard F23
#define KEY_F24 0x73  // Keyboard F24

#define KEY_OPEN 0x74        // Keyboard Execute
#define KEY_HELP 0x75        // Keyboard Help
#define KEY_PROPS 0x76       // Keyboard Menu
#define KEY_FRONT 0x77       // Keyboard Select
#define KEY_STOP 0x78        // Keyboard Stop
#define KEY_AGAIN 0x79       // Keyboard Again
#define KEY_UNDO 0x7a        // Keyboard Undo
#define KEY_CUT 0x7b         // Keyboard Cut
#define KEY_COPY 0x7c        // Keyboard Copy
#define KEY_PASTE 0x7d       // Keyboard Paste
#define KEY_FIND 0x7e        // Keyboard Find
#define KEY_MUTE 0x7f        // Keyboard Mute
#define KEY_VOLUMEUP 0x80    // Keyboard Volume Up
#define KEY_VOLUMEDOWN 0x81  // Keyboard Volume Down

#define KEY_KATAKANAHIRAGANA 0x88  // Keyboard International2
#define KEY_YEN 0x89               // Keyboard International3
#define KEY_HENKAN 0x8a            // Keyboard International4
#define KEY_MUHENKAN 0x8b          // Keyboard International5
#define KEY_KPJPCOMMA 0x8c         // Keyboard International6
#define KEY_HANGEUL 0x90           // Keyboard LANG1
#define KEY_HANJA 0x91             // Keyboard LANG2
#define KEY_KATAKANA 0x92          // Keyboard LANG3
#define KEY_HIRAGANA 0x93          // Keyboard LANG4
#define KEY_ZENKAKUHANKAKU 0x94    // Keyboard LANG5

#define KEY_MEDIA_PLAYPAUSE 0xe8
#define KEY_MEDIA_STOPCD 0xe9
#define KEY_MEDIA_PREVIOUSSONG 0xea
#define KEY_MEDIA_NEXTSONG 0xeb
#define KEY_MEDIA_EJECTCD 0xec
#define KEY_MEDIA_VOLUMEUP 0xed
#define KEY_MEDIA_VOLUMEDOWN 0xee
#define KEY_MEDIA_MUTE 0xef
#define KEY_MEDIA_WWW 0xf0
#define KEY_MEDIA_BACK 0xf1
#define KEY_MEDIA_FORWARD 0xf2
#define KEY_MEDIA_STOP 0xf3
#define KEY_MEDIA_FIND 0xf4
#define KEY_MEDIA_SCROLLUP 0xf5
#define KEY_MEDIA_SCROLLDOWN 0xf6
#define KEY_MEDIA_EDIT 0xf7
#define KEY_MEDIA_SLEEP 0xf8
#define KEY_MEDIA_COFFEE 0xf9
#define KEY_MEDIA_REFRESH 0xfa
#define KEY_MEDIA_CALC 0xfb

#endif
