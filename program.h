#include <cstdint>
#include <ostream>
#include <map>

namespace dvm {
    using namespace std;

    enum struct Opcode {
        RType  = 0x33,
        IType  = 0x13,
        Load   = 0x03,
        Store  = 0x23,
        Lui    = 0x37,
        Auipc  = 0x17,
        Jal    = 0x6f,
        Jalr   = 0x67,
        Branch = 0x63,
        System = 0x73,
    };

    enum struct FunctionType {
        Int    = 0x00,
        AltInt = 0x20,
    };

    enum struct RTypeCode {
        AddSub = 0x0,
        Sll    = 0x1,
        Slt    = 0x2,
        Sltu   = 0x3,
        Xor    = 0x4,
        SrlSra = 0x5,
        Or     = 0x6,
        And    = 0x7,
    };

    enum struct ITypeIntCode {
        Addi     = 0x0,
        Slti     = 0x2,
        Sltiu    = 0x3,
        Xori     = 0x4,
        Ori      = 0x6,
        Andi     = 0x7,
        Slli     = 0x1,
        SrliSrai = 0x5,
    };

    enum struct ITypeLoadCode {
        Lb  = 0x0,
        Lh  = 0x1,
        Lw  = 0x2,
        Lbu = 0x4,
        Lhu = 0x5, 
    };

    enum struct STypeCode {
        Sb = 0x0,
        Sh = 0x1,
        Sw = 0x2,
    };

    enum struct SbTypeCode {
        Beq  = 0x0,
        Bne  = 0x1,
        Blt  = 0x4,
        Bge  = 0x5,
        Bltu = 0x6,
        Bgeu = 0x7,
    };

    struct Memory {
        struct Page {
            uint8_t data[4096];
        };

        union UInt32ToUInt8 {
            uint32_t uint32;
            uint8_t  uint8[4];
        };

        std::map<uint32_t, Page*> page_table;

        uint32_t get4Bytes(uint8_t* table_data, uint32_t four_aligned_address) {
            UInt32ToUInt8 converter;

            #pragma unroll
            for (auto i = 0; i < 4; i++) {
                converter.uint8[i] = table_data[four_aligned_address + i];
            }

            return converter.uint32;
        }

        void set4Bytes(uint8_t* table_data, uint32_t four_aligned_address, uint32_t value) {
            UInt32ToUInt8 converter;
            converter.uint32 = value;

            #pragma unroll
            for (auto i = 0; i < 4; i++) {
                table_data[four_aligned_address + i] = converter.uint8[i];
            }
        }

        uint32_t read(uint32_t address) {
            auto table_address = address & ~0xfffu;
            auto four_aligned_address = address & 0xffcu;
            
            if (page_table.find(table_address) == page_table.end()) {
                auto table_data = page_table[table_address]->data;
                return get4Bytes(table_data, four_aligned_address);
            }
            else {
                page_table[table_address] = new Page;
                auto table_data = page_table[table_address]->data;
                return get4Bytes(table_data, four_aligned_address);
            }   
        }

        void write(uint32_t address, uint32_t value) {
            auto table_address = address & ~0xfffu;
            auto four_aligned_address = address & 0xffcu;
            
            if (page_table.find(table_address) == page_table.end()) {
                auto table_data = page_table[table_address]->data;
                set4Bytes(table_data, four_aligned_address, value);                
            }
            else {
                page_table[table_address] = new Page;
                auto table_data = page_table[table_address]->data;
                return set4Bytes(table_data, four_aligned_address, value);
            } 
        }
    };
        
    struct Machine {
        uint32_t registers[32];
        uint32_t pc;

        Memory memory;

        uint32_t readMemory(uint32_t address) {
            return memory.read(address);
        }

        void writeMemory(uint32_t address, uint32_t value) {
            memory.write(address, value);
        }

        uint32_t readRegister(size_t register_number) {
            if (register_number == 0) {
                return 0;
            }
            else {
                return registers[register_number];
            }
        }

        void writeRegister(size_t register_number, uint32_t value) {
            if (register_number != 0) {
                registers[register_number] = value;
            }
        }

        uint32_t readPc() {
            return pc;
        }

        void writePc(uint32_t value) {
            pc = value;
        }

        void add(size_t rd, size_t rs1, size_t rs2) {
            uint32_t ans = readRegister(rs1) + readRegister(rs2);
            writeRegister(rd, ans);
            pc = pc + 4;
        }

        void sub(size_t rd, size_t rs1, size_t rs2) {
            uint32_t ans = readRegister(rs1) - readRegister(rs2);
            writeRegister(rd, ans);
            pc = pc + 4;
        }

        void sll(size_t rd, size_t rs1, size_t rs2) {
            uint32_t ans = readRegister(rs1) << readRegister(rs2);
            writeRegister(rd, ans);
            pc = pc + 4;
        }

        void slt(size_t rd, size_t rs1, size_t rs2) {
            uint32_t ans = rs1_val < rs2_val;
            writeRegister(rd, ans);
            pc = pc + 4;
        }

        void sltu(size_t rd, size_t rs1, size_t rs2) {
            uint32_t ans = readRegister(rs1) < readRegister(rs2);
            writeRegister(rd, ans);
            pc = pc + 4;
        }

        void doXor(size_t rd, size_t rs1, size_t rs2) {
            uint32_t ans = readRegister(rs1) ^ readRegister(rs2);
            writeRegister(rd, ans);
            pc = pc + 4;
        }

        void srl(size_t rd, size_t rs1, size_t rs2) {
            uint32_t ans = readRegister(rs1) >> readRegister(rs2);
            writeRegister(rd, ans);
            pc = pc + 4;
        }

        void sra(size_t rd, size_t rs1, size_t rs2) {
            int32_t rs1_val = readRegister(rs1);
            int32_t rs2_val = readRegister(rs2);

            uint32_t ans = rs1_val >> rs2_val;
            writeRegister(rd, ans);
            pc = pc + 4;
        }

        void doOr(size_t rd, size_t rs1, size_t rs2) {
            uint32_t ans = readRegister(rs1) | readRegister(rs2);
            writeRegister(rd, ans);
            pc = pc + 4;
        }

        void doAnd(size_t rd, size_t rs1, size_t rs2) {
            uint32_t ans = readRegister(rs1) & readRegister(rs2);
            writeRegister(rd, ans);
            pc = pc + 4;
        }


        /*******I-type Immediate Arithmetic Instructions*******/

        void addi(size_t rd, size_t rs1, int32_t itype_imm) {
            uint32_t ans = readRegister(rs1) + itype_imm;
            writeRegister(rd, ans);
            pc = pc + 4;
        }

        void slti(size_t rd, size_t rs1, int32_t itype_imm) {
            uint32_t ans = (int32_t)readRegister(rs1) < itype_imm;
            writeRegister(rd, ans);
            pc = pc + 4;
        }

        void sltiu(size_t rd, size_t rs1, int32_t itype_imm) {
            uint32_t imm = itype_imm;
            uint32_t ans = readRegister(rs1) < imm;
            writeRegister(rd, ans);
            pc = pc + 4;
        }

        void xori(size_t rd, size_t rs1, int32_t itype_imm) {
            uint32_t ans = readRegister(rs1) ^ itype_imm;
            writeRegister(rd, ans);
            pc = pc + 4;
        }

        void ori(size_t rd, size_t rs1, int32_t itype_imm) {
            uint32_t ans = readRegister(rs1) | itype_imm;
            writeRegister(rd, ans);
            pc = pc + 4;
        }

        void andi(size_t rd, size_t rs1, int32_t itype_imm) {
            uint32_t ans = readRegister(rs1) & itype_imm;
            writeRegister(rd, ans);
            pc = pc + 4;
        }

        void slli(size_t rd, size_t rs1, int32_t itype_imm) {
            uint32_t ans = readRegister(rs1) << itype_imm;
            writeRegister(rd, ans);
            pc = pc + 4;
        }

        void srli(size_t rd, size_t rs1, int32_t itype_imm) {
            uint32_t imm = itype_imm;
            uint32_t ans = readRegister(rs1) >> imm;
            writeRegister(rd, ans);
            pc = pc + 4;
        }

        void srai(size_t rd, size_t rs1, int32_t itype_imm) {
            int32_t rs1_val = readRegister(rs1);
            uint32_t ans = rs1_val >> itype_imm;
            writeRegister(rd, ans);
            pc = pc + 4;
        }


        /*******U-type Load Instructions*******/

        void lui(size_t rd, uint32_t utype_imm) {
            writeRegister(rd, utype_imm);
            pc = pc + 4;
        }

        void auipc(size_t rd, uint32_t utype_imm) {
            writeRegister(rd, pc + utype_imm);
            pc = pc + 4;
        }


        /*******I-type Load Instructions*******/

        void lw(size_t rd, size_t rs1, int32_t itype_imm) {
            uint32_t read_word = readMemory(readRegister(rs1) + itype_imm);
            writeRegister(rd, read_word);
            pc = pc + 4;
        }

        void lh(size_t rd, size_t rs1, int32_t itype_imm) {
            uint32_t addr = readRegister(rs1) + itype_imm;
            uint32_t second_last_bit = (addr & 0x2) >> 1;
            uint32_t addr_aligned = addr & ~0x2; //switch off second last bit, align to 4
            uint32_t read_word = readMemory(addr_aligned);

            int32_t output = 0;

            if (second_last_bit == 0) 
            {
                output = (((int32_t)(read_word & 0xFFFF)) << 16) >> 16;
            }
            else 
            {
                output = ((int32_t)(read_word & 0xFFFF0000)) >> 16;
            }

            writeRegister(rd, output);
            pc = pc + 4;
        }

        void lb(size_t rd, size_t rs1, int32_t itype_imm) {
            uint32_t addr = readRegister(rs1) + itype_imm;
            uint32_t first_two_bits = (addr & 0x3);
            uint32_t addr_aligned = addr & ~0x3; //switch off last two bits
            uint32_t read_word = readMemory(addr_aligned);

            int32_t output = 0;

            uint32_t offset = first_two_bits * 8U;
            output = (((int32_t)(read_word & (0xFFU << offset))) << (24 - offset)) >> 24;

            writeRegister(rd, output);
            pc = pc + 4;
        }

        void lhu(size_t rd, size_t rs1, int32_t itype_imm) {
            uint32_t addr = readRegister(rs1) + itype_imm;
            uint32_t second_last_bit = (addr & 0x2) >> 1;
            uint32_t addr_aligned = addr & ~0x2; //switch off second last bit, align to 4
            uint32_t read_word = readMemory(addr_aligned);

            uint32_t output = 0;

            if (second_last_bit == 0) 
            {
                output = read_word & 0xFFFF;
            }
            else 
            {
                output = (read_word & 0xFFFF0000) >> 16U;
            }

            writeRegister(rd, output);
            pc = pc + 4;
        }

        void lbu(size_t rd, size_t rs1, int32_t itype_imm) {
            uint32_t addr = readRegister(rs1) + itype_imm;
            uint32_t first_two_bits = (addr & 0x3);
            uint32_t addr_aligned = addr & ~0x3; //switch off last two bits
            uint32_t read_word = readMemory(addr_aligned);

            uint32_t output = 0;

            uint32_t offset = first_two_bits * 8U;
            output = (read_word & (0xFFU << offset)) >> offset;

            writeRegister(rd, output);
            pc = pc + 4;
        }


        /*******S-type Store Instructions*******/

        void sw(size_t rs1, size_t rs2, int32_t stype_offset) {
            uint32_t addr = readRegister(rs1) + stype_offset;
            uint32_t word = readRegister(rs2);
            writeMemory(addr, word);
            pc = pc + 4;
        }

        void sh(size_t rs1, size_t rs2, int32_t stype_offset) {
            uint32_t addr = readRegister(rs1) + stype_offset;
            uint32_t second_last_bit = (addr & 0x2) >> 1;
            uint32_t addr_aligned = addr & ~0x2; //switch off second last bit, align to 4
            uint32_t read_word = readMemory(addr_aligned);
            uint32_t to_write = readRegister(rs2) & 0xFFFF;
            uint32_t output = 0;

            if (second_last_bit == 0) {
                output = (read_word & ~0xFFFF) | to_write;
            }
            else {
                output = (read_word & ~0xFFFF0000) | (to_write << 16);
            }

            writeMemory(addr_aligned, output);
            pc = pc + 4;
        }

        void sb(size_t rs1, size_t rs2, int32_t stype_offset) {
            uint32_t addr = readRegister(rs1) + stype_offset;
            uint32_t first_two_bits = (addr & 0x3);
            uint32_t addr_aligned = addr & ~0x3; //switch off last two bits
            uint32_t read_word = readMemory(addr_aligned);
            uint32_t to_write = readRegister(rs2) & 0xFF;

            uint32_t output = 0;

            uint32_t offset = first_two_bits * 8U;
            output = (read_word & ~(0xFFU << offset)) | (to_write << offset);

            writeMemory(addr_aligned, output);
            pc = pc + 4;
        }


        /*******J-type Jump*******/

        void jal(size_t rd, int32_t jtype_offset) {
            if (rd != 0) {
                writeRegister(rd, pc + 4);
            }

            pc = pc + jtype_offset;
        }


        /*******I-type Jump*******/

        void jalr(size_t rd, size_t rs1, int32_t itype_imm) {
            uint32_t addr = readRegister(rs1) + itype_imm;
            addr = addr & ~1;

            if (rd != 0) {
                writeRegister(rd, pc + 4);
            }

            pc = addr;
        }


        /*******B-type Jump*******/

        void beq(size_t rs1, size_t rs2, int32_t btype_offset) {
            uint32_t rs1_val = readRegister(rs1);
            uint32_t rs2_val = readRegister(rs2);

            if (rs1_val == rs2_val) {
                pc = pc + btype_offset;
            }
            else {
                pc = pc + 4;
            }
        }

        void bne(size_t rs1, size_t rs2, int32_t btype_offset) {
            uint32_t rs1_val = readRegister(rs1);
            uint32_t rs2_val = readRegister(rs2);

            if (rs1_val != rs2_val) {
                pc = pc + btype_offset;
            }
            else {
                pc = pc + 4;
            }
        }

        void blt(size_t rs1, size_t rs2, int32_t btype_offset) {
            int32_t rs1_val = readRegister(rs1);
            int32_t rs2_val = readRegister(rs2);

            if (rs1_val < rs2_val) {
                pc = pc + btype_offset;
            }
            else {
                pc = pc + 4;
            }
        }

        void bge(size_t rs1, size_t rs2, int32_t btype_offset) {
            int32_t rs1_val = readRegister(rs1);
            int32_t rs2_val = readRegister(rs2);

            if (rs1_val >= rs2_val) {
                pc = pc + btype_offset;
            }
            else {
                pc = pc + 4;
            }
        }

        void bltu(size_t rs1, size_t rs2, int32_t btype_offset) {
            uint32_t rs1_val = readRegister(rs1);
            uint32_t rs2_val = readRegister(rs2);

            if (rs1_val < rs2_val) {
                pc = pc + btype_offset;
            }
            else {
                pc = pc + 4;
            }
        }

        void bgeu(size_t rs1, size_t rs2, int32_t btype_offset) {
            uint32_t rs1_val = readRegister(rs1);
            uint32_t rs2_val = readRegister(rs2);

            if (rs1_val >= rs2_val) {
                pc = pc + btype_offset;
            }
            else {
                pc = pc + 4;
            }
        }


    };

    std::ostream& operator<<(std::ostream& o, const Machine& m) {
        o << "[DVM Machine]\n";
        for (auto i = 0; i < 32; i++) {
            o << "x" << i << ": ";

            if (i < 0x10) {
                o << " ";
            }

            o << hex << m.registers[i] << "\n";
        }
        o << endl;

        return o;
    }
} 
