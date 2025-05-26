# At89s-prog
ISP programmer for Atmel (now Microchip) 8051 MCU clones - e.g. `At89s2051`, `At89s4051`, `At89s51`, `At89s52` microcontrollers.

Only bare Arduino (e.g. "nano", "pico", "mini" etc) is necessary - to serve as "programmer" itself.

This project allows directly sending Intel HEX file of compiled 8051 code over `UART` to `Arduino`, which further uploads it via `SPI` to
attached target device. Tested with At89s52 and At89s4051 - they are from slightly different families, so other chips from these families
should work. Note that chips with "c" index after "89" do not have SPI programming mode though.

### Wiring

- pin 9 - to target reset (1)
- pin 10 - to target MOSI (17)
- pin 11 - to target MISO (18)
- pin 12 - to target CLK (19)

Attach VCC and GND to target (including VPP if it is dedicated pin), plug in quartz between XTAL inputs. That's all.

### UART settings

1200 Baud (to match firmware writing speed), no parity, single stop bit.

### Extra

Besides writing HEX file to device, the option to read program memory is also provided. Just replace colons (`:`) in your hex-file
with question marks - and you'll read data instead of writing (data in hex-file themselves are not used, except for addresses in the
beginning of the lines).
