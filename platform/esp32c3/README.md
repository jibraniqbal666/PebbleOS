# ESP32-C3 Platform Support

This directory contains the minimal bootloader implementation for ESP32-C3.

## Prerequisites

1. **ESP-IDF Framework**: ESP-IDF v5.x or later must be installed
   - Installation: https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/get-started/
   - Default path: `/Users/jibraniqbal/AndroidStudioProjects/pebble-next/esp-idf`
   - Can override by setting `ESP_IDF_PATH` environment variable

2. **RISC-V Toolchain**: `riscv32-esp-elf-gcc` toolchain
   - Usually comes with ESP-IDF installation
   - Should be in `$ESP_IDF_PATH/tools/riscv32-esp-elf-*`

3. **esptool.py**: For flashing bootloader
   - Usually comes with ESP-IDF installation
   - Can be installed separately: `pip install esptool`

## Building

```bash
# Configure for ESP32-C3
./waf configure --board=esp32c3

# Build bootloader
./waf build

# Flash bootloader (requires ESP32-C3 connected via USB)
./waf flash --port=/dev/ttyUSB0
```

## Current Status

This is a **minimal bootloader implementation** that provides:
- Basic hardware initialization
- UART debug serial output
- Flash driver for reading firmware
- Firmware loading and jump to firmware entry point

## Not Yet Implemented

- Full ESP-IDF integration (currently uses stubs)
- Display drivers
- Button handling
- PMIC support
- Firmware update logic
- Boot bits persistence (currently uses static variables)

## Memory Layout

- Bootloader: 0x0 - 0x8000 (32KB)
- Firmware: 0x8000+ (starts after bootloader)
- IRAM: 0x40380000 (128KB)
- DRAM: 0x3FC80000 (400KB)

## Notes

- The bootloader uses ESP-IDF APIs but currently has minimal stubs
- Full ESP-IDF integration requires linking against ESP-IDF libraries
- Debug serial is critical - ensure UART works early for troubleshooting

