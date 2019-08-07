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

## Electronics
#### ATMega32u4
I chose the ATMega32u4 for the keyboard primarily because of its hardware USB controller, which makes programming a USB application much easier than software USB emulation. Also, it supports a 16mHz clock frequency and has enough
Digital I/O pins for the keyboard, so no multiplexing is needed. It also comes in the TQFP package, which is small enough to fit on a reasonably sized board but is also not impossible to solder by hand.

## Code
#### USB
There are a few steps before a USB device can serve its intended purpose:
1. The device must setup the Control Endpoint, mandated to be Endpoint #0 by the USB Spec
2. The device must receive packets on the Control Endpoint, and respond accordingly
In the file `usb.c`:
The function `usb_init()` handles the initialization of the USB interface and sets `ISR(USB_COM_vect)` 
to handle the setup of the control endpoint once the USB Interface has finished resetting (Indicated by `EORSTE`).
Also, `ISR(USB_COM_vect)` sets `ISR(USB_GEN_vect)` to be called when setup packets are received. This is set
through the Endpoint Interrupt Register `UEINTX` and the `RXSTPI` bit.
The descriptors of the device tell the host information about it. The keyboard has several:
1. Device Descriptor - Required by all devices, most basic information about the device like USB version
2. Configuration Descriptor - Required, more specific info like number of endpoints and endpoint sizes
3. Interface Descrpitor - Info about the USB Interface and what endpoints it uses
4. Information about the keyboard and its protocol, specific to this device
5. USB-HID Descriptor - Info about the USB-HID Report descriptor
6. Endpoint descriptor - Info about the keyboard endpoints
The host needs these descriptors to enumerate the device. It requests them through specific requests in the 
setup packets.
In the ISR `USB_GEN_vect`:
The data in the endpoint selected by `UENUM` is stored in FIFO fashion in the register `UEDATX`.
The setup packet is loaded into variables based on the packet fields' length and order.
Based on the values of `bRequest` and `bmRequestType`, you can figure out what the host wants. The request
`GET_DESCRIPTOR` is what the host sends when it wants the descriptors for device enumeration.
There are several other requests that configure the device address and communicate the device status to the
host.
The USB-HID class specific requests are used to configure the HID parameters of the device.
In `if(wIndex == 0)`:
The request `GET_REPORT` is the host manually requesting the keyboard status.
The request `GET_IDLE` is the host requesting the time before key-repeat reports are sent.
The request `GET_PROTOCOL` doesn't do anything, because our keyboard only has one protocol.
The `SET` requests do not do anything on our device, although they must be implemented. (Except for `SET_IDLE`)
The device can now function normally. The reports of the keyboard status are taken care of by the actual
matrix code calling the send functions, or by the idle handler.
In the ISR `USB_COM_vect`:
The SOFE interrupt occurs once every ms. If the keyboard state hasn't changed in the last interval set by 
`SET_IDLE`, we are required to resend the same report so that the host can register key repeats.
#### Keyboard
Because the keyboard doesn't have the normal number of keys, the keyboard must have key layers controlled
with the raise and lower keys. These keys can't be treated as normal keys, and do not translate into keypresses.
These keys are hardwired to shift the key layer. 
