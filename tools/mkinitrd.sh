 
#!/bin/bash

rm -rf ../initrd/test_programs/*
cp ../userland/test_programs/bin/* ../initrd/test_programs/
tar -cf ../build/bin/initrd.tar ../initrd

echo "initrd.tar created."
 
