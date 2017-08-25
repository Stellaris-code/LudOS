 #!/bin/sh

RED='\033[0;31m'
NC='\033[0m' # No Color

if grub-file --is-x86-multiboot "$1"; then
  echo -e "Multiboot confirmed"
else
  echo -e "${RED}The file is not multiboot${NC}"
  exit 1
fi
