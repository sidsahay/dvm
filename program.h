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

    enum struct RTypeFunctionCode {
        AddSub = 0x0,
        Sll    = 0x1,
        Slt    = 0x2,
        Sltu   = 0x3,
        Xor    = 0x4,
        SrlSra = 0x5,
        Or     = 0x6,
        And    = 0x7,
    };

    enum struct ITypeIntFunctionCode {
        Addi     = 0x0,
        Slti     = 0x2,
        Sltiu    = 0x3,
        Xori     = 0x4,
        Ori      = 0x6,
        Andi     = 0x7,
        Slli     = 0x1,
        SrliSrai = 0x5,
    };

    enum struct STypeFunctionCode {
        Sb = 0x0,
        Sh = 0x1,
        Sw = 0x2,
    };

    enum struct SbTypeFunctionCode {
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
            return registers[register_number];
        }

        void writeRegister(size_t register_number, uint32_t value) {
            registers[register_number] = value;
        }

        uint32_t readPc() {
            return pc;
        }

        void writePc(uint32_t value) {
            pc = value;
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
