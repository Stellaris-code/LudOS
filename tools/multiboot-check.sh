 #!/bin/sh

if grub-file --is-x86-multiboot "$1"; then
  echo -e "Multiboot confirmed"
else
  echo -e "The file is not multiboot$" 1>&2
  exit 1
fi
