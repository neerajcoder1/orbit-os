import subprocess
import time

print("Building Orbit OS...")
subprocess.run(["make", "clean"], check=True)
subprocess.run(["make", "iso"], check=True)

print("Starting QEMU...")
qemu = subprocess.Popen([
    "qemu-system-i386", "-cdrom", "orbitos.iso", 
    "-display", "none", "-serial", "file:serial.log",
    "-monitor", "stdio"
], stdin=subprocess.PIPE, text=True)

# Wait for QEMU and OS to boot
time.sleep(2)

print("Typing 'ls'...")
qemu.stdin.write("sendkey l\n")
qemu.stdin.flush()
time.sleep(0.1)
qemu.stdin.write("sendkey s\n")
qemu.stdin.flush()
time.sleep(0.1)
qemu.stdin.write("sendkey ret\n")
qemu.stdin.flush()

time.sleep(1)

print("Typing 'cat hello.txt'...")
qemu.stdin.write("sendkey c\n")
qemu.stdin.flush()
time.sleep(0.1)
qemu.stdin.write("sendkey a\n")
qemu.stdin.flush()
time.sleep(0.1)
qemu.stdin.write("sendkey t\n")
qemu.stdin.flush()
time.sleep(0.1)
qemu.stdin.write("sendkey spc\n")
qemu.stdin.flush()
time.sleep(0.1)
qemu.stdin.write("sendkey h\n")
qemu.stdin.flush()
time.sleep(0.1)
qemu.stdin.write("sendkey e\n")
qemu.stdin.flush()
time.sleep(0.1)
qemu.stdin.write("sendkey l\n")
qemu.stdin.flush()
time.sleep(0.1)
qemu.stdin.write("sendkey l\n")
qemu.stdin.flush()
time.sleep(0.1)
qemu.stdin.write("sendkey o\n")
qemu.stdin.flush()
time.sleep(0.1)
qemu.stdin.write("sendkey dot\n")
qemu.stdin.flush()
time.sleep(0.1)
qemu.stdin.write("sendkey t\n")
qemu.stdin.flush()
time.sleep(0.1)
qemu.stdin.write("sendkey x\n")
qemu.stdin.flush()
time.sleep(0.1)
qemu.stdin.write("sendkey t\n")
qemu.stdin.flush()
time.sleep(0.1)
qemu.stdin.write("sendkey ret\n")
qemu.stdin.flush()

time.sleep(2)
qemu.terminate()
qemu.wait()

# Check serial log
try:
    with open("serial.log", "r", encoding="utf-8", errors="replace") as f:
        log = f.read()
        print("Serial Log:\n" + log)
        if "Hello from the Orbit OS Initrd Filesystem!" in log:
            print("SUCCESS: Orbit OS loaded VFS, TarFS, and read the initrd via cat!")
            exit(0)
        else:
            print("FAILURE: Did not see hello.txt content.")
            exit(1)
except FileNotFoundError:
    print("FAILURE: serial.log not found")
    exit(1)
