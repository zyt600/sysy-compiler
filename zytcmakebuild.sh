cmake -DCMAKE_BUILD_TYPE=Debug -B build
cmake --build build
build/compiler -koopa ./mydebug/hello.c -o hello.koopa
# build/compiler -riscv ./mydebug/hello.c -o hello.koopa
