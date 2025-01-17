#!/bin/bash

IMAGE="storage_vgc.img"
DEVICE="/dev/loop0"
MOUNT_DIR="mount"
BIN_DIR="bin" 


if [ ! -d "$MOUNT_DIR" ]; then
    mkdir $MOUNT_DIR
fi

sudo losetup $DEVICE $IMAGE
mount $DEVICE $MOUNT_DIR

if [ -d "$BIN_DIR" ]; then
    cp -r $BIN_DIR/* $MOUNT_DIR/
    echo "Binaries has been copied succesfully."
fi

ln -sf $DEVICE ./device_file

echo "Disk has been mounted."



