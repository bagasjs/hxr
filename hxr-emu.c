#include "hxr.h"
#include <stdio.h>
#include <stdlib.h>

void usage(FILE* f, const char* name)
{
    fprintf(f, "USAGE: %s [rom]\n", name);
}

int main(int argc, const char** argv)
{
    if(argc < 2) {
        fprintf(stderr, "ERROR: Please provide an argument\n");
        usage(stderr, argv[0]);
        return 1;
    }

    HXR hxr = {0};
    if(hxr_init(&hxr, argv[1]) != 0) {
        fprintf(stderr, "ERROR: Failed to load ROM\n");
        return 1;
    }

    while(hxr.halt == 0) {
        uint16_t inst = hxr_fetch(&hxr);
        hxr_execute(&hxr, inst);
        hxr.ip += 2;
    }

    hxr_dump_registers(&hxr);
    return 0;
}
