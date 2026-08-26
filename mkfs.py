import sys
import struct
import os

if len(sys.argv) < 3:
    print("Usage: python3 mkfs.py <output_file> <directory>")
    sys.exit(1)

out_file = sys.argv[1]
in_dir = sys.argv[2]

files = []
for f in os.listdir(in_dir):
    path = os.path.join(in_dir, f)
    if os.path.isfile(path):
        with open(path, 'rb') as f_in:
            data = f_in.read()
            files.append((f.encode('ascii'), data))

# Header: [Magic:4] [NumFiles:4]
# For each file: [Name:64] [Offset:4] [Length:4]

out = open(out_file, 'wb')
out.write(b'ORBT')
out.write(struct.pack('<I', len(files)))

header_size = 8 + len(files) * (64 + 4 + 4)
current_offset = header_size

for name, data in files:
    padded_name = name[:63] + b'\0' * (64 - len(name[:63]))
    out.write(padded_name)
    out.write(struct.pack('<I', current_offset))
    out.write(struct.pack('<I', len(data)))
    current_offset += len(data)

for name, data in files:
    out.write(data)

out.close()
