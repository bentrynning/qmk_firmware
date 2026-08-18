#include QMK_KEYBOARD_H
#include "os_detection.h"

enum sofle_layers {
    _QWERTY,
    _ARROWS,
    _NUMS,
    _F,
    _GAME,
};

enum custom_keycodes {
    KC_LOWER = SAFE_RANGE,
    KC_RAISE,
    BIT_0,
    BIT_1,
    BIT_2,
    BIT_3,
    BIT_4,
    EMOJI_HANDS,
    EMOJI_LAUGH,
    EMOJI_HEART,
    TOGGLE_GAMING,
    EXIT_GAMING,
    CHAT_MODE,
    CAP_AREA,
    CAP_DESK,
    REC_SCRN,
};

static bool is_apple = false;
static bool chat_mode_active = false;
static bool left_shift_pressed = false;
static bool right_shift_pressed = false;
static bool caps_toggle_triggered = false;

const uint16_t PROGMEM combo_lparen[] = {KC_D, KC_F, COMBO_END};
const uint16_t PROGMEM combo_rparen[] = {KC_J, KC_K, COMBO_END};
const uint16_t PROGMEM combo_lbracket[] = {KC_E, KC_R, COMBO_END};
const uint16_t PROGMEM combo_rbracket[] = {KC_U, KC_I, COMBO_END};
const uint16_t PROGMEM combo_lbrace[] = {KC_C, KC_V, COMBO_END};
const uint16_t PROGMEM combo_rbrace[] = {KC_M, KC_COMM, COMBO_END};
const uint16_t PROGMEM combo_less[] = {KC_W, KC_E, COMBO_END};
const uint16_t PROGMEM combo_greater[] = {KC_I, KC_O, COMBO_END};
const uint16_t PROGMEM combo_tilde[] = {KC_A, KC_S, COMBO_END};
const uint16_t PROGMEM combo_grave[] = {KC_S, KC_D, COMBO_END};
const uint16_t PROGMEM combo_plus[] = {KC_R, KC_U, COMBO_END};
const uint16_t PROGMEM combo_minus[] = {KC_V, KC_M, COMBO_END};
const uint16_t PROGMEM combo_equal[] = {KC_F, KC_J, COMBO_END};
const uint16_t PROGMEM combo_asterisk[] = {KC_E, KC_I, COMBO_END};
const uint16_t PROGMEM combo_underscore[] = {KC_R, KC_T, COMBO_END};
const uint16_t PROGMEM combo_word_del[] = {KC_4, KC_5, COMBO_END};
const uint16_t PROGMEM combo_emoji_hands[] = {KC_T, KC_Y, COMBO_END};
const uint16_t PROGMEM combo_emoji_laugh[] = {KC_G, KC_H, COMBO_END};
const uint16_t PROGMEM combo_emoji_heart[] = {KC_B, KC_N, COMBO_END};
const uint16_t PROGMEM combo_exclamation[] = {KC_Y, KC_U, COMBO_END};
const uint16_t PROGMEM combo_ampersand[] = {KC_D, KC_K, COMBO_END};
const uint16_t PROGMEM combo_dollar[] = {KC_S, KC_L, COMBO_END};
const uint16_t PROGMEM combo_hash[] = {KC_F, KC_G, COMBO_END};
const uint16_t PROGMEM combo_at[] = {KC_H, KC_J, COMBO_END};
const uint16_t PROGMEM combo_percent[] = {KC_V, KC_B, COMBO_END};
const uint16_t PROGMEM combo_caret[] = {KC_N, KC_M, COMBO_END};
const uint16_t PROGMEM combo_backslash[] = {KC_K, KC_L, COMBO_END};
const uint16_t PROGMEM combo_pipe[] = {KC_L, KC_SCLN, COMBO_END};

combo_t key_combos[] = {
    COMBO(combo_lparen, KC_LPRN),
    COMBO(combo_rparen, KC_RPRN),
    COMBO(combo_lbracket, KC_LBRC),
    COMBO(combo_rbracket, KC_RBRC),
    COMBO(combo_lbrace, KC_LCBR),
    COMBO(combo_rbrace, KC_RCBR),
    COMBO(combo_less, KC_LT),
    COMBO(combo_greater, KC_GT),
    COMBO(combo_tilde, KC_TILD),
    COMBO(combo_grave, KC_GRV),
    COMBO(combo_plus, KC_PLUS),
    COMBO(combo_minus, KC_MINS),
    COMBO(combo_equal, KC_EQL),
    COMBO(combo_asterisk, KC_ASTR),
    COMBO(combo_underscore, KC_UNDS),
    COMBO(combo_word_del, LALT(KC_BSPC)),
    COMBO(combo_emoji_hands, EMOJI_HANDS),
    COMBO(combo_emoji_laugh, EMOJI_LAUGH),
    COMBO(combo_emoji_heart, EMOJI_HEART),
    COMBO(combo_exclamation, KC_EXLM),
    COMBO(combo_ampersand, KC_AMPR),
    COMBO(combo_dollar, KC_DLR),
    COMBO(combo_hash, KC_HASH),
    COMBO(combo_at, KC_AT),
    COMBO(combo_percent, KC_PERC),
    COMBO(combo_caret, KC_CIRC),
    COMBO(combo_backslash, KC_BSLS),
    COMBO(combo_pipe, KC_PIPE),
};

uint16_t COMBO_LEN = ARRAY_SIZE(key_combos);

bool combo_should_trigger(uint16_t combo_index, combo_t *combo, uint16_t keycode, keyrecord_t *record) {
    return !layer_state_is(_GAME);
}

static int8_t bitwise_pressed_keys = 0;
static uint8_t bitwise_accumulator = 0;
const uint16_t bitwise_f_keys[] = {BIT_0, BIT_1, BIT_2, BIT_3, BIT_4};
const uint16_t f_keys[] = {
    KC_NO, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8,
    KC_F9, KC_F10, KC_F11, KC_F12, KC_F13, KC_F14, KC_F15, KC_F16,
    KC_F17, KC_F18, KC_F19, KC_F20, KC_F21, KC_F22, KC_F23, KC_F24,
};

static bool process_bitwise_f(uint16_t keycode, keyrecord_t *record) {
    if (get_highest_layer(layer_state) != _F) {
        return true;
    }

    int8_t index = -1;
    for (uint8_t i = 0; i < ARRAY_SIZE(bitwise_f_keys); ++i) {
        if (keycode == bitwise_f_keys[i]) {
            index = i;
            break;
        }
    }
    if (index < 0) {
        return true;
    }

    if (record->event.pressed) {
        bitwise_pressed_keys++;
        bitwise_accumulator |= (1 << index);
    } else if (--bitwise_pressed_keys <= 0) {
        bitwise_pressed_keys = 0;
        if (bitwise_accumulator < ARRAY_SIZE(f_keys) && f_keys[bitwise_accumulator] != KC_NO) {
            tap_code(f_keys[bitwise_accumulator]);
        }
        bitwise_accumulator = 0;
    }
    return false;
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* QWERTY
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |  `   |   1  |   2  |   3  |   4  |   5  |                    |   6  |   7  |   8  |   9  |   0  |  `   |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * | Tab  |   Q  |   W  |   E  |   R  |   T  |                    |   Y  |   U  |   I  |   O  |   P  | Bspc |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |CtlEsc|   A  |   S  |   D  |   F  |   G  |-------.    ,-------|   H  |   J  |   K  |   L  |   ;  |  '   |
 * |------+------+------+------+------+------|  MUTE |    |       |------+------+------+------+------+------|
 * |LShift|   Z  |   X  |   C  |   V  |   B  |-------|    |-------|   N  |   M  |   ,  |   .  |   /  |RShift|
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *            | LCtl | LAlt | LGUI |Lower | /Space  /       \Enter \  |Raise | RAlt | RGUI | RCtl |
 *            |      |      |      |      |/       /         \      \ |      |      |      |      |
 *            `----------------------------------'           '------''---------------------------'
     */

    [_QWERTY] = LAYOUT(
        KC_GRV,          KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                     KC_6,    KC_7,    KC_8,    KC_9,    KC_0, KC_GRV,
        KC_TAB,          KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                     KC_Y,    KC_U,    KC_I,    KC_O,    KC_P, KC_BSPC,
        LCTL_T(KC_ESC),  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                     KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN, KC_QUOT,
        KC_LSFT,         KC_Z,    KC_X,    KC_C,    KC_V,    KC_B, KC_MUTE,   XXXXXXX,    KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH, KC_RSFT,
                                   KC_LCTL, KC_LALT, KC_LGUI, KC_LOWER, KC_SPC, KC_ENT, KC_RAISE, KC_RALT, KC_RGUI, KC_RCTL
    ),
    /* NAVIGATION / MEDIA
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      | Vol- | Mute | Vol+ | RGB+ |                    | Home | PgDn | PgUp | End  |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      | RGB< | Prev | Play | Next | RGB- |-------.    ,-------| Left | Down |  Up  |Right |      |      |
 * |------+------+------+------+------+------|  MUTE |    |       |------+------+------+------+------+------|
 * |      | RGB  | RGB> | Hue+ | Sat+ | Sat- |-------|    |-------| Ins  | Pscr | Scrl | Pause|      |      |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *            |      |      |      |      |      | /       /       \      \ |      |      |      |      |
 *            |      |      |      |      |      |/       /         \      \|      |      |      |      |
 *            `----------------------------------'           '------''---------------------------'
     */

    [_ARROWS] = LAYOUT(
        _______, _______, _______, _______, _______, _______,                     _______, _______, _______, _______, _______, _______,
        _______, _______, KC_VOLD, KC_MUTE, KC_VOLU, UG_VALU,                     KC_HOME, KC_PGDN, KC_PGUP,  KC_END, _______, _______,
        _______, UG_PREV, KC_MPRV, KC_MPLY, KC_MNXT, UG_VALD,                     KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, _______, _______,
        _______, UG_TOGG, UG_NEXT, UG_HUEU, UG_SATU, UG_SATD, _______,    _______, KC_INS, KC_PSCR, KC_SCRL, KC_PAUS, _______, _______,
                                   _______, _______, _______, _______, _______,    _______, _______, _______, _______, _______
    ),
    /* NUMBERS / SYMBOLS
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |   !  |   @  |   *  |   `  |   ~  |                    |   ]  |   7  |   8  |   9  |   +  | Bspc |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |   #  |   '  |   "  |   -  |   =  |-------.    ,-------|   )  |   4  |   5  |   6  |   -  |      |
 * |------+------+------+------+------+------|  MUTE |    |       |------+------+------+------+------+------|
 * |      |   _  |   $  |   %  |   ^  |   &  |-------|    |-------|   }  |   1  |   2  |   3  |   /  |      |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *            |      |      |      |      |      | /       /       \ FnSpc\ |   0  |   .  |      |      |
 *            |      |      |      |      |      |/       /         \      \|      |      |      |      |
 *            `----------------------------------'           '------''---------------------------'
      */
     [_NUMS] = LAYOUT(
    _______, _______, _______, _______, _______, _______,                     _______, _______, _______, _______, _______, _______,
    _______, KC_EXLM,   KC_AT, KC_ASTR,  KC_GRV, KC_TILD,                     KC_RBRC,    KC_7,    KC_8,    KC_9, KC_PLUS, KC_BSPC,
    _______, KC_HASH, KC_QUOT, KC_DQUO, KC_MINS,  KC_EQL,                     KC_RPRN,    KC_4,    KC_5,    KC_6, KC_MINS, _______,
    _______, KC_UNDS,  KC_DLR, KC_PERC, KC_CIRC, KC_AMPR, _______,    _______, KC_RCBR,    KC_1,    KC_2,    KC_3, KC_SLSH, _______,
                                        _______, _______, _______, _______, _______,    LT(_F, KC_SPC), KC_0, KC_DOT, _______, _______
    ),
    /* FUNCTION / CAPTURE
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |      |      |      |      |                    |      | Desk |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |  F+1 |  F+2 |  F+4 |  F+8 | F+16 |-------.    ,-------|      | Area |      |      |      |      |
 * |------+------+------+------+------+------|  MUTE |    |       |------+------+------+------+------+------|
 * |      |      |      |      |      |      |-------|    |-------|      | Rec  |      |      |      | Game |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *            |      |      |      |      |      | /       /       \      \ |      |      |      |      |
 *            |      |      |      |      |      |/       /         \      \|      |      |      |      |
 *            `----------------------------------'           '------''---------------------------'
      */
     [_F] = LAYOUT(
    _______, _______, _______, _______, _______, _______,                     _______, _______, _______, _______, _______, _______,
    _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                     XXXXXXX, CAP_DESK, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    _______,   BIT_0,   BIT_1,   BIT_2,   BIT_3,   BIT_4,                     XXXXXXX, CAP_AREA, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______,    _______, XXXXXXX, REC_SCRN, XXXXXXX, XXXXXXX, XXXXXXX, TOGGLE_GAMING,
                                        _______, _______, _______, _______, _______,    _______, _______, _______, _______, _______
    ),
    /* GAMING
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * | Tab  |   Q  |   W  |   E  |   R  |   T  |                    |   Y  |   U  |   I  |   O  |   P  | Bspc |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |Shift |   A  |   S  |   D  |   F  |   G  |-------.    ,-------|   H  |   J  |   K  |   L  |   ;  |   '  |
 * |------+------+------+------+------+------|  MUTE |    |       |------+------+------+------+------+------|
 * | Ctrl |   Z  |   X  |   C  |   V  |   B  |-------|    |-------|   N  |   M  |   ,  |   .  |   /  | Exit |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *            |      |      | Alt  | Esc  | Spc  | /       /       \ Ent  \ | Bspc | Chat |      |      |
 *            |      |      |      |      |      |/       /         \      \|      |      |      |      |
 *            `----------------------------------'           '------''---------------------------'
      */
    [_GAME] = LAYOUT(
          _______, _______, _______, _______, _______, _______,                     _______, _______, _______, _______, _______, _______,
          KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                        KC_Y,    KC_U,    KC_I,    KC_O,    KC_P, KC_BSPC,
          KC_LSFT,   KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                        KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN, KC_QUOT,
          KC_LCTL,   KC_Z,    KC_X,    KC_C,    KC_V,    KC_B, _______,    _______,    KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH, EXIT_GAMING,
                                              _______, _______, KC_LALT, KC_ESC, KC_SPC, KC_ENT, KC_BSPC, CHAT_MODE, _______, _______
     )
};

#ifdef OLED_ENABLE

static void render_logo(void) {
    static const char PROGMEM qmk_logo[] = {
        0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,0x90,0x91,0x92,0x93,0x94,
        0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,
        0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,0xd0,0xd1,0xd2,0xd3,0xd4,0
    };

    oled_write_P(qmk_logo, false);
}

static void print_status_narrow(void) {
    // Print current mode
    oled_write_P(PSTR("\n\n"), false);
    oled_write_ln_P(PSTR("MODE"), false);
    oled_write_ln_P(PSTR(""), false);
    if (keymap_config.swap_lctl_lgui) {
        oled_write_ln_P(PSTR("MAC"), false);
    } else {
        oled_write_ln_P(PSTR("WIN"), false);
    }

    oled_write_ln_P(PSTR("Qwrt"), false);
    oled_write_P(PSTR("\n\n"), false);
    // Print current layer
    oled_write_ln_P(PSTR("LAYER"), false);
    switch (get_highest_layer(layer_state)) {
        case _QWERTY:
            oled_write_P(PSTR("Base\n"), false);
            break;
        case _ARROWS:
            oled_write_P(PSTR("Nav\n"), false);
            break;
        case _NUMS:
            oled_write_P(PSTR("Nums\n"), false);
            break;
        case _F:
            oled_write_P(PSTR("Fn\n"), false);
            break;
        case _GAME:
            oled_write_P(PSTR("Game\n"), false);
            break;
        default:
            oled_write_ln_P(PSTR("Undef"), false);
    }
    oled_write_P(PSTR("\n\n"), false);
    led_t led_usb_state = host_keyboard_led_state();
    oled_write_ln_P(PSTR("CPSLK"), led_usb_state.caps_lock);
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (is_keyboard_master()) {
        return OLED_ROTATION_270;
    }
    return rotation;
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        print_status_narrow();
    } else {
        render_logo();
    }
    return false;
}

#endif

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (chat_mode_active && record->event.pressed && keycode == KC_ENT) {
        chat_mode_active = false;
        layer_clear();
        layer_on(_GAME);
        return true;
    }

    if (!process_bitwise_f(keycode, record)) {
        return false;
    }

    switch (keycode) {
        case KC_LOWER:
            if (record->event.pressed) {
                layer_on(_NUMS);
            } else {
                layer_off(_NUMS);
            }
            return false;
        case KC_RAISE:
            if (record->event.pressed) {
                layer_on(_ARROWS);
            } else {
                layer_off(_ARROWS);
            }
            return false;
        case EMOJI_HANDS:
        case EMOJI_LAUGH:
        case EMOJI_HEART:
            if (record->event.pressed) {
                if (is_apple) {
                    tap_code16(LCTL(LGUI(KC_SPC)));
                } else {
                    tap_code16(LGUI(KC_DOT));
                }
                wait_ms(200);
                if (keycode == EMOJI_HANDS) {
                    SEND_STRING("raised hands");
                } else if (keycode == EMOJI_LAUGH) {
                    SEND_STRING("joy");
                } else {
                    SEND_STRING("heart");
                }
                wait_ms(50);
                tap_code(KC_ENT);
            }
            return false;
        case TOGGLE_GAMING:
            if (record->event.pressed) {
                layer_on(_GAME);
            }
            return false;
        case EXIT_GAMING:
            if (record->event.pressed) {
                layer_off(_GAME);
                chat_mode_active = false;
            }
            return false;
        case CHAT_MODE:
            if (record->event.pressed) {
                chat_mode_active = true;
                layer_off(_GAME);
            }
            return false;
        case CAP_AREA:
            if (record->event.pressed) {
                tap_code16(LGUI(LCTL(LSFT(KC_4))));
            }
            return false;
        case CAP_DESK:
            if (record->event.pressed) {
                tap_code16(LGUI(LCTL(LSFT(KC_3))));
            }
            return false;
        case REC_SCRN:
            if (record->event.pressed) {
                tap_code16(LGUI(LSFT(KC_5)));
            }
            return false;
    }

    if (keycode == KC_LSFT) {
        left_shift_pressed = record->event.pressed;
        if (!record->event.pressed) {
            caps_toggle_triggered = false;
        }
    } else if (keycode == KC_RSFT) {
        right_shift_pressed = record->event.pressed;
        if (!record->event.pressed) {
            caps_toggle_triggered = false;
        }
    }

    if (left_shift_pressed && right_shift_pressed && !caps_toggle_triggered) {
        caps_toggle_triggered = true;
        tap_code(KC_CAPS);
        return false;
    }

    return true;
}

bool process_detected_host_os_user(os_variant_t detected_os) {
    is_apple = detected_os == OS_MACOS || detected_os == OS_IOS;
    return true;
}

#ifdef ENCODER_ENABLE

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) {
        if (clockwise) {
            tap_code(KC_VOLU);
        } else {
            tap_code(KC_VOLD);
        }
    } else if (index == 1) {
        if (clockwise) {
            tap_code(KC_PGDN);
        } else {
            tap_code(KC_PGUP);
        }
    }
    return true;
}

#endif
