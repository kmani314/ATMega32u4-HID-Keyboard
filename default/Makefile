###############################################################################
# Makefile for the project keyboard
###############################################################################

## General Flags
PROJECT = keyboard
MCU = atmega32u4
TARGET = keyboard.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=16000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wno-unused-value -Wno-unused-variable -Wno-unused-but-set-variable
CFLAGS += -Wno-pointer-sign # using int8_t instead of char
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 
INCLUDES=-I../include/

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=keyboard.map
#LDFLAGS +=  -Wl,-u,vfprintf -lprintf_flt -lm


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = main.o matrix.o usb.o

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) keyboard.hex keyboard.eep keyboard.lss size

## Compile
main.o: ../src/main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

matrix.o: ../src/matrix.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usb.o: ../src/usb.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<


##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) keyboard.elf dep/* keyboard.hex keyboard.eep keyboard.lss keyboard.map


## Other dependencies
-include $(shell mkdir dep 2>NUL) $(wildcard dep/*)

