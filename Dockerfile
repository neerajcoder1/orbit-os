FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y \
    build-essential \
    gcc-multilib \
    grub-common \
    grub-pc-bin \
    xorriso \
    mtools \
    qemu-system-x86 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /orbit-os
