#include QMK_KEYBOARD_H
#include "enums.h"
#include "os_detection.h"

// OS detection state
static bool isApple = false;

// Tap dance state for ESC/TAB/FN layer

#define XXXXXXX KC_NO

// Caps lock toggle state tracking
static bool left_shift_pressed = false;
static bool right_shift_pressed = false;
static bool caps_toggle_triggered = false;

// Chat mode state tracking
static bool chat_mode_active = false;

#define KC_SLCK KC_SCROLL_LOCK

#define RALT_X RALT_T(KC_X)
#define RALT_DOT RALT_T(KC_DOT)

enum custom_keycodes {
    RESET_KBD = SAFE_RANGE,
    TOGGLE_LAYOUT,
    BIT_0,
    BIT_1,
    BIT_2,
    BIT_3,
    BIT_4,
    // Emoji keycodes
    EMOJI_HANDS,     // 🙌🏼
    EMOJI_LAUGH,     // 😂
    EMOJI_HEART,     // ❤️
    SYM_EURO,     // €
    CAPS_TOGGLE,     // Custom caps lock toggle
    // Additional symbols for numbers layer
    SYM_ARROW,    // =>
    SYM_PERCENT,  // %
    SYM_CARET,    // ^
    SYM_DOLLAR,   // $
    TOGGLE_GAMING,   // Toggle gaming layer on
    EXIT_GAMING,     // Exit gaming layer
    CHAT_MODE,       // Temporary chat mode in gaming
    // OS-aware copy/paste
    OS_COPY,  // OS-aware copy
    OS_PASTE, // OS-aware paste
    // Screenshot and screen recording
    CAP_AREA,    // Cmd+Ctrl+Shift+4 - Capture area
    CAP_DESK, // Cmd+Ctrl+Shift+3 - Capture desktop
    REC_SCRN,   // Cmd+Shift+5 - Record screen
};

// QMK Native Combo Definitions
// Define combo key sequences for QWERTY layout

// Parentheses combos
const uint16_t PROGMEM combo_lparen[] = {KC_D, KC_F, COMBO_END}; // Left parenthesis (
const uint16_t PROGMEM combo_rparen[] = {KC_J, KC_K, COMBO_END}; // Right parenthesis )

// Brackets combos
const uint16_t PROGMEM combo_lbracket[] = {KC_E, KC_R, COMBO_END}; // Left bracket [
const uint16_t PROGMEM combo_rbracket[] = {KC_U, KC_I, COMBO_END}; // Right bracket ]

// Braces combos
const uint16_t PROGMEM combo_lbrace[] = {KC_C, KC_V, COMBO_END}; // Left brace {
const uint16_t PROGMEM combo_rbrace[] = {KC_M, KC_COMM, COMBO_END}; // Right brace }

// Comparison operators
const uint16_t PROGMEM combo_less[] = {KC_W, KC_E, COMBO_END}; // Less than <
const uint16_t PROGMEM combo_greater[] = {KC_I, KC_O, COMBO_END}; // Greater than >

// Special characters
const uint16_t PROGMEM combo_tilde[] = {KC_A, KC_S, COMBO_END}; // Tilde ~
const uint16_t PROGMEM combo_grave[] = {KC_S, KC_D, COMBO_END}; // Grave `

// Math operators
const uint16_t PROGMEM combo_plus[] = {KC_R, KC_U, COMBO_END}; // Plus +
const uint16_t PROGMEM combo_minus[] = {KC_V, KC_M, COMBO_END}; // Minus -
const uint16_t PROGMEM combo_equal[] = {KC_F, KC_J, COMBO_END}; // Equal =
const uint16_t PROGMEM combo_asterisk[] = {KC_E, KC_I, COMBO_END}; // Asterisk *
const uint16_t PROGMEM combo_underscore[] = {KC_R, KC_T, COMBO_END}; // Underscore _

// Special functions
const uint16_t PROGMEM combo_word_del[] = {KC_4, KC_5, COMBO_END}; // Word delete (Alt+Backspace)

// Emoji combos
const uint16_t PROGMEM combo_emoji_hands[] = {KC_T, KC_Y, COMBO_END}; // Raising hands emoji 🙌🏼
const uint16_t PROGMEM combo_emoji_laugh[] = {KC_G, KC_H, COMBO_END}; // Laughing emoji 😂
const uint16_t PROGMEM combo_emoji_heart[] = {KC_B, KC_N, COMBO_END}; // Heart emoji ❤️

// Additional symbol combos
const uint16_t PROGMEM combo_exclamation[] = {KC_Y, KC_U, COMBO_END}; // Exclamation mark !
const uint16_t PROGMEM combo_ampersand[] = {KC_D, KC_K, COMBO_END}; // Ampersand &
const uint16_t PROGMEM combo_dollar[] = {KC_S, KC_L, COMBO_END}; // Dollar sign $
const uint16_t PROGMEM combo_hash[] = {KC_F, KC_G, COMBO_END}; // Hash symbol #
const uint16_t PROGMEM combo_at[] = {KC_H, KC_J, COMBO_END}; // At symbol @
const uint16_t PROGMEM combo_percent[] = {KC_V, KC_B, COMBO_END}; // Percent symbol %
const uint16_t PROGMEM combo_caret[] = {KC_N, KC_M, COMBO_END}; // Caret symbol ^
const uint16_t PROGMEM combo_backslash[] = {KC_K, KC_L, COMBO_END}; // Backslash
const uint16_t PROGMEM combo_pipe[] = {KC_L, KC_SCLN, COMBO_END}; // Pipe |

// ═══════════════════════════════════════════════════════════════════════════
// BITWISE FUNCTION KEY INPUT IMPLEMENTATION
// ═══════════════════════════════════════════════════════════════════════════

// Bitwise F-key state tracking
static int8_t bitwise_pressed_keys = 0;
static uint8_t bitwise_accumulator = 0;

// Define which keys act as bit inputs on the function layer (left home row)
// We'll use the BIT_0 through BIT_4 keycodes from the custom enum above
const uint16_t bitwise_f_keys[] = { BIT_0, BIT_1, BIT_2, BIT_3, BIT_4 };
uint8_t NUM_BITWISE_F_KEYS = sizeof(bitwise_f_keys) / sizeof(uint16_t);

// Function key mapping array (F1-F24)
uint16_t F_KEYS[] = {
    KC_NO,   // 0 - no function key
    KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,
    KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_F13,  KC_F14,  KC_F15,  KC_F16,
    KC_F17,  KC_F18,  KC_F19,  KC_F20,  KC_F21,  KC_F22,  KC_F23,  KC_F24,
    KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO    // 25-31 unused
};

// Process bitwise function key input
bool process_bitwise_f(uint16_t keycode, keyrecord_t *record) {
    // Check if we're on the function layer
    if (get_highest_layer(layer_state) != _F) {
        return true; // Not on function layer, let normal processing continue
    }

    // Find if this keycode is one of our bitwise keys
    int index = -1;
    for (int i = 0; i < NUM_BITWISE_F_KEYS; ++i) {
        if (keycode == bitwise_f_keys[i]) {
            index = i;
            break;
        }
    }

    // If not a bitwise key, let normal processing continue
    if (index == -1) {
        return true;
    }

    // Handle bitwise key press/release
    if (record->event.pressed) {
        bitwise_pressed_keys++;
        bitwise_accumulator |= (1 << index);
    } else {
        bitwise_pressed_keys--;
        if (bitwise_pressed_keys <= 0) {
            bitwise_pressed_keys = 0;
            // Send the function key if accumulator is in valid range
            if (bitwise_accumulator > 0 && bitwise_accumulator < sizeof(F_KEYS)/sizeof(uint16_t)) {
                if (F_KEYS[bitwise_accumulator] != KC_NO) {
                    tap_code(F_KEYS[bitwise_accumulator]);
                }
            }
            bitwise_accumulator = 0;
        }
    }

    return false; // Don't process this key normally
}

// Define the combo arrays for QMK native combo system
combo_t key_combos[] = {
    // QWERTY layout combos
    COMBO(combo_lparen, LSFT(KC_9)),           // Left parenthesis (
    COMBO(combo_rparen, LSFT(KC_0)),           // Right parenthesis )
    COMBO(combo_lbracket, KC_LBRC),             // Left bracket [
    COMBO(combo_rbracket, KC_RBRC),             // Right bracket ]
    COMBO(combo_lbrace, LSFT(KC_LBRC)),         // Left brace {
    COMBO(combo_rbrace, LSFT(KC_RBRC)),         // Right brace }
    COMBO(combo_less, LSFT(KC_COMM)),           // Less than <
    COMBO(combo_greater, LSFT(KC_DOT)),         // Greater than >
    COMBO(combo_tilde, LSFT(KC_GRV)),           // Tilde ~
    COMBO(combo_grave, KC_GRV),                 // Grave `
    COMBO(combo_plus, LSFT(KC_EQL)),            // Plus +
    COMBO(combo_minus, KC_MINS),                // Minus -
    COMBO(combo_equal, KC_EQL),                 // Equal =
    COMBO(combo_asterisk, LSFT(KC_8)),          // Asterisk *
    COMBO(combo_underscore, LSFT(KC_MINS)),     // Underscore _
    COMBO(combo_word_del, LALT(KC_BSPC)),       // Word delete (Alt+Backspace)
    COMBO(combo_emoji_hands, EMOJI_HANDS),      // Raising hands emoji 🙌🏼
    COMBO(combo_emoji_laugh, EMOJI_LAUGH),      // Laughing emoji 😂
    COMBO(combo_emoji_heart, EMOJI_HEART),      // Heart emoji ❤️
    COMBO(combo_exclamation, LSFT(KC_1)),       // Exclamation mark !
    COMBO(combo_ampersand, LSFT(KC_7)),         // Ampersand &
    COMBO(combo_dollar, LSFT(KC_4)),            // Dollar sign $
    COMBO(combo_hash, LSFT(KC_3)),              // Hash symbol #
    COMBO(combo_at, LSFT(KC_2)),                // At symbol @
    COMBO(combo_percent, LSFT(KC_5)),           // Percent symbol %
    COMBO(combo_caret, LSFT(KC_6)),             // Caret symbol ^
    COMBO(combo_backslash, KC_BSLS),            // Backslash
    COMBO(combo_pipe, LSFT(KC_BSLS)),           // Pipe |
};

uint16_t COMBO_LEN = ARRAY_SIZE(key_combos);

// Disable combos when gaming layer is active
bool combo_should_trigger(uint16_t combo_index, combo_t *combo, uint16_t keycode, keyrecord_t *record) {
    // Disable all combos on gaming layer
    if (layer_state_is(_GAME)) {
        return false;
    }
    return true;
}

// ═══════════════════════════════════════════════════════════════════════════

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // ═══════════════════════════════════════════════════════════════════════════
    // LAYER 0: QWERTY BASE LAYER
    // ═══════════════════════════════════════════════════════════════════════════
    [0] = LAYOUT_split_3x6_3(
    //,-----------------------------------------------------.                    ,-----------------------------------------------------.
        KC_TAB,   KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,   KC_U,    KC_I,    KC_O,    KC_P,   KC_BSPC,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        KC_LCTL,  KC_A,    KC_S,    KC_D,    KC_F,  KC_G,                    KC_H,   KC_J,    KC_K,    KC_L,   KC_SCLN,  KC_QUOT,
    //|--------+--------+--------+--------+-------+--------|                    |--------+--------+--------+--------+--------+--------|
        KC_LSFT,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                         KC_N,   KC_M,   KC_COMM, KC_DOT,  KC_SLSH, RSFT_T(KC_ESC),
    //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                    KC_LCTL, KC_LGUI, LT(2, KC_SPC),      KC_ENT, MO(1), KC_RALT
                                       //`--------------------------'  `--------------------------'
    ),
    // ═══════════════════════════════════════════════════════════════════════════
    // LAYER 1: NAVIGATION / MEDIA LAYER 
    // ═══════════════════════════════════════════════════════════════════════════
    [1] = LAYOUT_split_3x6_3(
    //,-----------------------------------------------------.                    ,-----------------------------------------------------.
        KC_NO,   KC_NO,   KC_VOLD, KC_MUTE, KC_VOLU, BL_UP,                        KC_HOME, KC_PGDN, KC_PGUP,  KC_END, _______, _______,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        _______, _______, KC_MPRV, KC_MPLY, KC_MNXT, BL_DOWN,                      KC_LEFT, KC_DOWN,  KC_UP,  KC_RGHT, _______, _______,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        _______, _______, _______, KC_COPY, KC_PSTE, _______,                      KC_INS, KC_PSCR, KC_SCRL, KC_PAUS,  _______, _______,
    //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                            _______, _______, _______,    _______, _______, _______
                                        //`--------------------------'  `--------------------------'
    ),

    // ═══════════════════════════════════════════════════════════════════════════
    // LAYER 2: NUMBERS / SYMBOLS LAYER
    // ═══════════════════════════════════════════════════════════════════════════
    [2] = LAYOUT_split_3x6_3(
    //,-----------------------------------------------------.                    ,-----------------------------------------------------.
        _______, SYM_PERCENT, SYM_CARET, SYM_EURO, SYM_DOLLAR, SYM_ARROW,          KC_MINS,   KC_7,    KC_8,    KC_9,  KC_EQL, _______,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      KC_PLUS,   KC_4,    KC_5,    KC_6, KC_ASTR, XXXXXXX,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                     KC_EQL,    KC_1,    KC_2,    KC_3, KC_SLSH, _______,
    //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                            _______, _______, _______,       LT(3, KC_ENT), KC_0, _______
                                        //`--------------------------'  `--------------------------'
    ),

    // ═══════════════════════════════════════════════════════════════════════════
    // LAYER 4: FUNCTION KEYS LAYER (BITWISE INPUT)
    // ═══════════════════════════════════════════════════════════════════════════
    // Left home row uses bitwise input: A(1) + S(2) + D(4) + F(8) + G(16) = F1-F24
    // Example: A+D = 1+4 = 5 = F5, S+F = 2+8 = 10 = F10, etc.
    [3] = LAYOUT_split_3x6_3(
    //,-----------------------------------------------------.                    ,-----------------------------------------------------.
        _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                     XXXXXXX, CAP_DESK, XXXXXXX, XXXXXXX, XXXXXXX, TOGGLE_LAYOUT,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        _______, BIT_0,   BIT_1,   BIT_2,   BIT_3,   BIT_4,                       XXXXXXX, CAP_AREA, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                     XXXXXXX, REC_SCRN, XXXXXXX, XXXXXXX, XXXXXXX, TOGGLE_GAMING,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
                                            _______, _______, _______,    _______, _______, _______
                                        //`--------------------------'  `--------------------------'
    ),

    // ═══════════════════════════════════════════════════════════════════════════
    // LAYER 5: GAMING LAYER
    // ═══════════════════════════════════════════════════════════════════════════
    [4] = LAYOUT_split_3x6_3(
    //,-----------------------------------------------------.                    ,-----------------------------------------------------.
        KC_TAB,     KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,    KC_O,   KC_P,  KC_BSPC,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        KC_LSFT,    KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                         KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN, KC_QUOT,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        KC_LCTL,    KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                         KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH, EXIT_GAMING,
    //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                            KC_LALT,  KC_ESC,  KC_SPC,     KC_ENT, KC_BSPC, CHAT_MODE
                                        //`--------------------------'  `--------------------------'
    ),
    
    // ═══════════════════════════════════════════════════════════════════════════
    // LAYER 5: UTILS LAYER
    // ═══════════════════════════════════════════════════════════════════════════
    [5] = LAYOUT_split_3x6_3(
    //,-----------------------------------------------------.                    ,-----------------------------------------------------.
        _______,  _______,  _______,  _______,  _______,  _______,                  _______, _______, _______, _______, _______, _______,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        _______,  _______,  _______,  _______,  _______,  _______,                  _______, _______, _______, _______, _______, _______,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        _______,  _______,  _______,  _______,  _______,  _______,                  _______, _______, _______, _______, _______, _______,
    //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                            KC_RGUI,  _______,  _______,    _______, _______, _______
                                        //`--------------------------'  `--------------------------'
    )
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Handle Enter key in chat mode first - return to gaming
    // Only exit chat mode on regular Enter or when mod-tap is tapped (not held for shift)
    if (chat_mode_active && record->event.pressed) {
        if (keycode == KC_ENT) {
            // Regular Enter key
            chat_mode_active = false;
            layer_clear();
            layer_on(_GAME);
            return true;
        } else if (keycode == MT(MOD_LSFT, KC_ENT) && record->tap.count > 0) {
            // Mod-tap Enter key was tapped (not held for shift)
            chat_mode_active = false;
            layer_clear();
            layer_on(_GAME);
            return true;
        }
    }

    // Handle bitwise function key input
    if (!process_bitwise_f(keycode, record)) {
        return false;
    }

    switch (keycode) {
        case RESET_KBD:
            if (record->event.pressed) {
                soft_reset_keyboard();
            }
            return false;
        case TOGGLE_LAYOUT:
            // No longer used - QWERTY only
            return false;
        case SYM_EURO:
            if (record->event.pressed) {
                // Use Option+Shift+2 for Euro symbol on macOS
                register_mods(MOD_LALT | MOD_LSFT);
                register_code(KC_2);
                unregister_code(KC_2);
                unregister_mods(MOD_LALT | MOD_LSFT);
            }
            return false;
        case SYM_ARROW:
            if (record->event.pressed) {
                SEND_STRING("=>");  // Arrow symbol
            }
            return false;
        case SYM_PERCENT:
            if (record->event.pressed) {
                SEND_STRING("%");  // Percent symbol
            }
            return false;
        case SYM_CARET:
            if (record->event.pressed) {
                SEND_STRING("^");  // Caret symbol
            }
            return false;
        case SYM_DOLLAR:
            if (record->event.pressed) {
                SEND_STRING("$");  // Dollar symbol
            }
            return false;
        case EMOJI_HANDS:
            if (record->event.pressed) {
                // Open emoji picker and type "raised hands"
                register_mods(MOD_LCTL | MOD_LGUI);
                register_code(KC_SPC);
                unregister_code(KC_SPC);
                unregister_mods(MOD_LCTL | MOD_LGUI);
                wait_ms(200);
                SEND_STRING("raised hands");
                wait_ms(50);
                tap_code(KC_ENT);
            }
            return false;
        case EMOJI_LAUGH:
            if (record->event.pressed) {
                // Open emoji picker and type "joy"
                register_mods(MOD_LCTL | MOD_LGUI);
                register_code(KC_SPC);
                unregister_code(KC_SPC);
                unregister_mods(MOD_LCTL | MOD_LGUI);
                wait_ms(200);
                SEND_STRING("joy");
                wait_ms(50);
                tap_code(KC_ENT);
            }
            return false;
        case EMOJI_HEART:
            if (record->event.pressed) {
                // Open emoji picker and type "heart"
                register_mods(MOD_LCTL | MOD_LGUI);
                register_code(KC_SPC);
                unregister_code(KC_SPC);
                unregister_mods(MOD_LCTL | MOD_LGUI);
                wait_ms(200);
                SEND_STRING("heart");
                wait_ms(50);
                tap_code(KC_ENT);
            }
            return false;
        case CAPS_TOGGLE:
            if (record->event.pressed) {
                tap_code(KC_CAPS);  // Toggle caps lock
            }
            return false;
        case TOGGLE_GAMING:
            if (record->event.pressed) {
                layer_on(_GAME);  // Turn on gaming layer
            }
            return false;
        case EXIT_GAMING:
            if (record->event.pressed) {
                layer_off(_GAME);  // Turn off gaming layer
                chat_mode_active = false;  // Reset chat mode when exiting gaming
            }
            return false;
        case CHAT_MODE:
            if (record->event.pressed) {
                chat_mode_active = true;
                // Turn off gaming layer and switch to base layer
                layer_off(_GAME);
                layer_on(_QWERTY);
            }
            return false;
        case OS_COPY:
            if (record->event.pressed) {
                if (isApple) {
                    // Use Cmd+C for Apple devices
                    register_mods(MOD_LGUI);
                    register_code(KC_C);
                    unregister_code(KC_C);
                    unregister_mods(MOD_LGUI);
                } else {
                    // Use Ctrl+C for other devices
                    register_mods(MOD_LCTL);
                    register_code(KC_C);
                    unregister_code(KC_C);
                    unregister_mods(MOD_LCTL);
                }
            }
            return false;
        case OS_PASTE:
            if (record->event.pressed) {
                if (isApple) {
                    // Use Cmd+V for Apple devices
                    register_mods(MOD_LGUI);
                    register_code(KC_V);
                    unregister_code(KC_V);
                    unregister_mods(MOD_LGUI);
                } else {
                    // Use Ctrl+V for other devices
                    register_mods(MOD_LCTL);
                    register_code(KC_V);
                    unregister_code(KC_V);
                    unregister_mods(MOD_LCTL);
                }
            }
            return false;
        case CAP_AREA:
            if (record->event.pressed) {
                // Cmd+Ctrl+Shift+4 for area capture
                register_mods(MOD_LGUI | MOD_LCTL | MOD_LSFT);
                register_code(KC_4);
                unregister_code(KC_4);
                unregister_mods(MOD_LGUI | MOD_LCTL | MOD_LSFT);
            }
            return false;
        case CAP_DESK:
            if (record->event.pressed) {
                // Cmd+Ctrl+Shift+3 for desktop capture
                register_mods(MOD_LGUI | MOD_LCTL | MOD_LSFT);
                register_code(KC_3);
                unregister_code(KC_3);
                unregister_mods(MOD_LGUI | MOD_LCTL | MOD_LSFT);
            }
            return false;
        case REC_SCRN:
            if (record->event.pressed) {
                // Cmd+Shift+5 for screen recording
                register_mods(MOD_LGUI | MOD_LSFT);
                register_code(KC_5);
                unregister_code(KC_5);
                unregister_mods(MOD_LGUI | MOD_LSFT);
            }
            return false;
    }

    // Handle shift key tracking for caps lock toggle
    // Check for both shift keys being pressed (regular shift keys)
    if (keycode == KC_LSFT) {  // Left shift
        if (record->event.pressed) {
            left_shift_pressed = true;
        } else {
            left_shift_pressed = false;
            caps_toggle_triggered = false;  // Reset when releasing
        }
    } else if (keycode == KC_RSFT) {  // Right shift
        if (record->event.pressed) {
            right_shift_pressed = true;
        } else {
            right_shift_pressed = false;
            caps_toggle_triggered = false;  // Reset when releasing
        }
    }

    // Check if both shifts are pressed and we haven't triggered yet
    if (left_shift_pressed && right_shift_pressed && !caps_toggle_triggered) {
        caps_toggle_triggered = true;
        tap_code(KC_CAPS);  // Toggle caps lock
        return false;  // Don't process the key further
    }

    // Custom combo logic removed - now using QMK native combos

    return true;
}

bool wpm_keycode_user(uint16_t keycode) {
    if ((keycode >= QK_MOD_TAP && keycode <= QK_MOD_TAP_MAX) || (keycode >= QK_LAYER_TAP && keycode <= QK_LAYER_TAP_MAX) || (keycode >= QK_MODS && keycode <= QK_MODS_MAX)) {
        keycode = keycode & 0xFF;
    } else if (keycode > 0xFF) {
        keycode = 0;
    }
    if (keycode >= KC_A && keycode <= KC_0) {
        return true;
    }

    return false;
}

// OS detection callback - update isApple when OS is detected
bool process_detected_host_os_user(os_variant_t detected_os) {
    switch (detected_os) {
        case OS_MACOS:
        case OS_IOS:
            isApple = true;
            break;
        case OS_LINUX:
        case OS_WINDOWS:
        case OS_UNSURE:
        default:
            isApple = false;
            break;
    }
    return true;
}