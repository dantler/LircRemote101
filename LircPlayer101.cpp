/* LircPlayer101.c - Infrared bit banger on Arduino101
 *
 * Copyright (C) 2016 David Antler
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#include "Arduino.h"
#include "LircPlayer101.h"

#define INFRARED_HZ         38000
#define TOGGLES_PER_SECOND  (2 * INFRARED_HZ) /* Up AND down; e.g., one HZ would have 2 toggles per second */
#define US_PER_SEC          1000000
#define SECS_PER_US         (1/1000000)
#define TICKS_PER_US        32
#define TICKS_PER_TOGGLE    ( ( US_PER_SEC / TOGGLES_PER_SECOND ) * ( TICKS_PER_US ) )

// Fudge factor is to compensate for the loop latency.
// It's our magic number to get us down to 38KHz.
#define TICKS_PER_TOGGLE_FUDGE_FACTOR   100


/*****************   HELPER FUNCTIONS  ********************/

void delayMicrosecondsWithPulse(uint32_t duration, int pin)
{
  uint32_t toggleCount = ((uint32_t)(duration * TOGGLES_PER_SECOND) / US_PER_SEC)+1;
  bool GpioPinDriveHigh = true;

  while (toggleCount) {

    // Flip the pin we're bit-banging.

    digitalWrite(pin, GpioPinDriveHigh);
    GpioPinDriveHigh = !GpioPinDriveHigh;

    // Wait an appropriate amount of time.  We need to
    // subtract a "fudge factor" to accomodate for loop latency.
    // The "fudge factor" could be confirmed using an oscilliscope.
    // All I needed to do is see what number gives us 38KHz!

    delayTicks(TICKS_PER_TOGGLE - TICKS_PER_TOGGLE_FUDGE_FACTOR);
    toggleCount = toggleCount - 1;
  }

  digitalWrite(pin, LOW);
}

/*****************    API FUNCTIONS    ********************/

void sendRawCode (const unsigned short buf[],  unsigned short len, int pin)
{
  /* TODO: This function is extremely timing-sensitive and
   * should mask interrupts
   */
  unsigned short i;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);

  for (i = 0; i < len; i++)
  {
    if (i % 2)
    {
      // We're in a delay
      digitalWrite(pin, LOW);
      // Subtract one to compensate for loop latency.
      delayMicroseconds(buf[i] - 1);
    }
    else
    {
      // We're in a pulse
      delayMicrosecondsWithPulse(buf[i], pin);
    }
  }

  digitalWrite(pin, LOW);
}


#define LDR_UNSUPPORTED_MASK   (LRD_FREQUENCY | LRD_DUTYCYCLE | LRD_PREDATA | LRD_FOOT | LRD_PRELEAD)

#define PUSH_TO_BUF(val)  { buf[bufLen] = (val); bufLen++; }

/**
 * This function is based on my interpretation of the document found here:
 *
 *    http://lirc.org/html/lircd.conf.html
 *
 */
void sendRemoteCode(const LircRemote* remote, int codeIndex, int pin)
{
  unsigned short buf[128];
  unsigned char bufLen = 0;
  unsigned long mask;

  // Ensure all features are supported.  @todo: assert somehow.
  if (0 != (remote->descriptions & LDR_UNSUPPORTED_MASK))
  {
      return;
  }

  // @todo: ensure codeIndex is within the structure.

  // copy header
  if(remote->descriptions | LRD_HEADER)
  {
    PUSH_TO_BUF(remote->header[0]);
    PUSH_TO_BUF(remote->header[1]);
  }

  // @todo: copy plead

  // @todo: copy predata

  // @todo: copy pre

  // Format the button codes and send them.
  // We start at the most significant bit and work our way down to the LSB.
  for(mask = (1 << ((remote->bits)-1)); mask != 0; mask >>= 1)
  {
    if(mask & (remote->button_codes[codeIndex]))
    {
      // if '1', push one.
      PUSH_TO_BUF(remote->one[0]);
      PUSH_TO_BUF(remote->one[1]);
    }
    else
    {
      PUSH_TO_BUF(remote->zero[0]);
      PUSH_TO_BUF(remote->zero[1]);
    }

  }

  // @todo: copy 'post'

  // @todo: copy postdata

  // copy ptrail
  if(remote-> descriptions | LRD_PTRAIL)
  {
    PUSH_TO_BUF(remote->ptrail);
  }

  // ??? repeat? ???

  // @todo: copy 'foot'

  // @todo: copy 'gap'

  // @todo: assert no buffer overflow.

  // Send the code.
  sendRawCode(buf, bufLen, pin);
}

