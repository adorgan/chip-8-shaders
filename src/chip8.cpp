#include <cstdlib>
#include <fstream>
#include <chrono>
#include <random>
#include <cstring>
#include <time.h>

#include "chip8.hpp"

#define MEMORY_START 0x200
#define FONT_SIZE 80
#define FONT_START_ADDRESS 0x50

Chip8::Chip8(){
    // seed random number generator
    srand(time(NULL));

    pc = MEMORY_START;     // first address to be executed

    // load fonts to memory
    for(int i = 0; i < FONT_SIZE; i++){
        memory[FONT_START_ADDRESS + i] = fontset[i];
    }
}

void Chip8::loadROM(const char *filename){

    // open ROM file
    std::ifstream file;
    file.open(filename, std::ios::binary | std::ios::ate);

    if(file.is_open()){
        
        // Get size of file and allocate a buffer to hold the contents
		std::streampos size = file.tellg();
		char *buffer = new char[size];

		// Go back to the beginning of the file and fill the buffer
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		// Load the ROM contents into the Chip8's memory, starting at 0x200
		for (long i = 0; i < size; ++i)
		{
			memory[0x200 + i] = buffer[i];
		}

		// Free the buffer
		delete[] buffer;
    }
}

void Chip8::cycle(){
    //fetch instructions
    opcode = (memory[pc] << 8) | memory[pc + 1];

    // increment program counter
    pc += 2;

    // decode
    // get values for decoding
    uint16_t first_nibble = (opcode & 0xF000);
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t Y = (opcode & 0x00F0) >> 4;
    uint8_t N = (opcode & 0x000F);
    uint8_t NN = (opcode & 0x00FF);
    uint8_t NNN = (opcode & 0x0FFF);
    switch(first_nibble){
        case 0x0000:
            switch(opcode){
                case 0x00E0:
                    OP_00E0();
                    break;
                case 0x00EE:
                    OP_00EE();
                    break;
            }
            break;
        case 0x1000:
            OP_1NNN();
            break;
        case 0x2000:
            OP_2NNN();
            break;
        case 0x3000:
            OP_3XNN();
            break;
        case 0x4000:
            OP_4XNN();
            break;
        case 0x5000:
            OP_5XY0();
            break;
        case 0x6000:
            OP_6XNN();
            break;
        case 0x7000:
            OP_7XNN();
            break;
        case 0x8000:
            switch(N){
                case 0x00:
                    OP_8XY0();
                    break;
                case 0x01:
                    OP_8XY1();
                    break;
                case 0x02:
                    OP_8XY2();
                    break;
                case 0x03:
                    OP_8XY3();
                    break;
                case 0x04:
                    OP_8XY4();
                    break;
                case 0x05:
                    OP_8XY5();
                    break;
                case 0x06:
                    OP_8XY6();
                    break;
                case 0x07:
                    OP_8XY7();
                    break;
                case 0x0E:
                    OP_8XYE();
                    break;
            }
            break;
        case 0xA000:
            OP_ANNN();
            break;
        case 0xB000:
            OP_BNNN();
            break;
        case 0xC000:
            OP_CXNN();
            break;
        case 0xD000:
            OP_DXYN();
            break;
        case 0xE000:
            switch(N){
                case 0x0E:
                    OP_EX9E();
                    break;
                case 0x01:
                    OP_EXA1();
                    break;
            }
            break;
        case 0xF000:
            switch(NN){
                case 0x07:{
                    OP_FX07();
                    break;
                }
                case 0x0A:{
                    OP_FX0A();
                    break;
                }
                case 0x15:{
                    OP_FX15();
                    break;
                }
                case 0x18:{
                    OP_FX18();
                    break;
                }
                case 0x1E:{
                    OP_FX1E();
                    break;
                }
                case 0x29:{
                    OP_FX29();
                    break;
                }
                case 0x33:{
                    OP_FX33();
                    break;
                }
                case 0x55:{
                    OP_FX55();
                    break;
                }
                case 0x65:{
                    OP_FX65();
                    break;
                }
            }
    }

    // Decrement the delay timer if it's been set
	if (delay_timer > 0){
		delay_timer--;
	}

	// Decrement the sound timer if it's been set
	if (sound_timer > 0){
		sound_timer--;
	}
}

/* opcodes */
// clear
void Chip8::OP_00E0(){

    // set pixels to 0
    std::memset(screen, 0, sizeof(screen));
}

// return from subroutine
void Chip8::OP_00EE(){
    // decrement stack pointer
    sp--;

    // pop topmost address off stack and put in program counter
    pc = stack[sp];
}

// jump to NNN
void Chip8::OP_1NNN(){
    // use bitmask to find last 3 bytes of opcode
    uint16_t temp = opcode & 0x0FFF;
    pc = temp;
}

// call subroutine
void Chip8::OP_2NNN(){
    // find address of subroutine
    uint16_t addr = opcode & 0x0FFF;
    // place current pc on top of stack
    stack[sp] = pc;
    // increment stack pointer
    sp++;
    // set pc to subroutine address
    pc = addr;
}

// skip next address if VX = NN
void Chip8::OP_3XNN(){
    // get NN
    uint8_t NN = opcode & 0x00FF;
    // find register
    uint8_t VX  = (opcode & 0x0F00) >> 8;
    // skip next instruction if equal
    if(registers[VX] == NN){
        pc += 2;
    }
}

// skip next address if VX != NN
void Chip8::OP_4XNN(){
    // get NN
    uint8_t NN = opcode & 0x00FF;
    // find register
    uint8_t VX  = (opcode & 0x0F00) >> 8;
    // skip next instruction if equal
    if(registers[VX] != NN){
        pc += 2;
    }
}

// skip next address if VX == VY
void Chip8::OP_5XY0(){
    // get VX
    uint8_t VX = (opcode & 0x0F00) >> 8;
    // get VY
    uint8_t VY = (opcode & 0x00F0) >> 4;
    // skip next address if equal
    if(registers[VX] == registers[VY]){
        pc += 2;
    }
}

// set VX to NN
void Chip8::OP_6XNN(){
    // get VX
    uint8_t VX = (opcode & 0x0F00) >> 8;
    // get NN
    uint8_t NN = (opcode & 0x00FF);
    // set VX to NN
    registers[VX] = NN;
}

// set VX = VX + NN
void Chip8::OP_7XNN(){
    // get VX
    uint8_t VX = (opcode & 0x0F00) >> 8;
    // get NN
    uint8_t NN = (opcode & 0x00FF);
    // set VX = VX + NN
    registers[VX] = registers[VX] + NN;
}

// set VX = VY
void Chip8::OP_8XY0(){
    // get VX
    uint8_t VX = (opcode & 0x0F00) >> 8;
    // get VY
    uint8_t VY = (opcode & 0x00F0) >> 4;
    // set VX = VY
    registers[VX] = registers[VY];
}

// set VX = VX OR VY
void Chip8::OP_8XY1(){
    // get VX
    uint8_t VX = (opcode & 0x0F00) >> 8;
    // get VY
    uint8_t VY = (opcode & 0x00F0) >> 4;
    // set VX = VX OR VY
    registers[VX] = (registers[VX] | registers[VY]);
}

// set VX = VX AND VY
void Chip8::OP_8XY2(){
    // get VX
    uint8_t VX = (opcode & 0x0F00) >> 8;
    // get VY
    uint8_t VY = (opcode & 0x00F0) >> 4;
    // set VX = VX OR VY
    registers[VX] = (registers[VX] & registers[VY]);
}

// set VX = VX XOR VY
void Chip8::OP_8XY3(){
    // get VX
    uint8_t VX = (opcode & 0x0F00) >> 8;
    // get VY
    uint8_t VY = (opcode & 0x00F0) >> 4;
    // set VX = VX XOR VY
    registers[VX] = (registers[VX] ^ registers[VY]);
}

// set VX = VX + VY, set VF = carry
void Chip8::OP_8XY4(){
    // get VX
    uint8_t VX = (opcode & 0x0F00) >> 8;
    // get VY
    uint8_t VY = (opcode & 0x00F0) >> 4;
    
    // set carry flag
    if(registers[VX] + registers[VY] > 255){
        // overflow
        registers[0xF] = 1;
    }
    else{
        registers[0xF] = 0;
    }

    // add last 8 bits to register
    registers[VX] = (registers[VX] + registers[VY]) & 0xFF;
}

// set VX = VX - VY, set VF = NOT borrow
void Chip8::OP_8XY5(){
    // get VX
    uint8_t VX = (opcode & 0x0F00) >> 8;
    // get VY
    uint8_t VY = (opcode & 0x00F0) >> 4;
    // set NOT borrow flag
    if(registers[VX] > registers[VY]){
        registers[0xF] = 1;
    }
    else{
        registers[0xF] = 0;
    }

    // add last 8 bits to register
    registers[VX] = registers[VX] - registers[VY];
}

// set VX = VX shift right 1
void Chip8::OP_8XY6(){
    // get VX
    uint8_t VX = (opcode & 0x0F00) >> 8;
    
    // set VF to least significant bit
    registers[0XF] = registers[VX] & 0x1;

    // shift 1 to right (divide by 2)
    registers[VX] = (registers[VX] >> 1);
}

// set VX = VX - VY, set VF to NOT borrow
void Chip8::OP_8XY7(){
    // get VX
    uint8_t VX = (opcode & 0x0F00) >> 8;
    // get VY
    uint8_t VY = (opcode & 0x00F0) >> 4;
    // set NOT borrow flag
    if(registers[VY] > registers[VX]){
        registers[0xF] = 1;
    }
    else{
        registers[0xF] = 0;
    }

    // add last 8 bits to register
    registers[VX] = registers[VY] - registers[VX];
}

// set VX = VX shift left 1, sets VF to most significant bit
void Chip8::OP_8XYE(){
    // get VX
    uint8_t VX = (opcode & 0x0F00) >> 8;
    
    // set VF to most significant bit
    registers[0XF] = (registers[VX] & 0x80) >> 7;

    // shift 1 to left (multiply by 2)
    registers[VX] = (registers[VX] << 1);
}

// skip next address if VX != VY
void Chip8::OP_9XY0(){
    // get VX
    uint8_t VX = (opcode & 0x0F00) >> 8;
    // get VY
    uint8_t VY = (opcode & 0x00F0) >> 4;
    // skip next address if not equal
    if(registers[VX] != registers[VY]){
        pc += 2;
    }
}

// set I to NNN
void Chip8::OP_ANNN(){
    // get NNN
    uint16_t NNN = opcode & 0x0FFF;
    // set I
    I = NNN;
}

// jump to NNN + V0
void Chip8::OP_BNNN(){
    // get NNN
    uint16_t NNN = opcode & 0x0FFF;
    // set next address 
    pc = registers[0x00] + NNN;
}

// set VX = rand() & NN
void Chip8::OP_CXNN(){
    // get NN
    uint8_t NN = opcode & 0x00FF;
    // get VX
    uint8_t X  = (opcode & 0x0F00) >> 8;
    // generate random number from 0 to 255
    random_num = rand() % 255;
    // assign VX
    registers[X] = random_num & NN;
}

// draw sprite
void Chip8::OP_DXYN(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	uint8_t height = opcode & 0x000Fu;

	// Wrap if going beyond screen boundaries
	uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
	uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

	registers[0xF] = 0;

	for (unsigned int row = 0; row < height; ++row)
	{
		uint8_t spriteByte = memory[I + row];

		for (unsigned int col = 0; col < 8; ++col)
		{
			uint8_t spritePixel = spriteByte & (0x80u >> col);
			uint32_t* screenPixel = &screen[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

			// Sprite pixel is on
			if (spritePixel)
			{
				// Screen pixel also on - collision
				if (*screenPixel == 0xFFFFFFFF)
				{
					registers[0xF] = 1;
				}

				// Effectively XOR with the sprite pixel
				*screenPixel ^= 0xFFFFFFFF;
			}
		}
	}
}

// if (key() == Vx), skip next instruction
void Chip8::OP_EX9E(){
    // get X
    uint8_t X = (opcode & 0x0F00) >> 8;
    // check if key stored in VX is pressed
    if(key[registers[X]]){
        pc += 2;
    }
}

// if (key() != Vx), skip next instruction
void Chip8::OP_EXA1(){
    // get X
    uint8_t X = (opcode & 0x0F00) >> 8;
    // check if key stored in VX is not pressed
    if(!key[registers[X]]){
        pc += 2;
    }
}

// set VX to value of delay timer
void Chip8::OP_FX07(){
    // get X
    uint8_t X = (opcode & 0x0F00) >> 8;
    // set VX to delay timer
    registers[X] = delay_timer;
}

// await key press and store key in VX
void Chip8::OP_FX0A(){
    // get X
    uint8_t X = (opcode & 0x0F00) >> 8;

    // set bool to track key presses
    bool key_is_pressed = false;

    // loop until a key is pressed
    while(!key_is_pressed){
        // iterate through each key to see if it is pressed
        for(int i = 0; i < 16; i++){
            if(key[i]){
                // store key and exit loop
                key_is_pressed = true;
                registers[X] = key[i];
                break;
            }
        }
    }
}

// set delay timer to VX
void Chip8::OP_FX15(){
    // get X
    uint8_t X = (opcode & 0x0F00) >> 8;
    // set delay timer
    delay_timer = registers[X];
}

// set sound timer to VX
void Chip8::OP_FX18(){
    // get X
    uint8_t X = (opcode & 0x0F00) >> 8;
    // set sound timer
    sound_timer = registers[X];
}

// I += VX
void Chip8::OP_FX1E(){
    // get X
    uint8_t X = (opcode & 0x0F00) >> 8;
    // set index
    I += registers[X];
}

// I = sprite_addr[VX]
void Chip8::OP_FX29(){
    // get X
    uint8_t X = (opcode & 0x0F00) >> 8;

    // set I to offset (each char sprite is 5 bytes)
    I = FONT_START_ADDRESS + (5 * registers[X]);
}

// *(I+0) = BCD(3); *(I+1) = BCD(2); *(I+2) = BCD(1);
void Chip8::OP_FX33(){
    // get X
    uint8_t X = (opcode & 0x0F00) >> 8; 
    uint8_t val = registers[X];

    // store ones place in VX
    memory[I + 2] = val % 10;
    val = val/10;
    // store tens place in VX
    memory[I + 1] = val % 10;
    val = val/10;
    // store hundreds place in VX
    memory[I] = val % 10;
}

// stores V0 to VX in memory at index I offsets incremented by 1
void Chip8::OP_FX55(){
    // get X
    uint8_t X = (opcode & 0x0F00) >> 8;

    // add V0 to VX to memory offset by 1
    for(uint8_t i = 0; i <= X; i++){
        memory[I + i] = registers[i];
    }
}

// fulls V0 to VX from memory at index I with offsets incremented by 1
void Chip8::OP_FX65(){
    // get X
    uint8_t X = (opcode & 0x0F00) >> 8;

    // fill V0 to VX from memory offset by 1
    for(uint8_t i = 0; i <= X; i++){
        registers[i] = memory[I + i];
    }
}