/*--------------------------------------------------------------------*/
/* miniassembler.c                                                    */
/* Author: Bob Dondero, Donna Gabai                                   */
/*--------------------------------------------------------------------*/

#include "miniassembler.h"
#include <assert.h>
#include <stddef.h>

/*--------------------------------------------------------------------*/
/* Modify *puiDest in place,
   setting uiNumBits starting at uiDestStartBit (where 0 indicates
   the least significant bit) with bits taken from uiSrc,
   starting at uiSrcStartBit.
   uiSrcStartBit indicates the rightmost bit in the field.
   setField sets the appropriate bits in *puiDest to 1.
   setField never unsets any bits in *puiDest.                        */
static void setField(unsigned int uiSrc, unsigned int uiSrcStartBit,
                     unsigned int *puiDest, unsigned int uiDestStartBit,
                     unsigned int uiNumBits)
{
    unsigned int uiTotalBits = sizeof(unsigned int) * 8;
    unsigned int bitmask = 0x0;

    /* Trivial case */
    if (uiNumBits == 0) return;

    /* Set the bitmask to the correct number of bits */
    if (uiNumBits >= uiTotalBits) {
        bitmask = ~0u;
    }

    else {
        bitmask = (1u << uiNumBits) - 1u;
    }

    /* Copy the correct set of bits from uiSrc */
    bitmask = bitmask & (uiSrc >> uiSrcStartBit);

    /* Mask the correct set of bits in puiDest */
    *puiDest = *puiDest | (bitmask << uiDestStartBit);
}

/*--------------------------------------------------------------------*/

unsigned int MiniAssembler_mov(unsigned int uiReg, int iImmed)
{
    /* Base instruction with sf == 0 to move into a Wd register
       and hw == 0 for no left shift */
    unsigned int uiInstruction = 0x52800000;

    /* Set Wd into bits [0...4] */
    setField(uiReg, 0, &uiInstruction, 0, 5);

    /* Set imm16 (immediate) into bits [5...20] */
    setField(iImmed, 0, &uiInstruction, 5, 16);

    return uiInstruction;
}

/*--------------------------------------------------------------------*/

unsigned int MiniAssembler_adr(unsigned int uiReg, unsigned long ulAddr,
   unsigned long ulAddrOfThisInstr)
{
   unsigned int uiInstr;
   unsigned int uiDisp;

   /* Base Instruction Code */
   uiInstr = 0x10000000;

   /* register to be inserted in instruction */
   setField(uiReg, 0, &uiInstr, 0, 5);

   /* displacement to be split into immlo and immhi and inserted */
   uiDisp = (unsigned int)(ulAddr - ulAddrOfThisInstr);

   setField(uiDisp, 0, &uiInstr, 29, 2);
   setField(uiDisp, 2, &uiInstr, 5, 19);

   return uiInstr;
}

/*--------------------------------------------------------------------*/

unsigned int MiniAssembler_strb(unsigned int uiFromReg,
   unsigned int uiToReg)
{
    /* Base instruction code with 0 offset (strb w0, [x0]) */
    unsigned int uiInstruction = 0x39000000;

    /* Set register to be loaded from in bits [0...4] */
    setField(uiFromReg, 0, &uiInstruction, 0, 5);

    /* Set register to be loaded to in bits [5...9] */
    setField(uiToReg, 0, &uiInstruction, 5, 5);

    return uiInstruction;
}

/*--------------------------------------------------------------------*/

unsigned int MiniAssembler_b(unsigned long ulAddr,
   unsigned long ulAddrOfThisInstr)
{
    /* Base instruction code */
    unsigned int uiInstruction = 0x14000000;

    /* Calculate the shifted offset */
    int label = (int) (((long) ulAddr - (long) ulAddrOfThisInstr) / 4 );

    /* Sets label in bits [0...25] */
    setField((unsigned int) label, 0, &uiInstruction, 0, 26);

    return uiInstruction;
}
