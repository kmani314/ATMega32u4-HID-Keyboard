/*
usb.c
USB Controller initialization, device setup, and HID interrupt routines
*/

#define F_CPU 16000000
#include "usb.h"
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "avr/io.h"


volatile uint8_t keyboard_pressed_keys[6] = {0, 0, 0, 0, 0, 0};
volatile uint8_t keyboard_modifier = 0;

static uint16_t keyboard_idle_value =
    125;  // HID Idle setting, how often the device resends unchanging reports,
          // we are using a scaling of 4 because of the register size
static uint8_t current_idle =
    0;  // Counter that updates based on how many SOFE interrupts have occurred
static uint8_t this_interrupt =
    0;  // This is not the best way to do it, but it
        // is much more readable than the alternative

/*  Device Descriptor - The top level descriptor when enumerating a USB device`
        Specification: USB 2.0 (April 27, 2000) Chapter 9 Table 9-5

*/

static const uint8_t device_descriptor[] PROGMEM = {
    // Stored in PROGMEM (Program Memory) Flash, freeing up some SRAM where
    // variables are usually stored
    18,  // bLength - The total size of the descriptor
    1,   // bDescriptorType - The type of descriptor - 1 is device
    0x00,
    0x02,  // bcdUSB - The USB protcol supported - Refer to USB 2.0
           // Chapter 9.6.1
    0,   // bDeviceClass - The Device Class, 0 indicating that the HID interface
         // will specify it
    0,   // bDeviceSubClass - 0, HID will specify
    0,   // bDeviceProtocol - No class specific protocols on a device level, HID
         // interface will specify
    32,  // bMaxPacketSize0 - 32 byte packet size; control endpoint was
         // configured in UECFG1X to be 32 bytes
    (idVendor & 255),
    ((idVendor >> 8) &
     255),  // idVendor - Vendor ID specified by USB-IF (To fit the 2 bytes, the
            // ID is split into least significant and most significant byte)
    (idProduct & 255),
    ((idProduct >> 8) & 255),  // idProduct - The Product ID specified by USB-IF
                               // - Split in the same way as idVendor
    0x00,
    0x01,  // bcdDevice - Device Version Number
    0,  // iManufacturer - The String Descriptor that has the manufacturer name
        // -
        // Specified by USB 2.0 Table 9-8
    0,  // iProduct - The String Descriptor that has the product name -
        // Specified
        // by USB 2.0 Table 9-8
    0,  // iSerialNumber - The String Descriptor that has the serial number of
        // the product - Specified by USB 2.0 Table 9-8
    1   // bNumConfigurations - The number of configurations of the device, most
        // devices only have one
};

/*  HID Descriptor - The descriptor that gives information about the HID device
        Specification: Device Class Definition for Human Interface Devices (HID)
   6/27/2001 Appendix B - Keyboard Protocol Specification This descriptor was
   written referring to the example descriptor in table E.6
*/
static const uint8_t keyboard_HID_descriptor[] PROGMEM = {
    0x05,
    0x01,  // Usage Page - Generic Desktop - HID Spec Appendix E E.6 - The
           // values for the HID tags are not clearly listed anywhere really, so
           // this table is very useful
    0x09,
    0x06,  // Usage - Keyboard
    0xA1,
    0x01,  // Collection - Application
    0x05,
    0x07,  // Usage Page - Key Codes
    0x19,
    0xE0,  // Usage Minimum - The bit that controls the 8 modifier characters
           // (ctrl, command, etc)
    0x29,
    0xE7,  // Usage Maximum - The end of the modifier bit (0xE7 - 0xE0 = 1 byte)
    0x15,
    0x00,  // Logical Minimum - These keys are either not pressed or pressed, 0
           // or 1
    0x25,
    0x01,  // Logical Maximum - Pressed state == 1
    0x75,
    0x01,  // Report Size - The size of the IN report to the host
    0x95,
    0x08,  // Report Count - The number of keys in the report
    0x81,
    0x02,  // Input - These are variable inputs
    0x95,
    0x01,  // Report Count - 1
    0x75,
    0x08,  // Report Size - 8
    0x81,
    0x01,  // This byte is reserved according to the spec
    0x95,
    0x05,  // Report Count - This is for the keyboard LEDs
    0x75,
    0x01,  // Report Size
    0x05,
    0x08,  // Usage Page for LEDs
    0x19,
    0x01,  // Usage minimum for LEDs
    0x29,
    0x05,  // Usage maximum for LEDs
    0x91,
    0x02,  // Output - This is for a host output to the keyboard for the status
           // of the LEDs
    0x95,
    0x01,  // Padding for the report so that it is at least 1 byte
    0x75,
    0x03,  // Padding
    0x91,
    0x01,  // Output - Constant for padding
    0x95,
    0x06,  // Report Count - For the keys
    0x75,
    0x08,  // Report Size - For the keys
    0x15,
    0x00,  // Logical Minimum
    0x25,
    0x65,  // Logical Maximum
    0x05,
    0x07,  // Usage Page - Key Codes
    0x19,
    0x00,  // Usage Minimum - 0
    0x29,
    0x65,  // Usage Maximum - 101
    0x81,
    0x00,  // Input - Data, Array
    0xC0   // End collection
};

/*  Configuration Descriptor - The descriptor that gives information about the
   device conifguration(s) and how to select them Specification: USB 2.0 (April
   27, 2000) Chapter 9 Table 9-10 Note: The reason the descriptors are
   structured the way they are is to better show the tree structure of the
   descriptors; if the HID inteface descriptor was separate from the
   configuration descriptor, we wouldn't need the HID_OFFSET, but that would
   imply that the descriptors are separate entities when they are really
   dependent on each other
*/

static const uint8_t configuration_descriptor[] PROGMEM = {
    9,  // bLength
    2,  // bDescriptorType - 2 is device
    (CONFIG_SIZE & 255),
    ((CONFIG_SIZE >> 8) &
     255),  // wTotalLength - The total length of the descriptor tree
    1,      // bNumInterfaces - 1 Interface
    1,      // bConfigurationValue
    0,      // iConfiguration - We have no string descriptors
    0xC0,   // bmAttributes - Set the device power source
    50,     // bMaxPower - 50 x 2mA units = 100mA max power consumption
    // Refer to Table 9-10 for the descriptor structure - Configuration
    // Descriptors have interface descriptors, interface descriptors have
    // endpoint descriptors along with a special HID descriptor
    9,     // bLength
    4,     // bDescriptorType - 4 is interface
    0,     // bInterfaceNumber - This is the 0th and only interface
    0,     // bAlternateSetting - There are no alternate settings
    1,     // bNumEndpoints - This interface only uses one endpoint
    0x03,  // bInterfaceClass - 0x03 (specified by USB-IF) is the interface
           // class code for HID
    0x01,  // bInterfaceSubClass - 1 (specified by USB-IF) is the constant for
           // the boot subclass - this keyboard can communicate with the BIOS,
           // but is limited to 6KRO, as are most keyboards
    0x01,  // bInterfaceProtocol - 0x01 (specified by USB-IF) is the protcol
           // code for keyboards
    0,     // iInterface - There are no string descriptors for this
    // HID Descriptor - Refer to E.4 HID Spec
    9,           // bLength
    0x21,        // bDescriptorType - 0x21 is HID
    0x11, 0x01,  // bcdHID - HID Class Specification 1.11
    0,           // bCountryCode
    1,           // bNumDescriptors - Number of HID descriptors
    0x22,        // bDescriptorType - Type of descriptor
    sizeof(keyboard_HID_descriptor), 0,  // wDescriptorLength
    // Endpoint Descriptor - Example can be found in the HID spec table E.5
    7,     // bLength
    0x05,  // bDescriptorType
    KEYBOARD_ENDPOINT_NUM |
        0x80,  // Set keyboard endpoint to IN endpoint, refer to table
    0x03,      // bmAttributes - Set endpoint to interrupt
    8, 0,      // wMaxPacketSize - The size of the keyboard banks
    0x01       // wInterval - Poll for new data 1000/s, or once every ms
};

int usb_init() {
  cli();  // Global Interrupt Disable

  UHWCON |= (1 << UVREGE);  // Enable USB Pads Regulator

  PLLCSR |= 0x12;  // Configure to use 16mHz oscillator

  while (!(PLLCSR & (1 << PLOCK)))
    ;  // Wait for PLL Lock to be achieved

  USBCON |=
      (1 << USBE) | (1 << OTGPADE);  // Enable USB Controller and USB power pads
  USBCON &= ~(1 << FRZCLK);          // Unfreeze the clock

  UDCON = 0;  // FULL SPEED MODE

  USBCON &= ~(1 << DETACH);  // Connect
  UDIEN |= (1 << EORSTE) |
           (1 << SOFE);  // Re-enable the EORSTE (End Of Reset) Interrupt so we
                         // know when we can configure the control endpoint
  usb_config_status = 0;
  sei();  // Global Interrupt Enable
  return 0;
}

int send_keypress(uint8_t key, uint8_t mod) {
  keyboard_pressed_keys[0] = key;
  keyboard_modifier = mod;
  if (usb_send() < 0)
    return -1;
  keyboard_pressed_keys[0] = 0;
  keyboard_modifier = 0;
  if (usb_send() < 0)
    return -1;
  return 0;
}

int usb_send() {
  if (!usb_config_status)
    return -1;  // Why are you even trying
  cli();
  UENUM = KEYBOARD_ENDPOINT_NUM;

  while (!(UEINTX & (1 << RWAL)))
    ;  // Wait for banks to be ready
  UEDATX = keyboard_modifier;
  UEDATX = 0;
  for (int i = 0; i < 6; i++) {
    UEDATX = keyboard_pressed_keys[i];
  }

  UEINTX = 0b00111010;
  current_idle = 0;
  sei();
  return 0;
}

bool get_usb_config_status() {
  return usb_config_status;
}

ISR(USB_GEN_vect) {
  uint8_t udint_temp = UDINT;
  UDINT = 0;

  if (udint_temp & (1 << EORSTI)) {  // If end of reset interrupt
    // Configure Control Endpoint
    UENUM = 0;             // Select Endpoint 0, the default control endpoint
    UECONX = (1 << EPEN);  // Enable the Endpoint
    UECFG0X = 0;      // Control Endpoint, OUT direction for control endpoint
    UECFG1X |= 0x22;  // 32 byte endpoint, 1 bank, allocate the memory
    usb_config_status = 0;

    if (!(UESTA0X &
          (1 << CFGOK))) {  // Check if endpoint configuration was successful
      return;
    }

    UERST = 1;  // Reset Endpoint
    UERST = 0;

    UEIENX =
        (1 << RXSTPE);  // Re-enable the RXSPTE (Receive Setup Packet) Interrupt
    return;
  }
  if ((udint_temp & (1 << SOFI)) &&
      usb_config_status) {  // Check for Start Of Frame Interrupt and correct
                            // usb configuration, send keypress if a keypress
                            // event has not been sent through usb_send
    this_interrupt++;
    if (keyboard_idle_value &&
        (this_interrupt & 3) == 0) {  // Scaling by four, trying to save memory
      UENUM = KEYBOARD_ENDPOINT_NUM;
      if (UEINTX & (1 << RWAL)) {  // Check if banks are writable
        current_idle++;
        if (current_idle ==
            keyboard_idle_value) {  // Have we reached the idle threshold?
          current_idle = 0;
          UEDATX = keyboard_modifier;
          UEDATX = 0;
          for (int i = 0; i < 6; i++) {
            UEDATX = keyboard_pressed_keys[i];
          }
          UEINTX = 0b00111010;
        }
      }
    }
  }
}

ISR(USB_COM_vect) {
  UENUM = 0;
  if (UEINTX & (1 << RXSTPI)) {
    uint8_t bmRequestType = UEDATX;  // UEDATX is FIFO; see table in README
    uint8_t bRequest = UEDATX;
    uint16_t wValue = UEDATX;
    wValue |= UEDATX << 8;
    uint16_t wIndex = UEDATX;
    wIndex |= UEDATX << 8;
    uint16_t wLength = UEDATX;
    wLength |= UEDATX << 8;

    DDRC = 0xFF;

    UEINTX &= ~(
        (1 << RXSTPI) | (1 << RXOUTI) |
        (1 << TXINI));  // Handshake the Interrupts, do this after recording
                        // the packet because it also clears the endpoint banks
    if (bRequest == GET_DESCRIPTOR) {
      // The Host is requesting a descriptor to enumerate the device
      uint8_t* descriptor;
      uint8_t descriptor_length;

      if (wValue == 0x0100) {  // Is the host requesting a device descriptor?
        descriptor = device_descriptor;
        descriptor_length = pgm_read_byte(descriptor);
      } else if (wValue ==
                 0x0200) {  // Is it asking for a configuration descriptor?
        descriptor = configuration_descriptor;
        descriptor_length =
            CONFIG_SIZE;  // Configuration descriptor is comprised of many
                          // different descriptors; the length is more than
                          // bLength
      } else if (wValue ==
                 0x2100) {  // Is it asking for a HID Report Descriptor?
        descriptor = configuration_descriptor + HID_OFFSET;
        descriptor_length = pgm_read_byte(descriptor);
      } else if (wValue == 0x2200) {
        descriptor = keyboard_HID_descriptor;
        descriptor_length = sizeof(keyboard_HID_descriptor);
      } else {
        PORTC = 0xFF;
        UECONX |=
            (1 << STALLRQ) | (1 << EPEN);  // Enable the endpoint and stall, the
                                           // descriptor does not exist
        return;
      }

      uint8_t request_length =
          wLength > 255 ? 255
                        : wLength;  // Our endpoint is only so big; the USB Spec
                                    // says to truncate the response if the size
                                    // exceeds the size of the endpoint

      descriptor_length =
          request_length > descriptor_length
              ? descriptor_length
              : request_length;  // Truncate to descriptor length at most

      while (descriptor_length > 0) {
        while (!(UEINTX & (1 << TXINI)))
          ;  // Wait for banks to be ready for data transmission
        if (UEINTX & (1 << RXOUTI))
          return;  // If there is another packet, exit to handle it

        uint8_t thisPacket =
            descriptor_length > 32
                ? 32
                : descriptor_length;  // Make sure that the packet we are
                                      // getting is not too big to fit in the
                                      // endpoint

        for (int i = 0; i < thisPacket; i++) {
          UEDATX = pgm_read_byte(
              descriptor +
              i);  // Send the descriptor over UEDATX, use pgmspace functions
                   // because the descriptors are stored in flash
        }

        descriptor_length -= thisPacket;
        descriptor += thisPacket;
        UEINTX &= ~(1 << TXINI);
      }
      return;
    }

    if (bRequest == SET_CONFIGURATION &&
        bmRequestType ==
            0) {  // Refer to USB Spec 9.4.7 - This is the configuration request
                  // to place the device into address mode
      usb_config_status = wValue;
      UEINTX &= ~(1 << TXINI);
      UENUM = KEYBOARD_ENDPOINT_NUM;
      UECONX = 1;
      UECFG0X = 0b11000001;  // EPTYPE Interrupt IN
      UECFG1X = 0b00000110;  // Dual Bank Endpoint, 8 Bytes, allocate memory
      UERST = 0x1E;          // Reset all of the endpoints
      UERST = 0;
      return;
    }

    if (bRequest == SET_ADDRESS) {
      UEINTX &= ~(1 << TXINI);
      while (!(UEINTX & (1 << TXINI)))
        ;  // Wait until the banks are ready to be filled

      UDADDR = wValue | (1 << ADDEN);  // Set the device address
      return;
    }

    if (bRequest == GET_CONFIGURATION &&
        bmRequestType == 0x80) {  // GET_CONFIGURATION is the host trying to get
                                  // the current config status of the device
      while (!(UEINTX & (1 << TXINI)))
        ;  // Wait until the banks are ready to be filled
      UEDATX = usb_config_status;
      UEINTX &= ~(1 << TXINI);
      return;
    }

    if (bRequest == GET_STATUS) {
      while (!(UEINTX & (1 << TXINI)))
        ;
      UEDATX = 0;
      UEDATX = 0;
      UEINTX &= ~(1 << TXINI);
      return;
    }

    if (wIndex == 0) {  // Is this a request to the keyboard interface for HID
                        // class-specific requests?
      if (bmRequestType ==
          0xA1) {  // GET Requests - Refer to the table in HID Specification 7.2
                   // - This byte specifies the data direction of the packet.
                   // Unnecessary since bRequest is unique, but it makes the
                   // code clearer
        if (bRequest == GET_REPORT) {  // Get the current HID report
          while (!(UEINTX & (1 << TXINI)))
            ;  // Wait for the banks to be ready for transmission
          UEDATX = keyboard_modifier;

          for (int i = 0; i < 6; i++) {
            UEDATX = keyboard_pressed_keys
                [i];  // According to the spec, this method of getting the
                      // report is not used for device polling, although we
                      // still have to implement the response
          }
          UEINTX &= ~(1 << TXINI);
          return;
        }
        if (bRequest == GET_IDLE) {
          while (!(UEINTX & (1 << TXINI)))
            ;

          UEDATX = keyboard_idle_value;

          UEINTX &= ~(1 << TXINI);
          return;
        }
        if (bRequest == GET_PROTOCOL) {
          while (!(UEINTX & (1 << TXINI)))
            ;

          UEDATX = keyboard_protocol;

          UEINTX &= ~(1 << TXINI);
          return;
        }
      }

      if (bmRequestType ==
          0x21) {  // SET Requests - Host-to-device data direction
        if (bRequest == SET_REPORT) {
          while (!(UEINTX & (1 << RXOUTI)))
            ;  // This is the opposite of the TXINI one, we are waiting until
               // the banks are ready for reading instead of for writing
          keyboard_leds = UEDATX;

          UEINTX &= ~(1 << TXINI);  // Send ACK and clear TX bit
          UEINTX &= ~(1 << RXOUTI);
          return;
        }
        if (bRequest == SET_IDLE) {
          keyboard_idle_value = wValue;  //
          current_idle = 0;

          UEINTX &= ~(1 << TXINI);  // Send ACK and clear TX bit
          return;
        }
        if (bRequest ==
            SET_PROTOCOL) {  // This request is only mandatory for boot devices,
                             // and this is a boot device
          keyboard_protocol =
              wValue >> 8;  // Nobody cares what happens to this, arbitrary cast
                            // from 16 bit to 8 bit doesn't matter

          UEINTX &= ~(1 << TXINI);  // Send ACK and clear TX bit
          return;
        }
      }
    }
  }
  PORTC = 0xFF;
  UECONX |= (1 << STALLRQ) |
            (1 << EPEN);  // The host made an invalid request or there was an
                          // error with one of the request parameters
}
