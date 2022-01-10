// source: https://austinmorlan.com/posts/chip8_emulator/#what-is-an-emulator
#ifndef CHIP8_HPP
#define CHIP8_HPP
#include <cstdlib>
#include <iostream>

#define VIDEO_HEIGHT 32
#define VIDEO_WIDTH  64

class Chip8 {
    public:
        Chip8();
        void loadROM(const char*);      // load ROM data into memory
        void cycle();                   // execution cycle
        uint8_t  key[16]{};             // stores current state of keyboard keys 0-F.
        uint32_t screen[VIDEO_WIDTH * VIDEO_HEIGHT]{};   // stores on/off for pixels on screen

    private:
        // opcodes
        void OP_00E0();     // clear screen
        void OP_00EE();     // return
        void OP_1NNN();     // JMP addr
        void OP_2NNN();     // Call subroutine
        void OP_3XNN();     // skip if VX = NN
        void OP_4XNN();     // skip if VX != NN
        void OP_5XY0();     // skip if VX == VY
        void OP_6XNN();     // set VX to NN
        void OP_7XNN();     // VX = VX + NN
        void OP_8XY0();     // set VX = VY
        void OP_8XY1();     // set VX = VX OR VY
        void OP_8XY2();     // set VX = VX AND VY
        void OP_8XY3();     // set VX = VX XOR VY
        void OP_8XY4();     // set Vx = Vx + Vy, set VF = carry
        void OP_8XY5();     // set Vx = Vx - Vy, set VF = NOT borrow
        void OP_8XY6();     // VX = VX SHR 1
        void OP_8XY7();     // VX = VX - VY, set VF = NOT borrow
        void OP_8XYE();     // VX = VX SHL 1
        void OP_9XY0();     // skip if VX != VY
        void OP_ANNN();     // set I to NNN
        void OP_BNNN();     // JMP to NNN + V0
        void OP_CXNN();     // set VX = rand() & NN
        void OP_DXYN();     // draw sprite
        void OP_EX9E();     // if (key() == Vx), skip next direction
        void OP_EXA1();     // if (key() != Vx), skip next direction
        void OP_FX07();     // sets VX to value of delay timer
        void OP_FX0A();     // await key press and store in VX
        void OP_FX15();     // set delay timer to VX
        void OP_FX18();     // set sound timer to VX
        void OP_FX1E();     // I += VX
        void OP_FX29();     // I = sprite_addr[VX]
        void OP_FX33();     // *(I+0) = BCD(3); *(I+1) = BCD(2); *(I+2) = BCD(1);
        void OP_FX55();     // reg_dump(Vx, &I)
        void OP_FX65();     // reg_load(Vx, &I)

        // components of Chip-8 emulator
        uint16_t  opcode{};           // CPU instruction to be executed
        uint8_t   memory[4096]{};     // 4KB addressed 0x000 to 0xFFF
                                        // 0x000-0x1FF: intepreter
                                        //    -0x050-0x0A0: stores 16 built-in chars 0 to F
                                        // 0x200-0xFFF: instructions from ROM
        uint8_t   registers[16]{};    // 16 8-bit registers V0 to VF, each holding 0x00 to 0xFF 
        uint16_t  I{};                // used to store memory addresses for use in operations
        uint16_t  pc{};               // program counter: holds address of next instruction to execute
        uint16_t  stack[16]{};        // stack holding pc when entering subroutines
        uint8_t   sp{};               // holds place in stack where last data was entered, i.e. the "top"
        uint8_t   delay_timer{};      // used for timing
        uint8_t   sound_timer{};      // beeps when reaches 0
        uint8_t   random_num{};       // used for certain opcodes
        

        uint8_t fontset[80] = {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };    
};
#endif