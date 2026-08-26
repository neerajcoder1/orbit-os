# Networking

Orbit OS includes the foundation for a networking stack, starting with hardware initialization for the Intel E1000 Gigabit Ethernet Controller (Intel 8254x).

## Intel E1000 Driver

The E1000 driver (`drivers/e1000.c`) performs the following steps during boot:
1. **Discovery**: Queries the PCI enumerator for Vendor ID `0x8086` and Device ID `0x100E`.
2. **Bus Mastering**: Enables the Bus Master bit in the PCI Command Register to allow Direct Memory Access (DMA) in the future.
3. **MMIO Mapping**: Reads Base Address Register 0 (BAR0) to find the physical address of the device's Memory-Mapped I/O space. It then instructs the Virtual Memory Manager (VMM) to identity map a 1MB region at that address.
4. **MAC Retrieval**: Reads the hardware MAC Address from the `RAL` (Receive Address Low, `0x5400`) and `RAH` (Receive Address High, `0x5404`) device registers.

## Shell Integration

You can view the network adapter status and MAC address in the shell using the `netinfo` command.

```text
Orbit> netinfo
Intel E1000 (8254x) Gigabit Ethernet
MAC Address: 52:54:00:12:34:56
```

## Next Steps
Future iterations of the OS will allocate DMA memory for Transmit (TX) and Receive (RX) rings and implement a basic packet dispatcher for ARP and ICMP.
