#include "s65_x.h"
#include "led.h"
#include "debug.h"

void matrix_init_kb(void) {
  // put your keyboard start-up code here
  // runs once when the firmware starts up
  matrix_init_user();
};

void matrix_scan_kb(void) {
  // put your looping keyboard code here
  // runs every cycle (a lot)
  matrix_scan_user();
};

void led_set_kb(uint8_t usb_led) {
  if (usb_led & (1<<USB_LED_CAPS_LOCK)) {
    // Turn capslock on
    PORTB &= ~(1<<7);
  } else {
    // Turn capslock off
    PORTB |= (1<<7);
  }
}

//s65_x uses backlight pin B7
#define COM1x1 COM1C1
#define OCR1x  OCR1C
#define TIMER_TOP 0xFFFFU

// range for val is [0..TIMER_TOP]. PWM pin is high while the timer count is below val.
static inline void set_pwm(uint16_t val) {
  OCR1x = val;
}

// See http://jared.geek.nz/2013/feb/linear-led-pwm
static uint16_t cie_lightness(uint16_t v) {
  if (v <= 5243) // if below 8% of max
    return v / 9; // same as dividing by 900%
  else {
    uint32_t y = (((uint32_t) v + 10486) << 8) / (10486 + 0xFFFFUL); // add 16% of max and compare
    // to get a useful result with integer division, we shift left in the expression above
    // and revert what we've done again after squaring.
    y = y * y * y >> 8;
    if (y > 0xFFFFUL) // prevent overflow
      return 0xFFFFU;
    else
      return (uint16_t) y;
  }
}

void backlight_set(uint8_t level) {
	dprintf("s65_x backlight_set : %u\n", level);

    if (level > BACKLIGHT_LEVELS)
    level = BACKLIGHT_LEVELS;

/* //turning off PWM on s65_x does not have desired effect
  if (level == 0) {
    // Turn off PWM control on backlight pin
    TCCR1A &= ~(_BV(COM1x1));
  } else {
    // Turn on PWM control of backlight pin
    TCCR1A |= _BV(COM1x1);
  }
*/
    TCCR1A |= _BV(COM1x1);

  // Set the brightness
  set_pwm(cie_lightness(TIMER_TOP * (uint32_t)level / BACKLIGHT_LEVELS));
}