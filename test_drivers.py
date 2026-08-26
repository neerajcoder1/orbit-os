import subprocess
import time
import sys

def main():
    print("Building Orbit OS...")
    subprocess.run(["make", "clean"])
    subprocess.run(["make", "iso"])
    
    print("Starting QEMU...")
    qemu_cmd = [
        "qemu-system-i386",
        "-cdrom", "orbitos.iso",
        "-serial", "file:serial.log",
        "-display", "none",
        "-monitor", "stdio"
    ]
    
    process = subprocess.Popen(qemu_cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    
    # Wait for boot
    time.sleep(2)
    
    # Type 'date'
    print("Typing 'date'...")
    for c in "date\n":
        process.stdin.write(f"sendkey {c if c != '\n' else 'ret'}\n")
        process.stdin.flush()
        time.sleep(0.1)
        
    time.sleep(1)
    
    # Type 'lspci'
    print("Typing 'lspci'...")
    for c in "lspci\n":
        process.stdin.write(f"sendkey {c if c != '\n' else 'ret'}\n")
        process.stdin.flush()
        time.sleep(0.1)
        
    # Type 'netinfo'
    print("Typing 'netinfo'...")
    for c in "netinfo\n":
        process.stdin.write(f"sendkey {c if c != '\n' else 'ret'}\n")
        process.stdin.flush()
        time.sleep(0.1)
        
    time.sleep(2)
    
    process.terminate()
    process.wait()
    
    # Check serial log
    try:
        with open("serial.log", "r", encoding="utf-8", errors="replace") as f:
            log = f.read()
            print("Serial Log:\n" + log)
            if "MAC Address: " in log:
                print("SUCCESS: Network E1000 initialized successfully!")
                sys.exit(0)
            else:
                print("FAILURE: MAC Address output not found.")
                sys.exit(1)
    except FileNotFoundError:
        print("FAILURE: serial.log not found")
        sys.exit(1)

if __name__ == "__main__":
    main()
