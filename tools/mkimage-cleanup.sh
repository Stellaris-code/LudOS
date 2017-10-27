#!/bin/bash

echo "Cleaning up"
umount /mnt
kpartx -d /dev/mapper/hdd
dmsetup remove hdd
losetup -d /dev/loop3
