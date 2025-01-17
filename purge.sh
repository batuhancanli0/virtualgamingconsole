#!/bin/bash

IMAGE_FILE="storage_vgc.img"

echo "Running terminate.sh"
sudo ./terminate.sh

if [ -f "$IMAGE_FILE" ]; then
    echo "Deleting disk image file: $IMAGE_FILE"
    rm -f $IMAGE_FILE
fi

echo "Purge completed successfully."

