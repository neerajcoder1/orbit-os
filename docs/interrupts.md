# CPU and Interrupts (Phase 4)

## Global Descriptor Table (GDT)
The GDT defines the memory segments for the operating system. Orbit OS uses a flat memory model, defining three segments:
1. **Null Segment**: Required by the CPU.
2. **Code Segment**: Ring 0 kernel code spanning the entire 4GB address space.
3. **Data Segment**: Ring 0 kernel data spanning the entire 4GB address space.

The GDT is loaded via the `lgdt` instruction in `kernel/interrupts.s`.

## Interrupt Descriptor Table (IDT)
The IDT maps hardware and software interrupts to specific Interrupt Service Routines (ISRs). Orbit OS initializes a 256-entry table and populates the first 32 entries with CPU exception handlers (0-31) and the next 16 entries (32-47) with hardware IRQs mapped from the PIC.

## Interrupt Service Routines (ISRs & IRQs)
- **ISRs**: Handle CPU exceptions (e.g., division by zero, page faults).
- **IRQs**: Handle hardware interrupts.
All interrupts trigger a corresponding assembly stub in `interrupts.s`, which saves the processor state (registers) and calls the central C-level `isr_handler` or `irq_handler`.

## Programmable Interrupt Controller (PIC)
The legacy 8259 PIC chips are remapped during initialization. By default, they map hardware interrupts to 0-15, which conflicts with CPU exceptions. We remap the master PIC to offset 32 and the slave PIC to offset 40, ensuring hardware interrupts (like the timer and keyboard) safely trigger IRQ0-IRQ15.

## Programmable Interval Timer (PIT)
The PIT is a hardware timer connected to IRQ0 (Interrupt 32). Orbit OS configures the PIT to fire at a specified frequency (e.g., 100 Hz). The `pit_callback` increments a tick counter on every firing, which will eventually form the basis of process scheduling and uptime tracking.
