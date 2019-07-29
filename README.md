## ATMega32u4 HID Keyboard
###### Board Project 2019 - Krishna Mani
This is software for ATMegau32u4 or compatible AVR hardware USB chips that implements a standard-complying HID keyboard using only AVR-LIBC.

I used these for reference when coding:

1. https://www.avrfreaks.net/forum/usb-initialization-problem
2. https://www.pjrc.com/teensy/usb_keyboard.html
3. https://www.usb.org/sites/default/files/documents/hid1_11.pdf
4. https://www.avrfreaks.net/forum/help-properly-initializing-usb-hardware-atmega32u4
5. https://www.beyondlogic.org/usbnutshell/usb1.shtml
6. http://ww1.microchip.com/downloads/en/devicedoc/atmel-7766-8-bit-avr-atmega16u4-32u4_datasheet.pdf
7. https://github.com/arduino/ArduinoCore-avr/blob/master/cores/arduino/USBCore.cpp

## Code Explanation 
#### Initializing the USB Interface
The USB controller is initialized according to the datasheet register descriptions. First, the PLL (Phase Locked Loop) is setup to act as the USB clock, running at 48mHz
from a scaling of the 16mHz system clock. The register `UHWCON` enables the USB pad regulator for the USB interface, and the registers `PLLCSR`, `USBCON`, and `UDCON` are used
to configure the PLL, USB controller enable, and USB Speed Settings, respectively. The register `UDIEN` is used to enable USB Device Interrupts, such as `EORSTE`, which triggers
when the device finishes resetting the controller on USB plugin.
#### USB Setup and Enumeration
After `EORSTE`, the Control Endpoint 0 is setup to receive setup packets from the host. `UEIENX` is set to enable the Endpoint Interrupt `RXSTPI` for receiving setup packets.
Here is a table of USB Setup packet fields (5) :

|Offset|Field|Size|Value|Description|
|------|-----|----|-----|-----------|
|0|bmRequestType|1|Bit-Map|Request Device Info|
|1|bRequest|1|Value|Request|
|2|wValue|2|Value|Value|
|4|wIndex|2|Index or Offset|Index|
|6|wLength|2|Count|Number of bytes to transfer if there is a data phase|

These values are used to determine the request type and parameters. They are read in through the FIFO register `UEDATX`. The ISR `USB_COM_vect` is triggered by interrupts in `UEINTX`. This includes the `RXSTPI` interrupt, which triggers on setup packet receive.
On `UEINTX & (1 << RXSTPI)`, we read in the setup packet values and determine the request type. There are several types of setup requests, the first of which is the `GET_DESCRIPTOR`. First the device descriptor is sent, which also includes information about which descriptors the host is allowed to request. Then, configration, interface, HID, and endpoint descriptors are sent to fully enumerate the device. Several other setup requests and HID requests are sent to indicate to the host the protocol the device requires and device type.
The request `SET_CONFIGURATION` is responsible for the setup of Endpoint 3, the endpoint responsible for the interrupt requests to the host to send HID reports. After this request, the device is considered fully configured except for the HID specific requests. 
#### USBHID Reporting and Idle
Once the device is setup fully, the `SOFE` (Start of Frame) interrupt is responsible for reporting the device state. Apart from the other methods `usb_send` and `get_keypress`, which update the HID device idle and send the report independently, the `SOFE` interrupt takes care of reporting device state when no reports are generated after an interval of `keyboard_idle_value`, which is set by the host in the HID request `SET_IDLE`.
#### Keycode Logic
The majority of implementations of HID keyboards use large lists of #define's to map ASCII characters on the keyboard to HID keycode values. This, however, makes it inconvenient to make keys that map to more than one keycode like macros. The arrays `keycodes[]` and `modifiers[]` are ordered in ASCII value with the HID keycode stored at the corresponding index. This makes converting arbitrarily long strings of ASCII to keycodes trivial, just casting to int and performing a lookup, making this essentially the same in terms of computational power. The `modifiers[]` array corresponds to keys that need a shift key pressed in HID keycodes, but are different codes in ASCII. It's useless when making actual layouts because you press the shift key yourself, but might be useful if you want a very specific macro. Most of these large arrays have the keyword `PROGMEM` (Program Memory). This means that they are stored in flash instead of SRAM, which is used for variables. Since SRAM is significantly more limited than flash, it makes sense to store constants in flash, which is slower but that doesn't matter too much in most cases. The descriptors are also stored in PROGMEM.

