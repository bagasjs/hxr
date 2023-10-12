#include "hxr.h"
#define COMMON_IMPLEMENTATION
#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

void usage(FILE* f, const char* name)
{
    fprintf(f, "USAGE: %s [output] [src]\n", name);
}

void trap(const char* fmt, ...)
{
    fprintf(stderr, "[TRAP] ");
    va_list arg;
    va_start(arg, fmt);
    vfprintf(stderr, fmt, arg);
    va_end(arg);
    fprintf(stderr, "\n");
}

int get_file_size(FILE* f)
{
    int result;
    if(!f) return 0;
    fseek(f, 0L, SEEK_END);
    result = ftell(f);
    fseek(f, 0L, SEEK_SET);
    return result;
}

int load_file_data(FILE* f, char* result)
{
    if(!f) return 0;
    if(!result) return 0;
    int size = get_file_size(f);
    int read_sz = fread(result, sizeof(char), size, f);
    if(read_sz != size) {
        return 0;
    }
    return read_sz;
}

uint16_t parse_instruction(String_View op, String_View a1, String_View a2)
{
    uint16_t inst = 0;
    if(sv_eq(op, sv_from_cstr("mov"))) {
        if(a1.count == 2 && __common_isdigit(a1.data[1])) {
            uint16_t r = a1.data[1] - '0';
            inst |= r << 5;
        } else {
            trap("The 1st argument of instruction "SV_FMT" should be a register");
        }

        if(a2.count <= 0) {
            trap("The 2nd argument of instruction "SV_FMT" should be either registers or immediate value", SV_ARGV(op));
        }

        if(a2.data[0] == 'r') {
            inst |= MOV;
            uint16_t r = a2.data[1] - '0';
            inst |= r << 8;
        } else {
            inst |= MOVI;
            int r = sv_to_int(a2);
            inst |= r << 8;
        }
    } else if(sv_eq(op, sv_from_cstr("add"))) {
        if(a1.count == 2 && __common_isdigit(a1.data[1])) {
            int r = a1.data[1] - '0';
            inst |= r << 5;
        } else {
            trap("The 1st argument of instruction "SV_FMT" should be a register", SV_ARGV(op));
        }

        if(a2.count <= 0) {
            trap("The 2nd argument of instruction "SV_FMT" should be either registers or immediate value", SV_ARGV(op));
        }

        if(a2.data[0] == 'r') {
            inst |= ADD;
            uint16_t r = a2.data[1] - '0';
            inst |= r << 8;
        } else {
            inst |= ADDI;
            int r = sv_to_int(a2);
            inst |= r << 8;
        }
    } else if(sv_eq(op, sv_from_cstr("sub"))) {
        if(a1.count == 2 && __common_isdigit(a1.data[1])) {
            int r = a1.data[1] - '0';
            inst |= r << 5;
        } else {
            trap("The 1st argument of instruction "SV_FMT" should be a register", SV_ARGV(op));
        }

        if(a2.count <= 0) {
            trap("The 2nd argument of instruction "SV_FMT" should be either registers or immediate value", SV_ARGV(op));
        }

        if(a2.data[0] == 'r') {
            inst |= SUB;
            uint16_t r = a2.data[1] - '0';
            inst |= r << 8;
        } else {
            inst |= SUBI;
            int r = sv_to_int(a2);
            inst |= r << 8;
        }
    } else if(sv_eq(op, sv_from_cstr("mod"))) {
        if(a1.count == 2 && __common_isdigit(a1.data[1])) {
            int r = a1.data[1] - '0';
            inst |= r << 5;
        } else {
            trap("The 1st argument of instruction "SV_FMT" should be a register", SV_ARGV(op));
        }

        if(a2.count <= 0) {
            trap("The 2nd argument of instruction "SV_FMT" should be either registers or immediate value", SV_ARGV(op));
        }

        if(a2.data[0] == 'r') {
            inst |= MOD;
            uint16_t r = a2.data[1] - '0';
            inst |= r << 8;
        } else {
            inst |= MODI;
            int r = sv_to_int(a2);
            inst |= r << 8;
        }
    } else if(sv_eq(op, sv_from_cstr("hlt"))) {
        inst = HALT;
    } else {
        trap("Unknown Instruction "SV_FMT, SV_ARGV(op));
    }
    // printf(SV_FMT" "SV_FMT", "SV_FMT" = %u\n", SV_ARGV(op), SV_ARGV(a1), SV_ARGV(a2), inst);
    return inst;
}

uint16_t parse_line(String_View line)
{
    line = sv_rtrim(sv_ltrim(line));
    String_View op = sv_chop_by_delim(&line, ' ');
    String_View arg1 = sv_chop_by_delim(&line, ',');
    return parse_instruction(op, arg1, sv_ltrim(line));
}

typedef da(uint16_t) Program;

Program parse_source(String_View source)
{
    source = sv_rtrim(sv_ltrim(source));
    String_View line = INVALID_SV;
    Program insts = {0};
    while(source.count > 0) {
        line = sv_chop_by_delim(&source, '\n');
        uint16_t inst = parse_line(line);
        da_append(&insts, inst);
    }
    return insts;
}

void save_program_to_file(Program program, const char* filepath)
{
    FILE* f = fopen(filepath, "wb");
    if(!f) trap("Failed to save into \"%s\"", filepath);

    fwrite(program.data, sizeof(uint16_t), program.count, f);
    if(ferror(f)) trap("ERROR: couldn't write to file \"%s\"", filepath);
    fclose(f);
}

int main(int argc, const char** argv)
{
    if(argc < 3) {
        fprintf(stderr, "ERROR: Please provide arguments\n");
        usage(stderr, argv[0]);
        return 1;
    }

    const char* in = argv[2];
    const char* out = argv[1];

    FILE* f = fopen(in, "r");
    if(!f) {
        fprintf(stderr, "ERROR: Input file not found\n");
        return 1;
    }
    int file_size = get_file_size(f);
    char* in_data = (char*)malloc(sizeof(char) * (file_size + 1));
    if(load_file_data(f, in_data) != file_size) {
        fprintf(stderr, "ERROR: Failed to load file\n");
        return 1;
    }
    in_data[file_size] = '\0';
    String_View source = sv_from_parts(in_data, file_size);
    Program program = parse_source(source);
    save_program_to_file(program, out);
    free(in_data);

    return 0;
}


