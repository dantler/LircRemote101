/* LircRemote101.h - Header for infrared bit banger on Arduino101
 *
 * Copyright (C) 2016 David Antler
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#ifndef __LIRC_REMOTE_H__
#define __LIRC_REMOTE_H__

/** LRD_ prefix means Lirc Remote Description.  These are a mask that must be
 *  placed in the "descriptions" part of the LircRemote struct.
 */
#define LRD_HEADER        0x0001
#define LRD_ONE           0x0002
#define LRD_ZERO          0x0004
#define LRD_PTRAIL        0x0008
#define LRD_REPEAT        0x0010
#define LRD_GAP           0x0020
#define LRD_TOGBITMASK    0x0040
#define LRD_FLAGS         0x0080
#define LRD_PREDATA       0x0100
#define LRD_FOOT          0x0200
#define LRD_DUTYCYCLE     0x0400
#define LRD_FREQUENCY     0x0800
#define LRD_PRELEAD       0x1000

typedef struct _LircRemote {
  unsigned long  descriptions;
  unsigned short bits;
  unsigned short header[2];
  unsigned short one[2];
  unsigned short zero[2];
  unsigned short ptrail;
  unsigned short repeat[2];
  unsigned long  toggle_bit_mask;
  unsigned long  button_codes[10];
} LircRemote;

void sendRemoteCode(const LircRemote* remote, int codeIndex, int pin);
void sendRawCode (const unsigned short buf[],  unsigned short len, int pin);

#endif

