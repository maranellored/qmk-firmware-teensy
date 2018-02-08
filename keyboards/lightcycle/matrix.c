/*

Copyright 2013 Oleg Kostyuk <cub.uanic@gmail.com>
Copyright 2017 Erin Call <hello@erincall.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * scan matrix
 */
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include "wait.h"
#include "action_layer.h"
#include "print.h"
#include "debug.h"
#include "util.h"
#include "matrix.h"
#include "lightcycle.h"
#include "i2cmaster.h"
#ifdef DEBUG_MATRIX_SCAN_RATE
#include  "timer.h"
#endif

/*
 * This constant define not debouncing time in msecs, but amount of matrix
 * scan loops which should be made to get stable debounced results.
 *
 * On the Dactyl, the matrix scan rate is relatively low, because
 * communicating with the left hand's I/O expander is slower than simply
 * selecting local pins.
 * Now it's only 317 scans/second, or about 3.15 msec/scan.
 * According to Cherry specs, debouncing time is 5 msec.
 *
 * And so, there is no sense to have DEBOUNCE higher than 2.
 */

#ifndef DEBOUNCE
#   define DEBOUNCE	5
#endif

/* matrix state(1:on, 0:off) */
static uint16_t matrix[MATRIX_ROWS];

// Debouncing: store for each key the number of scans until it's eligible to
// change.  When scanning the matrix, ignore any changes in keys that have
// already changed in the last DEBOUNCE scans.
static uint8_t debounce_matrix[MATRIX_ROWS*MATRIX_COLS];

static uint16_t read_cols(void);
static void select_row(uint8_t row);
static void unselect_rows(void);

static uint8_t mcp23018_reset_loop;

__attribute__ ((weak))
void matrix_init_user(void) {}

__attribute__ ((weak))
void matrix_scan_user(void) {}

__attribute__ ((weak))
void matrix_init_kb(void) {
  matrix_init_user();
}

__attribute__ ((weak))
void matrix_scan_kb(void) {
  matrix_scan_user();
}

inline
uint8_t matrix_rows(void) {return MATRIX_ROWS;}

inline
uint8_t matrix_cols(void) {return MATRIX_COLS;}

void matrix_init(void)
{
    mcp23018_status = init_lightcycle();

    unselect_rows();

    // initialize matrix state
    for (uint8_t i=0; i < MATRIX_ROWS; i++)
        matrix[i] = 0;

    // Inintialize Debounce Matrix
    for (uint8_t i=0; i < MATRIX_ROWS*MATRIX_COLS; i++)
        debounce_matrix[i] = 0;

    matrix_init_quantum();
}

void matrix_power_up(void)
{
    matrix_init();
}

// Returns a uint16_t whose bits are set if the corresponding key should be
// eligible to change in this scan.
uint8_t debounce_mask(uint16_t row)
{
  uint8_t result = 0;
  for (uint8_t i=0; i < MATRIX_ROWS; i++)
  {
    if (debounce_matrix[row * MATRIX_COLS + i])
      debounce_matrix[row * MATRIX_COLS + i]--;
    else
      result |= (1 << i);
  }
  return result;
}

// Report changed keys in the given row.  Resets the debounce countdowns
// corresponding to each set bit in 'change' to DEBOUNCE.
void debounce_report(uint16_t change, uint16_t row)
{
    for (uint8_t i = 0; i < MATRIX_ROWS; i++)
    {
        if (change & (1 << i))
        {
          debounce_matrix[row * MATRIX_COLS + i] = DEBOUNCE;
        }
    }
}

uint8_t matrix_scan(void)
{
    if (mcp23018_status)
    { // if there was an error
        if (++mcp23018_reset_loop == 0)
        {
            // since mcp23018_reset_loop is 8 bit - we'll try to reset once in 255 matrix scans
            // this will be approx bit more frequent than once per second
            print("trying to reset mcp23018\n");
            mcp23018_status = init_mcp23018();

            if (mcp23018_status)
                print("left side not responding\n");
            else
                print("left side attached\n");
        }
    }

    for(uint8_t i=0; i < MATRIX_ROWS; i++)
    {
        select_row(i);
        wait_us(30);
        uint16_t col_data = read_cols();
        if(i==4) {print("Row: "); phex(i); print("Column: "); phex16(col_data); print("\n");}
        //uint16_t mask = debounce_mask(i);
        //uint16_t cols = (read_cols() & mask) | (matrix[i] & ~mask);
        //debounce_report(cols ^ matrix[i], i);
        matrix[i] = col_data;
        unselect_rows();
    }

    matrix_scan_quantum();

    return 1;
}

bool matrix_is_modified(void) // deprecated and evidently not called.
{
    return true;
}

inline
bool matrix_is_on(uint8_t row, uint8_t col)
{
    return (matrix[row] & ((uint16_t)1<<col));
}

inline
uint16_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

void matrix_print(void)
{
    print("\nr/c 0123456789ABCDEF\n");
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        phex(row); print(": ");
        pbin_reverse16(matrix_get_row(row));
        print("\n");
    }
}

uint8_t matrix_key_count(void)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        count += bitpop16(matrix[i]);
    }
    return count;
}


static uint16_t read_cols(void)
{
    // Columns 0-5 are on the MCP, 6-B are on the teensy
    // Read from MCP and Teensy
    uint8_t mcp_data = 0;
    uint8_t teensy_data = 0;
    uint16_t column_data = 0;

    if (!mcp23018_status)
    {
        mcp23018_status = i2c_start(I2C_ADDR_WRITE);    if (mcp23018_status) goto out;
        mcp23018_status = i2c_write(GPIOA);             if (mcp23018_status) goto out;
        mcp23018_status = i2c_start(I2C_ADDR_READ);     if (mcp23018_status) goto out;
        mcp_data = i2c_readNak();
        mcp_data = (~mcp_data) >> 1;
    out:
        i2c_stop();
    }

    // Read Teensy data
    teensy_data = ~((PINB & 0x0F) | ((PIND & 0x0C)<<2));

    column_data = ((teensy_data << 6) | (mcp_data & 0x3F)) & 0x0FFF;
    return column_data;
}


static void unselect_rows(void)
{
    // unselect on mcp23018
    if (!mcp23018_status)
    {
        // set all rows to drive high
        mcp23018_status = i2c_start(I2C_ADDR_WRITE); if (mcp23018_status) goto out;
        mcp23018_status = i2c_write(GPIOB);          if (mcp23018_status) goto out;
        mcp23018_status = i2c_write(0xFF);
    out:
        i2c_stop();
    }

    // Unselect on Teensy 2.0
    PORTF |=  (1<<0 | 1<<1 | 1<<4 | 1<<5 | 1<<6);
}


static void select_row(uint8_t row)
{
    // Drive row low on both the mcp and the teensy
    // select on mcp23018
    if (!mcp23018_status)
    {
        // set active row low and all other rows high
        mcp23018_status = i2c_start(I2C_ADDR_WRITE);    if (mcp23018_status) goto out;
        mcp23018_status = i2c_write(GPIOB);             if (mcp23018_status) goto out;
        mcp23018_status = i2c_write(0xFF & ~(1<<row));
    out:
        i2c_stop();
    }

    // Select on Teensy 2.0
    PORTF &= (row < 2) ? ~(1<<row) : ~(1<<(row + 2));
}

