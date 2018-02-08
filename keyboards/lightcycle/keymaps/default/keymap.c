#include "lightcycle.h"
#include "debug.h"
#include "action_layer.h"
#include "version.h"
#include "quantum.h"

#define BASE 0 // default layer
#define NUMB 1 // numbers
#define SYMB 2 // symbols

enum custom_keycodes {
  PLACEHOLDER = SAFE_RANGE, // ensure these codes start after the highest keycode defined in Quantum
  VRSN
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* Keymap 0: Basic Layer
*
* ,------+------+------+------+------+------.                    ,------+------+------+------+------+------.
* | ESC  |   '   |   ,  |   .  |   P  |   Y  |                    |   F  |   G  |   C  |   R  |   L  |  /   |
* |------+------+------+------+------+------|                    |------+------+------+------+------+-------|
* | TAB  |   A   |   O  |   E  |   U  |   I  |                    |   D  |   H  |   T  |   N  |   S  |  -   |
* |------+------+------+------+------+------|                    |------+------+------+------+------+-------|
* |LShift|   ;   |   Q  |   J  |   K  |   X  |                    |   B  |   M  |   W  |   V  |   Z  |RShift|
* |------+------+------+------+------+------'                    `------+------+------+------+------+-------|
* | App  | LCtrl | LAlt | Left | Right|                                  |  Up  | Down |   [  |   ]  | RAlt |
* `----------------------------------'                                  `-----------------------------------'
*                                      ,-------------.   ,--------------.
*                                      |       |      |  | Home | End   |
*                               ,------|  PgUp | PgDn |  |      |       |-------.
*                               | Back |-------|      |  |      |-------|       |
*                               | Space|       | ---- |  |------|       | Space |
*                               |      |  Del  |  ~1  |  |  ~2  | Enter |       |
*                               |      |       |      |  |      |       |       |
*                               `--------------------'   `---------------------'
*
*/

[BASE] = LAYOUT_lightcycle(  // layer 0 : default
        // left hand
          KC_ESC,       KC_QUOT,   KC_COMM,  KC_DOT,   KC_P,    KC_Y,                     
          KC_TAB,       KC_A,      KC_O,     KC_E,     KC_U,    KC_I,                     
          KC_LSFT,      KC_SCLN,   KC_Q,     KC_J,     KC_K,    KC_X,                     
          KC_LCTL,      KC_LGUI,  KC_LALT,  KC_LEFT,  KC_RGHT,                           
                                                                      KC_PGUP,  KC_PGDN,  
                                                             KC_BSPC, KC_DEL,   MO(NUMB), 
                                                                                          
        // right hand                                                                     
                  KC_F,         KC_G,     KC_C,     KC_R,     KC_L,     KC_SLSH,          
                  KC_D,         KC_H,     KC_T,     KC_N,     KC_S,     KC_MINS,          
                  KC_B,         KC_M,     KC_W,     KC_V,     KC_Z,     KC_RSFT,          
                                KC_UP,    KC_DOWN,  KC_LBRC,  KC_RBRC,  KC_RALT,          
        KC_HOME,  KC_END,                                                                 
        MO(SYMB), KC_ENT, KC_SPC                                                          
    ),

/* Keymap 1: Number Layer
 *
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |Versn |  F1  |  F2  |  F3  |  F4  |      |                    |  "   |   1  |   2  |   3  |   :  |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |  F5  |  F6  |  F7  |  F8  |      |                    |  \   |   4  |   5  |   6  |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |  F9  |  F10 |  F11 |  F12 |      |                    |   _  |   7  |   8  |   9  |   ?  |      |
 * |------+------+------+------+------+------'                    `------+------+------+------+------+------|
 * |RESET |      |      |      |      |                                  | BACK |  FWD |   {  |   }  |      |
 * `----------------------------------'                                  `----------------------------------'
 *                                      ,-------------.  ,-------------.
 *                                      |      |      |  |      | RCtrl|
 *                               ,------|      |      |  | RAlt |      |------.
 *                               |      |------|      |  |      |------|      |
 *                               |      |      |------|  |------|      |      |
                                 |      |      |      |  |      | RShft|  0   |
 *                               |      |      |      |  | Menu |      |      |
 *                               |      |      |      |  |      |      |      |
 *                               `--------------------'  `--------------------'
 */
// Numbers
[NUMB] = LAYOUT_lightcycle(
       // left hand
          VRSN,    KC_F1,   KC_F2,    KC_F3,    KC_F4,    KC_TRNS,
          KC_TRNS, KC_F5,   KC_F6,    KC_F7,    KC_F8,    KC_TRNS,
          KC_TRNS, KC_F9,   KC_F10,   KC_F11,   KC_F12,   KC_TRNS,
          RESET,   KC_TRNS, KC_TRNS,  KC_TRNS,  KC_TRNS,
                                                         KC_TRNS,  KC_TRNS,
                                               KC_TRNS,  KC_TRNS,  KC_TRNS,
       // right hand
                KC_DQT,     KC_1,    KC_2,    KC_3,    KC_COLN,   KC_TRNS,
                KC_SLSH,    KC_4,    KC_5,    KC_6,    KC_TRNS,   KC_TRNS,
                KC_UNDS,    KC_7,    KC_8,    KC_9,    KC_QUES,   KC_TRNS,
                            KC_WBAK, KC_WFWD, KC_LCBR, KC_RCBR,  KC_TRNS,
      KC_RALT,  KC_RCTL,
      KC_APP,  KC_RSFT,  KC_0
),

/* Keymap 2: Symbol Layer
 *
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |  !   |  @   |  #   |  $   |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |  %   |  ^   |  +   |  =   |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |  &   |  *   |  (   |  )   |                    |      |      |      |      |      |      |
 * |------------+------+------+------+------+'                    `------+------+------+------+------+------|
 * |      |      |  `   |  ~   |  |   |                                  |      |      |      |      |      |
 * `----------------------------------                                  `----------------------------------'
 *                                      ,-------------.  ,-------------.
 *                                      | break|      |  |      |      |
 *                               ,------|      | Togl |  |      |      |------.
 *                               |      |------| BL   |  |      |------|      |
 *                               |      |      |------|  |------|      |      |
                                 |  Caps|  Prt |      |  |      |      |      |
 *                               |  Lock|  Scr |      |  |      |      |      |
 *                               |      |      |      |  |      |      |      |
 *                               `--------------------'  `--------------------'
 */
// SYMBOLS
[SYMB] = LAYOUT_lightcycle(
       // left hand
          KC_TRNS,  KC_TRNS,   KC_EXLM,  KC_AT,    KC_HASH,  KC_DLR,
          KC_TRNS,  KC_TRNS,   KC_PERC,  KC_CIRC,  KC_PLUS,  KC_TRNS,
          KC_TRNS,  KC_TRNS,   KC_AMPR,  KC_ASTR,  KC_LPRN,  KC_RPRN,
          KC_TRNS,  KC_TRNS,   KC_GRV,   KC_TILD,  KC_PIPE,
                                                         KC_PAUSE, BL_TOGG,
                                               KC_CAPS,  KC_PSCR,  KC_TRNS,
       // right hand
                KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                         KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
      KC_TRNS,  KC_TRNS,
      KC_TRNS,  KC_TRNS,  KC_TRNS
),

};

const uint16_t PROGMEM fn_actions[] = {
    //[1] = ACTION_LAYER_TAP_TOGGLE(SYMB)                // FN1 - Momentary Layer 1 (Symbols)
};

const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt)
{
  // MACRODOWN only works in this function
  switch(id) {
    case 0:
      if (record->event.pressed) {
        SEND_STRING (QMK_KEYBOARD "/" QMK_KEYMAP " @ " QMK_VERSION);
      }
      break;
    case 1:
      if (record->event.pressed) { // For resetting EEPROM
        eeconfig_init();
      }
      break;
  }
  return MACRO_NONE;
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case VRSN:
      if (record->event.pressed) {
        SEND_STRING (QMK_KEYBOARD "/" QMK_KEYMAP " @ " QMK_VERSION);
      }
      return false;
      break;
  }
  return true;
}

// Runs just one time when the keyboard initializes.
void matrix_init_user(void) {};


// Runs constantly in the background, in a loop.
void matrix_scan_user(void) {};
