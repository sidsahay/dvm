riscv32-unknown-elf-gcc -lgcc -static -nostdlib -nostartfiles -march=rv32i -mabi=ilp32 -Wall -Wextra -std=c11 -pedantic -g -Werror=implicit-function-declaration $1
