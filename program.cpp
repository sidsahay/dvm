#include "program.h"
#include <iostream>

using namespace std;
using namespace dvm;

int main(int argc, char** argv) {
    Machine machine;
    uint32_t instr = 0x
    auto opcode = static_cast<Opcode>(instr & 0x7F);
    funct7_t funct7 = (instr >> 25) & 0x7F;
    riscv_isa_reg_t rs1 = (instr >> 15) & 0x1F;
    riscv_isa_reg_t rs2 = (instr >> 20) & 0x1F;
    riscv_isa_reg_t rd = (instr >> 7) & 0x1F;


    cout << "hello, world\n" << machine << endl;
    return 0;
}
