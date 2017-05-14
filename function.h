#include <stdio.h>
#include <stdlib.h>

int s[32];

int d_mem [1024];

unsigned int i_mem[256];

unsigned int PC, LO, HI, i_num, d_num;

unsigned int I_mem_size, D_mem_size, I_page_size, D_page_size, I_cache_size, I_cache_block, I_cache_associativity, D_cache_size, D_cache_block, D_cache_associativity;

unsigned int I_cache_index, D_cache_index, ITLB_entry, DTLB_entry, IPTE_entry, DPTE_entry, IMEM_entry, DMEM_entry, I_cache_WordPerBlock, D_cache_WordPerBlock;

unsigned int ICache_hit, ICache_miss, DCache_hit, DCache_miss, ITLB_hit, ITLB_miss, DTLB_hit, DTLB_miss, IPTE_hit, IPTE_miss, DPTE_hit, DPTE_miss;

int *ICache, *DCache, *ITLB, *DTLB, *IPTE, *DPTE;

FILE *fp_r, *fp_t;

int cycle, halt;

void Initial();

void InitialImf();

void PrintImf();

void Ending();

unsigned int Little2Big(unsigned int i);

unsigned int GetLineN(int n, FILE *fp);

unsigned int GetOpcode(unsigned int i);

unsigned int Get_rs(unsigned int i);

unsigned int Get_rt(unsigned int i);

unsigned int Get_rd(unsigned int i);

unsigned int Get_C(unsigned int i);

unsigned int Get_func(unsigned int i);

short int Get_i(unsigned int i);

int Get_addr(unsigned int i);

void R_type_func(unsigned int s,unsigned int t,unsigned int d,unsigned int C,unsigned int func);

void I_type_func(unsigned int op, unsigned int s, unsigned int t,short int im);

void I_PAinCache(int PA);

int I_VA2PA(int VA);

void D_PAinCache(int PA);

int D_VA2PA(int VA);

