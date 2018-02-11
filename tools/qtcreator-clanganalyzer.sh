set -e
echo "$@"
exec /usr/tooLargeForHome/qt/qtcreator-4.4.0-beta1/libexec/qtcreator/clang/bin/clang -ffreestanding "$@"
exit 0
