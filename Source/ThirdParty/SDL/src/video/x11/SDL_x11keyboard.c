/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2016 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "../../SDL_internal.h"

#if SDL_VIDEO_DRIVER_X11

#include "SDL_x11video.h"

#include "../../events/SDL_keyboard_c.h"
#include "../../events/scancodes_darwin.h"
#include "../../events/scancodes_xfree86.h"

#include <X11/keysym.h>
#include <X11/XKBlib.h>

#include "imKStoUCS.h"

/* *INDENT-OFF* */
static const struct {
    KeySym keysym;
    SDL_Scancode scancode;
} KeySymToSDLScancode[] = {
    { XK_Return, SDL_SCANCODE_RETURN },
    { XK_Escape, SDL_SCANCODE_ESCAPE },
    { XK_BackSpace, SDL_SCANCODE_BACKSPACE },
    { XK_Tab, SDL_SCANCODE_TAB },
    { XK_Caps_Lock, SDL_SCANCODE_CAPSLOCK },
    { XK_F1, SDL_SCANCODE_F1 },
    { XK_F2, SDL_SCANCODE_F2 },
    { XK_F3, SDL_SCANCODE_F3 },
    { XK_F4, SDL_SCANCODE_F4 },
    { XK_F5, SDL_SCANCODE_F5 },
    { XK_F6, SDL_SCANCODE_F6 },
    { XK_F7, SDL_SCANCODE_F7 },
    { XK_F8, SDL_SCANCODE_F8 },
    { XK_F9, SDL_SCANCODE_F9 },
    { XK_F10, SDL_SCANCODE_F10 },
    { XK_F11, SDL_SCANCODE_F11 },
    { XK_F12, SDL_SCANCODE_F12 },
    { XK_Print, SDL_SCANCODE_PRINTSCREEN },
    { XK_Scroll_Lock, SDL_SCANCODE_SCROLLLOCK },
    { XK_Pause, SDL_SCANCODE_PAUSE },
    { XK_Insert, SDL_SCANCODE_INSERT },
    { XK_Home, SDL_SCANCODE_HOME },
    { XK_Prior, SDL_SCANCODE_PAGEUP },
    { XK_Delete, SDL_SCANCODE_DELETE },
    { XK_End, SDL_SCANCODE_END },
    { XK_Next, SDL_SCANCODE_PAGEDOWN },
    { XK_Right, SDL_SCANCODE_RIGHT },
    { XK_Left, SDL_SCANCODE_LEFT },
    { XK_Down, SDL_SCANCODE_DOWN },
    { XK_Up, SDL_SCANCODE_UP },
    { XK_Num_Lock, SDL_SCANCODE_NUMLOCKCLEAR },
    { XK_KP_Divide, SDL_SCANCODE_KP_DIVIDE },
    { XK_KP_Multiply, SDL_SCANCODE_KP_MULTIPLY },
    { XK_KP_Subtract, SDL_SCANCODE_KP_MINUS },
    { XK_KP_Add, SDL_SCANCODE_KP_PLUS },
    { XK_KP_Enter, SDL_SCANCODE_KP_ENTER },
    { XK_KP_Delete, SDL_SCANCODE_KP_PERIOD },
    { XK_KP_End, SDL_SCANCODE_KP_1 },
    { XK_KP_Down, SDL_SCANCODE_KP_2 },
    { XK_KP_Next, SDL_SCANCODE_KP_3 },
    { XK_KP_Left, SDL_SCANCODE_KP_4 },
    { XK_KP_Begin, SDL_SCANCODE_KP_5 },
    { XK_KP_Right, SDL_SCANCODE_KP_6 },
    { XK_KP_Home, SDL_SCANCODE_KP_7 },
    { XK_KP_Up, SDL_SCANCODE_KP_8 },
    { XK_KP_Prior, SDL_SCANCODE_KP_9 },
    { XK_KP_Insert, SDL_SCANCODE_KP_0 },
    { XK_KP_Decimal, SDL_SCANCODE_KP_PERIOD },
    { XK_KP_1, SDL_SCANCODE_KP_1 },
    { XK_KP_2, SDL_SCANCODE_KP_2 },
    { XK_KP_3, SDL_SCANCODE_KP_3 },
    { XK_KP_4, SDL_SCANCODE_KP_4 },
    { XK_KP_5, SDL_SCANCODE_KP_5 },
    { XK_KP_6, SDL_SCANCODE_KP_6 },
    { XK_KP_7, SDL_SCANCODE_KP_7 },
    { XK_KP_8, SDL_SCANCODE_KP_8 },
    { XK_KP_9, SDL_SCANCODE_KP_9 },
    { XK_KP_0, SDL_SCANCODE_KP_0 },
    { XK_KP_Decimal, SDL_SCANCODE_KP_PERIOD },
    { XK_Hyper_R, SDL_SCANCODE_APPLICATION },
    { XK_KP_Equal, SDL_SCANCODE_KP_EQUALS },
    { XK_F13, SDL_SCANCODE_F13 },
    { XK_F14, SDL_SCANCODE_F14 },
    { XK_F15, SDL_SCANCODE_F15 },
    { XK_F16, SDL_SCANCODE_F16 },
    { XK_F17, SDL_SCANCODE_F17 },
    { XK_F18, SDL_SCANCODE_F18 },
    { XK_F19, SDL_SCANCODE_F19 },
    { XK_F20, SDL_SCANCODE_F20 },
    { XK_F21, SDL_SCANCODE_F21 },
    { XK_F22, SDL_SCANCODE_F22 },
    { XK_F23, SDL_SCANCODE_F23 },
    { XK_F24, SDL_SCANCODE_F24 },
    { XK_Execute, SDL_SCANCODE_EXECUTE },
    { XK_Help, SDL_SCANCODE_HELP },
    { XK_Menu, SDL_SCANCODE_MENU },
    { XK_Select, SDL_SCANCODE_SELECT },
    { XK_Cancel, SDL_SCANCODE_STOP },
    { XK_Redo, SDL_SCANCODE_AGAIN },
    { XK_Undo, SDL_SCANCODE_UNDO },
    { XK_Find, SDL_SCANCODE_FIND },
    { XK_KP_Separator, SDL_SCANCODE_KP_COMMA },
    { XK_Sys_Req, SDL_SCANCODE_SYSREQ },
    { XK_Control_L, SDL_SCANCODE_LCTRL },
    { XK_Shift_L, SDL_SCANCODE_LSHIFT },
    { XK_Alt_L, SDL_SCANCODE_LALT },
    { XK_Meta_L, SDL_SCANCODE_LGUI },
    { XK_Super_L, SDL_SCANCODE_LGUI },
    { XK_Control_R, SDL_SCANCODE_RCTRL },
    { XK_Shift_R, SDL_SCANCODE_RSHIFT },
    { XK_Alt_R, SDL_SCANCODE_RALT },
    { XK_Meta_R, SDL_SCANCODE_RGUI },
    { XK_Super_R, SDL_SCANCODE_RGUI },
    { XK_Mode_switch, SDL_SCANCODE_MODE },
};

static const struct
{
    SDL_Scancode const *table;
    int table_size;
} scancode_set[] = {
    { darwin_scancode_table, SDL_arraysize(darwin_scancode_table) },
    { xfree86_scancode_table, SDL_arraysize(xfree86_scancode_table) },
    { xfree86_scancode_table2, SDL_arraysize(xfree86_scancode_table2) },
};
/* *INDENT-OFF* */

/* This function only works for keyboards in US QWERTY layout */
static SDL_Scancode
X11_KeyCodeToSDLScancode(Display *display, KeyCode keycode)
{
    KeySym keysym;
    int i;

#if SDL_VIDEO_DRIVER_X11_HAS_XKBKEYCODETOKEYSYM
    keysym = X11_XkbKeycodeToKeysym(display, keycode, 0, 0);
#else
    keysym = X11_XKeycodeToKeysym(display, keycode, 0);
#endif
    if (keysym == NoSymbol) {
        return SDL_SCANCODE_UNKNOWN;
    }

    if (keysym >= XK_A && keysym <= XK_Z) {
        return SDL_SCANCODE_A + (keysym - XK_A);
    }

    if (keysym >= XK_0 && keysym <= XK_9) {
        return SDL_SCANCODE_0 + (keysym - XK_0);
    }

    for (i = 0; i < SDL_arraysize(KeySymToSDLScancode); ++i) {
        if (keysym == KeySymToSDLScancode[i].keysym) {
            return KeySymToSDLScancode[i].scancode;
        }
    }
    return SDL_SCANCODE_UNKNOWN;
}

static Uint32
X11_KeyCodeToUcs4(Display *display, KeyCode keycode, unsigned char group)
{
    KeySym keysym;

#if SDL_VIDEO_DRIVER_X11_HAS_XKBKEYCODETOKEYSYM
    keysym = X11_XkbKeycodeToKeysym(display, keycode, group, 0);
#else
    keysym = X11_XKeycodeToKeysym(display, keycode, 0);
#endif
    if (keysym == NoSymbol) {
        return 0;
    }

    return X11_KeySymToUcs4(keysym);
}

int
X11_InitKeyboard(_THIS)
{
    SDL_VideoData *data = (SDL_VideoData *) _this->driverdata;
    int i = 0;
    int j = 0;
    int min_keycode, max_keycode;
    struct {
        SDL_Scancode scancode;
        KeySym keysym;
        int value;
    } fingerprint[] = {
        { SDL_SCANCODE_HOME, XK_Home, 0 },
        { SDL_SCANCODE_PAGEUP, XK_Prior, 0 },
        { SDL_SCANCODE_UP, XK_Up, 0 },
        { SDL_SCANCODE_LEFT, XK_Left, 0 },
        { SDL_SCANCODE_DELETE, XK_Delete, 0 },
        { SDL_SCANCODE_KP_ENTER, XK_KP_Enter, 0 },
    };
    int best_distance;
    int best_index;
    int distance;

    X11_XAutoRepeatOn(data->display);

    /* Try to determine which scancodes are being used based on fingerprint */
    best_distance = SDL_arraysize(fingerprint) + 1;
    best_index = -1;
    X11_XDisplayKeycodes(data->display, &min_keycode, &max_keycode);
    for (i = 0; i < SDL_arraysize(fingerprint); ++i) {
        fingerprint[i].value =
            X11_XKeysymToKeycode(data->display, fingerprint[i].keysym) -
            min_keycode;
    }
    for (i = 0; i < SDL_arraysize(scancode_set); ++i) {
        /* Make sure the scancode set isn't too big */
        if ((max_keycode - min_keycode + 1) <= scancode_set[i].table_size) {
            continue;
        }
        distance = 0;
        for (j = 0; j < SDL_arraysize(fingerprint); ++j) {
            if (fingerprint[j].value < 0
                || fingerprint[j].value >= scancode_set[i].table_size) {
                distance += 1;
            } else if (scancode_set[i].table[fingerprint[j].value] != fingerprint[j].scancode) {
                distance += 1;
            }
        }
        if (distance < best_distance) {
            best_distance = distance;
            best_index = i;
        }
    }
    if (best_index >= 0 && best_distance <= 2) {
#ifdef DEBUG_KEYBOARD
        printf("Using scancode set %d, min_keycode = %d, max_keycode = %d, table_size = %d\n", best_index, min_keycode, max_keycode, scancode_set[best_index].table_size);
#endif
        SDL_memcpy(&data->key_layout[min_keycode], scancode_set[best_index].table,
                   sizeof(SDL_Scancode) * scancode_set[best_index].table_size);
    } else {
        SDL_Keycode keymap[SDL_NUM_SCANCODES];

        printf
            ("Keyboard layout unknown, please send the following to the SDL mailing list (sdl@libsdl.org):\n");

        /* Determine key_layout - only works on US QWERTY layout */
        SDL_GetDefaultKeymap(keymap);
        for (i = min_keycode; i <= max_keycode; ++i) {
            KeySym sym;
#if SDL_VIDEO_DRIVER_X11_HAS_XKBKEYCODETOKEYSYM
            sym = X11_XkbKeycodeToKeysym(data->display, i, 0, 0);
#else
            sym = X11_XKeycodeToKeysym(data->display, i, 0);
#endif
            if (sym != NoSymbol) {
                SDL_Scancode scancode;
                printf("code = %d, sym = 0x%X (%s) ", i - min_keycode,
                       (unsigned int) sym, X11_XKeysymToString(sym));
                scancode = X11_KeyCodeToSDLScancode(data->display, i);
                data->key_layout[i] = scancode;
                if (scancode == SDL_SCANCODE_UNKNOWN) {
                    printf("scancode not found\n");
                } else {
                    printf("scancode = %d (%s)\n", scancode, SDL_GetScancodeName(scancode));
                }
            }
        }
    }

    X11_UpdateKeymap(_this);

    SDL_SetScancodeName(SDL_SCANCODE_APPLICATION, "Menu");

#ifdef SDL_USE_IBUS
    SDL_IBus_Init();
#endif

    return 0;
}

void
X11_UpdateKeymap(_THIS)
{
    SDL_VideoData *data = (SDL_VideoData *) _this->driverdata;
    int i;
    SDL_Scancode scancode;
    SDL_Keycode keymap[SDL_NUM_SCANCODES];
    unsigned char group = 0;

    SDL_GetDefaultKeymap(keymap);
    
#if SDL_VIDEO_DRIVER_X11_HAS_XKBKEYCODETOKEYSYM
    {
        XkbStateRec state;
        if (X11_XkbGetState(data->display, XkbUseCoreKbd, &state) == Success) {
            group = state.group;
        }
    }
#endif


    for (i = 0; i < SDL_arraysize(data->key_layout); i++) {
        Uint32 key;

        /* Make sure this is a valid scancode */
        scancode = data->key_layout[i];
        if (scancode == SDL_SCANCODE_UNKNOWN) {
            continue;
        }

        /* See if there is a UCS keycode for this scancode */
        key = X11_KeyCodeToUcs4(data->display, (KeyCode)i, group);
        if (key) {
            keymap[scancode] = key;
        } else {
            SDL_Scancode keyScancode = X11_KeyCodeToSDLScancode(data->display, (KeyCode)i);

            switch (keyScancode) {
                case SDL_SCANCODE_RETURN:
                    keymap[scancode] = SDLK_RETURN;
                    break;
                case SDL_SCANCODE_ESCAPE:
                    keymap[scancode] = SDLK_ESCAPE;
                    break;
                case SDL_SCANCODE_BACKSPACE:
                    keymap[scancode] = SDLK_BACKSPACE;
                    break;
                case SDL_SCANCODE_TAB:
                    keymap[scancode] = SDLK_TAB;
                    break;
                case SDL_SCANCODE_DELETE:
                    keymap[scancode] = SDLK_DELETE;
                    break;
                default:
                    keymap[scancode] = SDL_SCANCODE_TO_KEYCODE(keyScancode);
                    break;
            }
        }
    }
    SDL_SetKeymap(0, keymap, SDL_NUM_SCANCODES);
}

void
X11_QuitKeyboard(_THIS)
{
#ifdef SDL_USE_IBUS
    SDL_IBus_Quit();
#endif
}

void
X11_StartTextInput(_THIS)
{

}

void
X11_StopTextInput(_THIS)
{
#ifdef SDL_USE_IBUS
    SDL_IBus_Reset();
#endif
}

void
X11_SetTextInputRect(_THIS, SDL_Rect *rect)
{
    if (!rect) {
        SDL_InvalidParamError("rect");
        return;
    }
       
#ifdef SDL_USE_IBUS
    SDL_IBus_UpdateTextRect(rect);
#endif
}

#endif /* SDL_VIDEO_DRIVER_X11 */

/* vi: set ts=4 sw=4 expandtab: */
