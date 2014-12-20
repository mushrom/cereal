CFLAGS = -Wall -Wextra -mmcu=attiny85 -Os -fdata-sections -ffunction-sections
# use this line instead to disable debugging
#CFLAGS = -Wall -Wextra -mmcu=attiny85 -Os -fdata-sections -ffunction-sections -DDEBUG=0
CSRC = $(wildcard *.c)
COBJ = $(CSRC:.c=.o)

OBJ = $(COBJ)

all: $(OBJ) cereal.elf cereal.hex

$(COBJ): Makefile cereal.h

cereal.elf: $(OBJ)
	avr-gcc $(CFLAGS) -Wl,-gc-sections -o $@ $(OBJ)
	avr-size $@

cereal.hex: cereal.elf
	avr-objcopy -Oihex cereal.elf cereal.hex

.c.o:
	avr-gcc $(CFLAGS) -c $< -o $@

flash: cereal.hex
	avrdude -c usbtiny -p attiny85 -U flash:w:cereal.hex

sudoflash: cereal.hex
	sudo avrdude -c usbtiny -p attiny85 -U flash:w:cereal.hex

clean:
	rm -f $(OBJ) cereal.elf cereal.hex
