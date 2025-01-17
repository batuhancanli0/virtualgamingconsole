#!/bin/bash

MOUNT_DIR="mount"
DEVICE_FILE="/dev/loop0"
SYMLINK="device_file"

if mount | grep $MOUNT_DIR > /dev/null; then
    echo "Unmounting $MOUNT_DIR"
    umount $MOUNT_DIR
fi

if sudo losetup -a | grep $DEVICE_FILE > /dev/null; then
    echo "Detaching loop device: $DEVICE_FILE"
    sudo losetup -d $DEVICE_FILE
fi

if [ -L "$SYMLINK" ]; then
    echo "Removing symbolic link: $SYMLINK"
    rm -f $SYMLINK
fi


echo "Termination completed successfully."




