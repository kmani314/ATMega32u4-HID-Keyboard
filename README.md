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
8. http://sdphca.ucsd.edu/lab_equip_manuals/usb_20.pdf

Finished Board:
[Finished Board](img/keyboard_top.jpg)

#### Hardware

##### Microcontroller
The ATMega32u4 controls the scanning matrix and the USB interface. I chose it because of its hardware USB interface and because it has enough I/O pins to support the whole keyboard without needing any multiplexing. It also has enough flash and SRAM for storing layouts. The 32u4 on the keyboard runs at 16mHz from an external crystal oscillator with 22pF load capacitors. It is powered directly from the USB lines with filtering capacitors.
##### PCBs
Initially, I needed to test the main microcontroller circuit of the keyboard to ensure that it worked and that the USB worked, because I had developed on an Arduino Leonardo. (boards/ATMegaTestingBoard.brd)

##### ATMega32u4 Circuit
The portion of the board for the ATMega32u4 is fairly standard for this chip and is similar to the Arduino Leonardo, excluding the power regulators. It consists of the chip, the oscillator and load caps, the power and layer leds, the reset switch, the USB connector and data resistors, the ICSP pins, and the filtering caps on the underside of the board.

Top / Bottom (boards/Keyboard.brd):

[32u4 Top](img/top_32u4.png)

[32u4 Bottom](bot_32u4.png)

The rest of the board is the scanning matrix. The rows and columns use up the majority of the I/O on the chip.
The matrix is standard for a keyboard - one pin of each switch in a column is connected through a diode to every other one in the column. Then, the remaining pin in each switch is common with the row. I used through hole diodes because they are much easier to solder. The switches are standard mechanical keyboard switches, and the footprints are from here: https://github.com/mknyszek/gateron-eagle
To read the matrix, the row pins are pulled high through internal pull-up resistors. One column at a time is set low, so that a pressed key would pull the row pin low, signaling a key press at the current row and column index.

Top / Bottom matrix (boards/Keyboard.brd):

[Matrix Top](img/top_matrix.png)

[Matrix Bottom](bot_matrix.png)

##### Board Assembly
Assembling the board was straightforward except for the SMD components, specifically the TQFP ATMega32u4. It is really easy to bridge several pads very quickly, and completely ruin the chip. I soldered these by holding them in place and tacking down the corners so the chip didn’t move. I used a lot of flux, then with very little solder dragged the iron across the pins. The USB connector is also really difficult, I soldered the anchor pins and then the pins. It is really easy to bridge the pins with the ground enclosure. Using super glue after ensures that the connector doesn’t snap off when you unplug the cable.

#### Code
##### USB
First, the USB controller of the 32u4 is initialized. This is handled in the function `usb_init()`. The PLL (Phase Locked Loop) is initialized, taking the external clock as input. It runs at 48mHz and acts as the USB clock. After the USB controller is initialized, the control endpoint is set up and will receive setup packets from the host. The setup packets, handled in the ISR `USB_gen_vect`, elicit responses from the keyboard based on the values of bmRequestType and bRequest, which determine the type of request. After the host has requested and received the USB descriptors, and the keyboard has correctly responded to HID-specific requests, the host can enumerate the device and use it as an input device.

The key repeat feature of modern keyboards is handled separately from the standard functions `usb_send()` and `usb_send_keypress()`. If a key is held down for some period of time, it should be repeated until it is released. This is taken care of in the ISR `USB_gen_vect`, specifically when the SOFE (start of frame) bit is set. This interrupt triggers at 1kHz, which is how often the host sends start of frame packets. If no keypresses have been generated since the period time, which is kept track of with keyboard_idle_value and current_idle, the entire HID report is resent to facilitate key repeat. The period of time the device should wait before resending the report is set from the wValue in the HID request SET_IDLE.
##### Matrix Software
The scanning matrix of the keyboard uses the current sourcing and internal pullups of the I/O ports on the 32u4. Because of the orientation of the diodes, the columns of the keyboard are set low when they are going to be read. The rows are always pulled up, meaning that if a key is pressed, it will be pulled low because the switch is connecting the row and columns. The matrix scanning is repeated quickly, and changes in key state are recorded in the six-byte USB HID report. The report is six bytes to support six concurrent key presses. The keyboard dynamically assigns and removes keys from the report to support 6-key-rollover, and it also resets the reports on layout layer shifts to prevent strange glitches. The meta and special keys like CTRL, ALT, and SHIFT have their own byte apart from the report, where each bit represents a different key. This is according to the USB HID Spec.

The rows and columns of the recorded keypresses are translated into actual keycodes from the specified layout in `layout.h`. The layout is written from the standard HID keycodes in `keys.h`. Because the keyboard does not have enough physical keys to support all the letters, function, and number keys, some keys are macros that change the mappings of all the keys. These keys are handled separately and do not generate keypress events on their own. Rather, they update the current layer that the scanning matrix is reporting keypresses from to change the keycodes.
