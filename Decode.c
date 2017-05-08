#include <stdio.h>
#include <stdlib.h>
#include "function.h"

unsigned int GetLineN(int n, FILE *fp)
{
    fpos_t pos;
    pos.__pos = n;
    unsigned int buff;

    fsetpos(fp,&pos);
    fread(&buff,sizeof(unsigned int),1,fp);

    return Little2Big(buff);
}

unsigned int Little2Big(unsigned int i)
{
    unsigned int b0,b1,b2,b3;

    b0 = (i & 0x000000ff) << 24u;
    b1 = (i & 0x0000ff00) << 8u;
    b2 = (i & 0x00ff0000) >> 8u;
    b3 = (i & 0xff000000) >> 24u;

    return b0 | b1 | b2 | b3;
}

unsigned int GetOpcode(unsigned int i) { return (i >> 26) & 0x0000003f; }

unsigned int Get_rs(unsigned int i){ return (i >> 21) & 0x0000001f; }

unsigned int Get_rt(unsigned int i){ return (i >> 16) & 0x0000001f; }

unsigned int Get_rd(unsigned int i){ return (i >> 11) & 0x0000001f; }

unsigned int Get_C(unsigned int i) { return (i >> 6) & 0x0000001f; }

unsigned int Get_func(unsigned int i){ return i & 0x0000003f; }

short int Get_i(unsigned int i){ return (i & 0x0000ffff); }

int Get_addr(unsigned int i){ return (i & 0x03ffffff); }
