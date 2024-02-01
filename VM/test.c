#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

uint16_t program[] = {0x3024, 0x223, 0x60, 0x3025, 0x3026};

int main(int argc, char** argv) {
    char *outf = "sum.obj";
    FILE *f = fopen(outf, "wb");
    if (NULL==f) {
        fprintf(stderr, "Cannot write to file %s\n", outf);
    }
    size_t writ = fwrite(program, sizeof(uint16_t), sizeof(program), f);
    fprintf(stdout, "Written size_t=%lu to file %s\n", writ, outf);
    fclose(f);
    return 0;
}
