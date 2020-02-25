#include "program.h"
#include <iostream>

using namespace std;
using namespace dvm;

int main(int argc, char** argv) {
    Machine machine;
    uint32_t instr = 0xfe010113u;
    auto opcode = static_cast<Opcode>(instr & 0x7F);
    auto function_type  = static_cast<FunctionType>((instr >> 25) & 0x7F);
    size_t rs1 = (instr >> 15) & 0x1F;
    size_t rs2 = (instr >> 20) & 0x1F;
    size_t rd = (instr >> 7) & 0x1F;

    auto itype_int_code = static_cast<ITypeIntCode>((instr >> 12) & 0x7);
    int32_t itype_imm = ((int32_t)instr) >> 20;
    auto itype_load_code = static_cast<ITypeLoadCode>((instr >> 12) & 0x7);
    auto stype_code = static_cast<STypeCode>((instr >> 12) & 0x7);
    uint32_t utype_imm = instr & ~0xFFF;
    RTypeCode rtype_code = static_cast<RTypeCode>((instr >> 12) & 0x7);

    //Decode the S-type store offset
    int32_t stype_offset = (((int32_t)instr) >> 20) & ~0x1F;
    stype_offset |= rd;

    //Decode J-type offset
    uint32_t jtype_temp = 0;
    jtype_temp |= (instr >> 20) & 0x7FE;
    jtype_temp |= (instr >> 9) & (1 << 11);
    jtype_temp |= instr & (0xFF << 12);
    jtype_temp |= (instr >> 11) & (1 << 20);

    int32_t jtype_offset = jtype_temp;
    jtype_offset = (jtype_offset << 11) >> 11;

    //Decode B-type offset
    uint32_t btype_temp = 0;
    btype_temp |= (instr >> 7) & (0xF << 1);
    btype_temp |= (instr >> 20) & (0x3F << 5);
    btype_temp |= (instr << 4) & (1 << 11);
    btype_temp |= (instr >> 19) & (1 << 12);

    int32_t btype_offset = btype_temp;
    btype_offset = (btype_offset << 19) >> 19;

    //Decode instruction as SB-type (branch)
    auto sbtype_code = static_cast<SbTypeCode>((instr >> 12) & 0x7);

    switch (opcode) {
        case Opcode::RType:
            cout << "arith\n";
            break;
        
        case Opcode::IType:
            cout << "arith imm\n";
            break;

        default:
            cout << "unknown\n";
            break;
    }
 
    return 0;
}
