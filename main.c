#include "function.h"

int main(int argc, char* argv[])
{
    if(argc < 2) I_mem_size = 64;
    else I_mem_size = atoi(argv[1]);

    if(argc < 3) D_mem_size = 32;
    else D_mem_size = atoi(argv[2]);

    if(argc < 4) I_page_size = 8;
    else I_page_size = atoi(argv[3]);

    if(argc < 5) D_page_size = 16;
    else D_page_size = atoi(argv[4]);

    if(argc < 6) I_cache_size = 16;
    else I_cache_size = atoi(argv[5]);

    if(argc < 7) I_cache_block = 4;
    else I_cache_block = atoi(argv[6]);

    if(argc < 8) I_cache_associativity = 4;
    else I_cache_associativity = atoi(argv[7]);

    if(argc < 9) D_cache_size = 16;
    else D_cache_size = atoi(argv[8]);

    if(argc < 10) D_cache_block = 4;
    else D_cache_block = atoi(argv[9]);

    if(argc < 11) D_cache_associativity = 1;
    else D_cache_associativity = atoi(argv[10]);

    unsigned int IS, opcode, addr, PA;

    Initial();

    cycle = 1;

    while(!halt)
    {
        IS = i_mem[PC/4];
        PA = I_VA2PA(PC);
        I_PAinCache(PA);
        opcode = GetOpcode(IS);
        switch(opcode)
        {
        case 0x00: // R_type
            PC = PC + 4;
            R_type_func(Get_rs(IS),Get_rt(IS),Get_rd(IS),Get_C(IS),Get_func(IS));
            break;
        case 0x02: // j
            addr = Get_addr(IS);
            PC = ((PC+4)&0xf0000000) | (addr<<2);
            break;
        case 0x03: // jal
            addr = Get_addr(IS);
            s[31] = PC + 4;
            PC = ((PC+4)&0xf0000000) | (addr<<2);
            break;
        case 0x3f: // halt
            halt = 1;
            break;
        default: // I_type
            I_type_func(opcode,Get_rs(IS),Get_rt(IS),Get_i(IS));
            PC = PC + 4;
            break;
        }
        if(!halt) PrintImf();
        cycle++;
    }

    Ending();
    return 0;
}
