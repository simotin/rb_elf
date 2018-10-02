/************************************************************************
*
* Device     : RX
*
* File Name  : dbsct.c
*
* Abstract   : Setting of B,R Section.
*
* History    : 1.00  (2009-08-07)
*            : 1.10  (2013-02-13)
*
* NOTE       : THIS IS A TYPICAL EXAMPLE.
*
* Copyright (C) 2009 Renesas Electronics Corporation.
* and Renesas Solutions Corporation. All rights reserved.
*
************************************************************************/

#include "typedefine.h"

#pragma unpack

#pragma section C C$DSEC
extern const struct {
    _UBYTE *rom_s;       /* Start address of the initialized data section in ROM */
    _UBYTE *rom_e;       /* End address of the initialized data section in ROM   */
    _UBYTE *ram_s;       /* Start address of the initialized data section in RAM */
}   _DTBL[] = {
    { __sectop("D"), __secend("D"), __sectop("R") },
    { __sectop("D_2"), __secend("D_2"), __sectop("R_2") },
    { __sectop("D_1"), __secend("D_1"), __sectop("R_1") }
};
#pragma section C C$BSEC
extern const struct {
    _UBYTE *b_s;         /* Start address of non-initialized data section */
    _UBYTE *b_e;         /* End address of non-initialized data section */
}   _BTBL[] = {
    { __sectop("B"), __secend("B") },
    { __sectop("B_2"), __secend("B_2") },
    { __sectop("B_1"), __secend("B_1") }
};

#pragma section

/*
** CTBL prevents excessive output of L1100 messages when linking.
** Even if CTBL is deleted, the operation of the program does not change.
*/
// delete 
_UBYTE * const _CTBL[] = {
    __sectop("C_1"), __sectop("C_2"), __sectop("C"),
    __sectop("W_1"), __sectop("W_2"), __sectop("W"),
    __sectop("L"), __sectop("SU"),
    __sectop("C$DSEC"),
    __sectop("C$BSEC"),
    // __sectop("C$INIT"),
    // __sectop("C$VTBL"),
    // __sectop("C$VECT"),
};

#pragma packoption
