#!/bin/bash
make clean
make iso
if [ $? -ne 0 ]; then
    echo "Make failed"
    exit 1
fi

echo "Running QEMU..."
# Run QEMU with a 15 second timeout, capture stdout to a file
timeout 15 qemu-system-i386 -cdrom orbitos.iso -nographic > test_output.log 2>&1

if grep -q "Unhandled CPU Exception!" test_output.log; then
    echo "SUCCESS: Orbit OS booted and handled an interrupt!"
    exit 0
else
    echo "FAILURE: Could not find interrupt message."
    exit 1
fi
