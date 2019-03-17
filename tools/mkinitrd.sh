 
#!/bin/bash

rm -rf ../initrd/test_programs/*
rm -rf ../initrd/user_programs/*
cp ../userland/test_programs/bin/* ../initrd/test_programs/
cp ../userland/user_programs/bin/* ../initrd/user_programs/
cp ../initrd/greet.sh ../initrd/user_programs/
tar -cf ../build/bin/initrd.tar ../initrd

echo "initrd.tar created."
 
