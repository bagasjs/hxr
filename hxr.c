#include "hxr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NDEBUG
    #define DEBUG_LOG(FMT, ...) printf(FMT, __VA_ARGS__)
#else
    #define DEBUG_LOG(FMT, ...)
#endif

int hxr_execute(HXR* cpu, uint16_t inst)
{
    switch(opcode(inst)) {
        case MOV:
            {
                cpu->r[ra(inst)] = cpu->r[rb(inst)];
                DEBUG_LOG("Running \"%s\"\n", "MOV");
            } break;
        case MOVI:
            {
                cpu->r[ra(inst)] = imm_8(inst);
                DEBUG_LOG("Running \"%s\"\n r%u = %u\n", "MOVI", ra(inst), cpu->r[ra(inst)]);
            } break;
        case JE:
            {
                if(cpu->r[0] == 1) {
                    cpu->ip = imm_11(inst);
                }
                DEBUG_LOG("Running \"%s\"\n", "JE");
            } break;
        case JN:
            {
                if(cpu->r[0] != 1) {
                    cpu->ip = imm_11(inst);
                }

                DEBUG_LOG("Running \"%s\"\n", "JN");
            } break;
        case JG:
            {
                if(cpu->r[0] > 1) {
                    cpu->ip = imm_11(inst);
                }
                DEBUG_LOG("Running \"%s\"\n", "JG");
            } break;
        case JL:
            {
                if((int)cpu->r[0] < 1) {
                    cpu->ip = imm_11(inst);
                }
                DEBUG_LOG("Running \"%s\"\n", "JL");
            } break;
        case CMP:
            {
                uint16_t a = cpu->r[ra(inst)];
                uint16_t b = cpu->r[rb(inst)];
                cpu->r[0] = a < b ? 0 : a - b + 1;
                DEBUG_LOG("Running \"%s\"\n", "CMP");
            } break;
        case ADD:
            {
                cpu->r[ra(inst)] += cpu->r[rb(inst)];
                DEBUG_LOG("Running \"%s\"\n", "ADD");
            } break;
        case ADDI:
            {
                cpu->r[ra(inst)] += imm_8(inst);
                DEBUG_LOG("Running \"%s\"\n", "ADDI");
            } break;
        case SUB:
            {
                cpu->r[ra(inst)] -= cpu->r[rb(inst)];
                DEBUG_LOG("Running \"%s\"\n", "SUB");
            } break;
        case SUBI:
            {
                cpu->r[ra(inst)] -= imm_8(inst);
                DEBUG_LOG("Running \"%s\"\n", "SUBI");
            } break;
        case MOD:
            {
                cpu->r[ra(inst)] %= cpu->r[rb(inst)];
                DEBUG_LOG("Running \"%s\"\n", "MOD");
            } break;
        case MODI:
            {
                cpu->r[ra(inst)] %= imm_8(inst);
                DEBUG_LOG("Running \"%s\"\n", "MODI");
            } break;

        case AND:
            {
                cpu->r[ra(inst)] &= cpu->r[rb(inst)];
                DEBUG_LOG("Running \"%s\"\n", "AND");
            } break;
        case OR:
            {
                cpu->r[ra(inst)] |= cpu->r[rb(inst)];
                DEBUG_LOG("Running \"%s\"\n", "OR");
            } break;
        case XOR:
            {
                cpu->r[ra(inst)] ^= cpu->r[rb(inst)];
                DEBUG_LOG("Running \"%s\"\n", "XOR");
            } break;
        case BSL:
            {
                cpu->r[ra(inst)] <<= cpu->r[rb(inst)];
                DEBUG_LOG("Running \"%s\"\n", "BSL");
            } break;
        case BSR:
            {
                cpu->r[ra(inst)] >>= cpu->r[rb(inst)];
                DEBUG_LOG("Running \"%s\"\n", "BSR");
            } break;
        case BSLI:
            {
                cpu->r[ra(inst)] <<= imm_8(inst);
                DEBUG_LOG("Running \"%s\"\n", "BSLI");
            } break;
        case BSRI:
            {
                cpu->r[ra(inst)] >>= imm_8(inst);
                DEBUG_LOG("Running \"%s\"\n", "BSRI");
            } break;
        case LDW:
            {
                cpu->r[ra(inst)] = hxr_load(cpu, cpu->r[rb(inst)], 16);
                DEBUG_LOG("Running \"%s\"\n", "LDW");
            } break;
        case STW:
            {
                hxr_store(cpu, cpu->r[rb(inst)], 16, cpu->r[ra(inst)]);
                DEBUG_LOG("Running \"%s\"\n", "STW");
            } break;
        case LDB:
            {
                cpu->r[ra(inst)] = hxr_load(cpu, cpu->r[rb(inst)], 8);
                DEBUG_LOG("Running \"%s\"\n", "LDB");
            } break;
        case STB:
            {
                hxr_store(cpu, cpu->r[rb(inst)], 8, cpu->r[ra(inst)]);
                DEBUG_LOG("Running \"%s\"\n", "STB");
            } break;
        case PUSH:
            {
                uint16_t value = imm_11(inst);
                cpu->mem[cpu->sp] = (uint8_t)((value >> 0) & 0xff);
                cpu->mem[cpu->sp + 1] = (uint8_t)((value >> 8) & 0xff);
                DEBUG_LOG("Running \"%s\"\n", "PSH");
            } break;
        case POP:
            {
                uint16_t value = (cpu->mem[cpu->sp + 0] << 0) | (cpu->mem[cpu->sp + 1] << 8);
                cpu->r[ra(inst)] = value;
                DEBUG_LOG("Running \"%s\"\n", "POP");
            } break;
        case HALT:
            {
                cpu->halt = 1;
                DEBUG_LOG("Running \"%s\"\n", "HALT");
            } break;
        default:
            {
                fprintf(stderr, "Unreachable\n");
                exit(EXIT_FAILURE);
            } break;
    }
    return 0;
}

int hxr_init(HXR* cpu, const char* filepath)
{
    FILE* f;
    uint8_t* result;
    size_t sz, read_sz;

    f = fopen(filepath, "rb");
    if(!f) return 0;
    fseek(f, 0, SEEK_END);
    sz = ftell(f);
    fseek(f, 0, SEEK_SET);

    result = (uint8_t*)malloc(sz * sizeof(uint8_t));
    if(!result) {
        fclose(f);
        return 1;
    }

    read_sz = fread(result, sizeof(uint8_t), sz, f);
    if(read_sz != sz) {
        free(result);
        fclose(f);
        return 1;
    }

    cpu->sp = 0;
    cpu->halt = 0;
    cpu->ip = HXR_INSTRUCTIONS_START;

    memcpy(&cpu->mem[cpu->ip], result, read_sz);
    free(result);
    return 0;
}

// memory utilities
uint16_t hxr_load(HXR* cpu, uint16_t addr, uint16_t size)
{
    switch(size) {
        case 8: return hxr_load_8(cpu, addr);
        case 16: return hxr_load_16(cpu, addr);
        default: return 0;
    }
}

uint16_t hxr_load_8(HXR* cpu, uint16_t addr)
{
    return cpu->mem[addr];
}

uint16_t hxr_load_16(HXR* cpu, uint16_t addr)
{
    return cpu->mem[addr] << 0
         | cpu->mem[addr + 1] << 8;
}

void hxr_store(HXR* cpu, uint16_t addr, uint16_t size, uint16_t value)
{
    switch(size) {
        case 8: hxr_store_8(cpu, addr, value); break;
        case 16: hxr_store_16(cpu, addr, value); break;
        default: break;
    }
}

void hxr_store_8(HXR* cpu, uint16_t addr, uint16_t value)
{
    cpu->mem[addr] = (uint8_t)((value >> 0) & 0xff);
}

void hxr_store_16(HXR* cpu, uint16_t addr, uint16_t value)
{
    cpu->mem[addr] = (uint8_t)((value >> 0) & 0xff);
    cpu->mem[addr + 1] = (uint8_t)((value >> 8) & 0xff);
}

// instruction decoder
uint16_t opcode(uint16_t inst)
{
    return inst & 0x1f;
}

uint16_t ra(uint16_t inst)
{
    return (inst >> 5) & 0x07;
}

uint16_t rb(uint16_t inst)
{
    return (inst >> 8) & 0x07;
}

uint16_t imm_11(uint16_t inst)
{
    return (inst >> 5) & 0x7ff;
}

uint16_t imm_8(uint16_t inst)
{
    return (inst >> 8) & 0xff;
}

// execution
uint16_t hxr_fetch(HXR* cpu)
{
    return hxr_load(cpu, cpu->ip, 16);
}

void hxr_dump_registers(HXR* cpu)
{
    for(int i = 0; i < 8; ++i) {
        printf("R%d(%u)\n", i, cpu->r[i]);
    }
}
