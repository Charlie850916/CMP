#include <stdio.h>
#include <stdlib.h>
#include "function.h"

void R_type_func(unsigned int rs, unsigned int rt, unsigned int rd, unsigned int C, unsigned int func)
{
    unsigned int i;
    long long a, b;
    switch(func)
    {
    case 0x20: // add
        s[rd] = s[rs] + s[rt];
        break;
    case 0x21: // addu
        s[rd] = s[rs] + s[rt];
        break;
    case 0x22: // sub
        s[rd] = s[rs] - s[rt];
        break;
    case 0x24: // and
        s[rd] = (s[rs] & s[rt]);
        break;
    case 0x25: // or
        s[rd] = (s[rs] | s[rt]);
        break;
    case 0x26: // xor
        s[rd] = (s[rs] ^ s[rt]);
        break;
    case 0x27: // nor
        s[rd] = ~(s[rs] | s[rt]);
        break;
    case 0x28: // nand
        s[rd] = ~(s[rs] & s[rt]);
        break;
    case 0x2a: // slt
        s[rd] = (s[rs] < s[rt]);
        break;
    case 0x00: // sll nop
        if(rt!=0||rd!=0||C!=0)
        {
            s[rd] = s[rt] << C;
            break;
        }
        break;
    case 0x02: // srl
        i = s[rt];
        i = i >> C;
        s[rd] = i;
        break;
    case 0x03: // sra
        s[rd] = s[rt] >> C;
        break;
    case 0x08: // jr
        PC = s[rs];
        break;
    case 0x18: // mult
        a = s[rs];
        b = s[rt];
        HI = a*b >> 32;
        LO = a*b & 0x00000000ffffffff ;
        break;
    case 0x19: // multu
        a = ( s[rs] & 0x00000000ffffffff);
        b = ( s[rt] & 0x00000000ffffffff);
        HI = a*b >> 32;
        LO = a*b & 0x00000000ffffffff ;
        break;
    case 0x10: // mfhi
        s[rd] = HI;
        break;
    case  0x12: // mflo
        s[rd] = LO;
        break;
    }
    s[0] = 0;
    return;
}
