set -xe

cc=gcc
cflags="-Wall -Wextra -DNDEBUG"

if [ ! -d ./build ]; then
    mkdir ./build
fi

$cc $cflags -o ./build/hxr-emu ./hxr-emu.c ./hxr.c
$cc $cflags -o ./build/hxr-asm ./hxr-asm.c ./hxr.c
./build/hxr-asm ./tests/basic.hxr ./tests/basic.hxs
