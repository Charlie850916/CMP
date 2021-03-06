#include "function.h"

void I_type_func(unsigned int op, unsigned int rs, unsigned int rt,short int im)
{
    int i = im;
    int PA;
    switch(op)
    {
    case 0x08: // addi
        s[rt] = s[rs] + im;
        break;
    case 0x09: // addiu
        s[rt] = s[rs] + im;
        break;
    case 0x23: // lw
        PA = D_VA2PA(s[rs]+im);
        D_PAinCache(PA);
        s[rt] = (d_mem[s[rs]+im]   << 24) |
                (d_mem[s[rs]+im+1] << 16) |
                (d_mem[s[rs]+im+2] << 8)  |
                (d_mem[s[rs]+im+3]);
        break;
    case 0x21: // lh
        PA = D_VA2PA(s[rs]+im);
        D_PAinCache(PA);
        s[rt] = (d_mem[s[rs]+im]<<24 >>16) |
                (d_mem[s[rs]+im+1]);
        break;
    case 0x25: // lhu
        PA = D_VA2PA(s[rs]+im);
        D_PAinCache(PA);
        s[rt] =  (d_mem[s[rs]+im]<<8) |
                 (d_mem[s[rs]+im+1]);
        break;
    case 0x20: // lb
        PA = D_VA2PA(s[rs]+im);
        D_PAinCache(PA);
        s[rt] = d_mem[s[rs]+im] << 24 >> 24;
        break;
    case 0x24: // lbu
        PA = D_VA2PA(s[rs]+im);
        D_PAinCache(PA);
        s[rt] = d_mem[s[rs]+im] ;
        break;
    case 0x2b: // sw
        PA = D_VA2PA(s[rs]+im);
        D_PAinCache(PA);
        d_mem[s[rs]+im] = (s[rt] >> 24) & 0x000000ff ;
        d_mem[s[rs]+im+1] = (s[rt] >> 16) & 0x000000ff ;
        d_mem[s[rs]+im+2] = (s[rt] >> 8) & 0x000000ff ;
        d_mem[s[rs]+im+3] = s[rt] & 0x000000ff ;
        break;
    case 0x29: // sh
        PA = D_VA2PA(s[rs]+im);
        D_PAinCache(PA);
        d_mem[s[rs]+im] = (s[rt] >> 8) & 0x000000ff ;
        d_mem[s[rs]+im+1] = s[rt] & 0x000000ff ;
        break;
    case 0x28: // sb
        PA = D_VA2PA(s[rs]+im);
        D_PAinCache(PA);
        d_mem[s[rs]+im] = s[rt] & 0x000000ff ;
        break;
    case 0x0f: // lui
        s[rt] = im << 16;
        break;
    case 0x0c: // andi
        s[rt] = s[rs] & (i&0x0000ffff);
        break;
    case 0x0d: // ori
        s[rt] = s[rs] | (i&0x0000ffff);
        break;
    case 0x0e: // nori
        s[rt] = ~(s[rs] | (i&0x0000ffff)) ;
        break;
    case 0x0a: // slti
        s[rt] = s[rs] < im ;
        break;
    case 0x04: // beq
        if(s[rs]==s[rt]) PC = PC + 4*im;
        break;
    case 0x05: // bne
        if(s[rs]!=s[rt]) PC = PC + 4*im;
        break;
    case 0x07: // bgtz
        if(s[rs]>0) PC = PC + 4*im;
        break;
    }
    s[0] = 0;
    return;
}
