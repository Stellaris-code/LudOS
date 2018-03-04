#!/bin/bash

echo "Cleaning up"
umount /mnt
umount /dev/mapper/hdd1
kpartx -d /dev/mapper/hdd
dmsetup remove hdd
losetup -d /dev/loop3
