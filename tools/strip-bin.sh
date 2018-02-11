set -e
cp ../build/bin/LudOS.bin ../build/bin/stripped.bin
strip --strip-all ../build/bin/stripped.bin
echo "Copied stripped binary"
