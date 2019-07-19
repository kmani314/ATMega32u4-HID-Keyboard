## ATMega32u4 HID Keyboard
###### Board Project - Krishna Mani
This is software for ATMegau32u4 or compatible AVR hardware USB chips that implements a standard-complying HID keyboard using only AVR-LIBC.

The following documents were used for reference:

1. https://www.avrfreaks.net/forum/usb-initialization-problem
2. https://www.pjrc.com/teensy/usb_keyboard.html
3. https://www.usb.org/sites/default/files/documents/hid1_11.pdf
4. https://www.avrfreaks.net/forum/help-properly-initializing-usb-hardware-atmega32u4
5. https://www.beyondlogic.org/usbnutshell/usb1.shtml
6. http://ww1.microchip.com/downloads/en/devicedoc/atmel-7766-8-bit-avr-atmega16u4-32u4_datasheet.pdf

## Code Explanation and Reasoning
#### Initializing the USB Interface
The USB controller is initialized according to the datasheet register descriptions. First, the PLL (Phase Locked Loop) is setup to act as the USB clock, running at 48mHz
from a scaling of the 16mHz system clock. The register `UHWCON` enables the USB pad regulator for the USB interface, and the registers `PLLCSR`, `USBCON`, and `UDCON` are used
to configure the PLL, USB controller enable, and USB Speed Settings, respectively. The register `UDIEN` is used to enable USB Device Interrupts, such as `EORSTE`, which triggers
when the device finishes resetting the controller on USB plugin.
#### USB Setup
After `EORSTE`, the Control Endpoint 0 is setup to receive setup packets from the host. `UEIENX` is set to enable the Endpoint Interrupt `RXSTPI` for receiving setup packets.
