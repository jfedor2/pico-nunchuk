# USB adapter for a Wii Nunchuk

This repository contains code for a Raspberry Pi Pico (or any other RP2040-based board) to interface with a Wii Nunchuk controller and adapt it so that it works as a USB mouse or gamepad.

Default wiring is GPIO2 - SDA (data), GPIO3 - SCL (clock). Also connect the VCC wire to 3.3V on the Pico and GND to GND.

You can compile it with the following commands (or use the precompiled UF2 files):

```
git clone https://github.com/jfedor2/pico-nunchuk.git
cd pico-nunchuk
git submodule update --init
mkdir build
cd build
cmake ..
make
```
