# Device Drivers

Orbit OS is incrementally adding support for basic PC hardware components.

## Real-Time Clock (RTC)

The RTC driver (`drivers/rtc.c`) interacts directly with the CMOS chip to retrieve the system's current date and time.
- Communicates via I/O ports `0x70` (Address) and `0x71` (Data).
- Handles the "Update in Progress" flag to ensure time is read consistently.
- Parses the BCD (Binary-Coded Decimal) format (if configured) into standard integers.
- Manages 12-hour vs 24-hour clock conversions.

You can check the system time in the shell using the `date` command.

## PCI Bus Enumerator

The PCI enumerator (`drivers/pci.c`) discovers all devices connected to the Peripheral Component Interconnect bus.
- Communicates via I/O ports `0xCF8` (Config Address) and `0xCFC` (Config Data).
- Scans all 256 buses, 32 devices, and 8 functions.
- Extracts the Vendor ID, Device ID, and Class/Subclass bytes.
- Fully supports discovering single-function and multi-function devices.

You can view connected devices in the shell using the `lspci` command.
In QEMU, this typically discovers the Intel 440FX Host Bridge, PIIX3 IDE controller, QEMU Virtual Video, and the Intel E1000 Gigabit Ethernet adapter.
