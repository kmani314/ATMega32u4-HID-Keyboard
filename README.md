## ATMega32u4 HID Keyboard
###### Board Project 2019 - Krishna Mani
This is software for ATMega32u4 or compatible AVR hardware USB chips that implements a standard-complying HID keyboard using only AVR-LIBC.

I used these for reference when coding:

1. https://www.avrfreaks.net/forum/usb-initialization-problem
2. https://www.pjrc.com/teensy/usb_keyboard.html
3. https://www.usb.org/sites/default/files/documents/hid1_11.pdf
4. https://www.avrfreaks.net/forum/help-properly-initializing-usb-hardware-atmega32u4
5. https://www.beyondlogic.org/usbnutshell/usb1.shtml
6. http://ww1.microchip.com/downloads/en/devicedoc/atmel-7766-8-bit-avr-atmega16u4-32u4_datasheet.pdf
7. https://github.com/arduino/ArduinoCore-avr/blob/master/cores/arduino/USBCore.cpp
## Hardware
##### Microcontroller
The ATMega32u4 controls the scanning matrix and the USB interface. I chose it because of its hardware USB interface and because it has enough I/O pins to support the whole keyboard without needing any multiplexing. It also has enough flash and SRAM for storing layouts.
The 32u4 on the keyboard runs at 16mHz from an external crystal oscillator with 22pF load capacitors. It is powered directly from the USB lines with filtering capacitors.
##### PCBs
## Code
##### USB
First, the USB controller of the 32u4 is initialized. This is handled in the function `usb_init()`. The PLL (Phase Locked Loop) is initialized, taking the external clock as input. It runs at 48mHz and acts as the USB clock. After the USB controller is initialized, the control endpoint is set up and will receive setup packets from the host. The setup packets, handled in the ISR `USB_gen_vect`, elicit responses from the keyboard based on the values of `bmRequestType` and `bRequest`, which determine the type of request. After the host has requested and received the USB descriptors, and the keyboard has correctly responded to HID-specific requests, the host can enumerate the device and use it as input. Then, the keyboard will send HID Idle Packets to register key repeats whenever keys have been held down for a period of time set by the host. 

##### Keyboard Software
The scannng matrix of the keyboard uses the current sourcing and internal pullups of the I/O ports on the 32u4. Because of the orientation of the didoes, the columns of the keyboard are set low when they are going to be read. The rows are always pulled up, meaning that if a key is pressed, it will be pulled low because the switch is connecting the row and columns. The matrix scanning is repeated quickly, and changes in key state are recorded in the six-byte USB HID report. The report is six bytes to support six concurrent key preses. The keyboard assigns and removes keys from the report to support 6-key-rollover. The meta and special keys like CTRL, ALT, and SHIFT have their own byte apart from the report, where each bit represents a different key. These keys are handled separately.

The row and columns of the recorded keypresses are translated into actual keycodes from the specified layout in `layout.h`. The layout is written from the standard HID keycodes in `keys.h`. Because the keyboard does not have enough physical keys to support all the letters, function, and number keys, some keys are macros that change the mappings of all the keys. The current layout I'm using has three layers: the standard QWERTY layer, a raised layer for the numbers and symbols, and a function layer for media and function keys. The layouts can be completely customized.

