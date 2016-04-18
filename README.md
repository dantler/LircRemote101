# LircRemote101

A project to enable infrared code playback via a single digital pin on
the [Arduino101](https://www.arduino.cc/en/Main/ArduinoBoard101) 
development board and a [lirc](http://www.lirc.org)-like input format.

# Hardware requirements

- Arduino101/Genuino101
- Infrared LED 950nm
- Resistor
- Power source (USB will work)

# Usage

Pick a digital pin, get an IR LED, and get some .cf files.

You can get your .cf files from the 
[lirc remote database](http://lirc.sourceforge.net/remotes/) or use
lirc to generate your own.


## Raw codes

Specify raw codes in an array of constant unsigned shorts and pass them
into the `sendRawCode` function.


Raw codes must be provided in the form of an array in the format
used by [lirc](http://lirc.org/html/lircd.conf.html) under
"begin raw_codes".


```c
  #define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))

  int GPIO_PIN = 3; // The pin your IRLED is using.

  const unsigned short  kanto_yu5_power[] = {
   9023,    4565,     533,     597,     490,     618,
    490,     618,     490,     618,     490,     618,
    490,     618,     490,     618,     490,     618,
    490,    1727,     533,    1727,     533,    1749,
    511,    1727,     511,    1727,     533,    1727,
    533,    1727,     533,    1727,     533,     597,
    490,     618,     490,    1749,     511,    1749,
    511,    1727,     533,     618,     490,     618,
    490,     618,     490,    1727,     533,    1727,
    511,     597,     490,     618,     490,     618,
    490,    1727,     533,    1727,     533,    1706,
    533
  };


  sendRawCodes(kanto_yu5_power, 
               _countof(kanto_yu5_power), 
               GPIO_PIN);
```

## Remotes

Create a remote by filling up the `LircRemote` structure and then
passing in your structure to the `sendRemoteCode` function.

```c

  const LircRemote fedders_aircon = {
    .descriptions = (LRD_HEADER | LRD_ONE | LRD_ZERO | LRD_PTRAIL | LRD_REPEAT | LRD_GAP | LRD_TOGBITMASK ),
    .bits         = 32,
    .header = {8975, 4557},
    .one    = {505, 1723},
    .zero   = {505, 604},
    .ptrail = 512,
    .repeat = {8975, 2299},
    .toggle_bit_mask = 0,
    .button_codes = {
                //ON_OFF                   
                0x04FB40BF,
                //FAN_SPEED
                0x04FB58A7,
                //MINUS_TEMP_TIME
                0x04FB8877,
                //PLUS_TEMP_TIME 
                0x04FB50AF,
                //TIMER
                0x04FB9867,
                //MODE
                0x04FB906F
    }
  };


  enum AirconButton {
    ON_OFF = 0,
    FAN_SPEED,
    MINUS_TEMP_TIME,
    PLUS_TEMP_TIME,
    TIMER,
    MODE
  } AirconButton;

  int GPIO_PIN = 3; // The pin your IRLED is using.

  sendRemoteCode(&fedders_aircon, ON_OFF, GPIO_PIN);
```

# Limitations

- Currently a 38KHz signal is the only supported output frequency.
- Only a subset of options in the "Remote" are suppoted.
- Interrupts are not masked during the IR code playback operation,
  which means this library may not be very reliable if interrupts
  are being used heavily on the system.


# License

BSD license.  See LICENSE.

