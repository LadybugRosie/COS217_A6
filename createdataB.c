/*
Produces a file called dataB with the student name, a nullbyte,
padding to overrun the stack, and the address of the instruction
in main to get a B, the latter of which will overwrite getName's
stored x30
*/
#include <stdio.h>
#include <string.h>


/* FUNCTION COMMENT */
int main() {
    unsigned int uiData;
    char name[] = "David";

    FILE *psFile = fopen("davidBgrade.txt", "w");

    if (!psFile) {
        perror("fopen did not return a valid pointer");
        return 1; 
    }

    fwrite(&name, strlen(name), 1, psFile); /* Check case that this does not return */

    fclose(psFile);

    return 0;
}
