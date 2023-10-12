#ifndef HXR_H
#define HXR_H

#include <stdint.h>
#define HXR_MEMORY_CAPACITY (1 * 1024 * 1024)
#define HXR_INSTRUCTIONS_START (1 * 40 * 1024)
#define HXR_HEAP_BASE (2 * 40 * 1024)

typedef struct {
    uint8_t mem[HXR_MEMORY_CAPACITY];
    uint16_t r[8];
    uint16_t ip; // instruction pointer
    uint16_t sp; // stack pointer
    uint8_t halt;
} HXR;

// memory utilities
uint16_t hxr_load(HXR* cpu, uint16_t addr, uint16_t size);
uint16_t hxr_load_8(HXR* cpu, uint16_t addr);
uint16_t hxr_load_16(HXR* cpu, uint16_t addr);
void hxr_store(HXR* cpu, uint16_t addr, uint16_t size, uint16_t value);
void hxr_store_8(HXR* cpu, uint16_t addr, uint16_t value);
void hxr_store_16(HXR* cpu, uint16_t addr, uint16_t value);

// instruction decoder
uint16_t opcode(uint16_t inst); // first 5 bit
uint16_t ra(uint16_t inst); // 3 bit after opcode
uint16_t rb(uint16_t inst); // 3 bit after ra
uint16_t imm_11(uint16_t inst); // last 11 bit
uint16_t imm_8(uint16_t inst); // last 8 bit

int hxr_init(HXR* cpu, const char* filepath);
uint16_t hxr_fetch(HXR* cpu);
int hxr_execute(HXR* cpu, uint16_t inst);
void hxr_dump_registers(HXR* cpu);

#define MOV  0x00
#define MOVI 0x01
#define CMP  0x02
#define JE   0x03
#define JN   0x04
#define JL   0x05
#define JG   0x06
#define ADD  0x07
#define SUB  0x08
#define MOD  0x09
#define ADDI 0x0A
#define SUBI 0x0B
#define MODI 0x0C
#define AND  0x0D
#define OR   0x0E
#define XOR  0x0F
#define BSL  0x10
#define BSR  0x11
#define BSLI 0x12
#define BSRI 0x13
#define LDW  0x14
#define STW  0x15
#define LDB  0x16
#define STB  0x17
#define PUSH  0x18
#define POP  0x19
#define HALT  0x1A

#endif // HXR_H
