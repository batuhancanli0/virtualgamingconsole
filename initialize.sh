#!/bin/bash

IMAGE_FILE="storage_vgc.img"

if [ -f "$IMAGE_FILE" ]; then
    echo "Overriding existing image file: $IMAGE_FILE"
    rm -f "$IMAGE_FILE"
fi

echo "Creating disk image file: $IMAGE_FILE"
dd if=/dev/zero of=$IMAGE_FILE bs=1M count=64
sudo mkfs.ext4 $IMAGE_FILE
echo "Disk image created successfully."




