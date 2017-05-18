#include "function.h"

int s_p[32];

unsigned int LO_p, HI_p;

void Initial()
{
    FILE *fp_i, *fp_d;
    int i, buff;

    fp_i = fopen("iimage.bin", "rb");
    fp_d = fopen("dimage.bin", "rb");
    fp_r = fopen("snapshot.rpt", "wb");
 
    HI = 0x0000000;
    LO = 0x0000000;
    HI_p = 0x00000000;
    LO_p = 0x00000000;

    for(i=0 ; i<32 ; i++)
    {
        s[i] = 0x00000000;
        s_p[i] = 0x00000000;
    }

    for(i=0 ; i<256 ; i++) i_mem[i] = 0;
    for(i=0 ; i<1024 ; i++) d_mem[i] = 0;

    PC = GetLineN(0x00, fp_i);
    i_num = GetLineN(0x04, fp_i);
    s[29] = GetLineN(0x00, fp_d);
    d_num = GetLineN(0x04, fp_d);

    s_p[29] = s[29];

    for(i=0 ; i<i_num ; i++) i_mem[PC/4+i] = GetLineN( i*4+8, fp_i);

    for(i=0 ; i<d_num ; i++)
    {
        buff = GetLineN(i*4+8, fp_d) ;
        d_mem[i*4]   = (buff >> 24) & 0x000000ff;
        d_mem[i*4+1] = (buff >> 16) & 0x000000ff;
        d_mem[i*4+2] = (buff >> 8)  & 0x000000ff;
        d_mem[i*4+3] =  buff        & 0x000000ff;
    }

    I_cache_index = I_cache_size/I_cache_block/I_cache_associativity;
    D_cache_index = D_cache_size/D_cache_block/D_cache_associativity;

    I_cache_WordPerBlock = I_cache_block/4;
    D_cache_WordPerBlock = D_cache_block/4;

    IPTE_entry = 1024/I_page_size;
    DPTE_entry = 1024/D_page_size;

    ITLB_entry = IPTE_entry/4;
    DTLB_entry = DPTE_entry/4;

    IMEM_entry = I_mem_size/I_page_size;
    DMEM_entry = D_mem_size/D_page_size;

    ITLB = malloc(ITLB_entry * 4 * sizeof(int)); // entry * {valid,LRU,VA,PA}
    IPTE = malloc(IPTE_entry * 3 * sizeof(int)); // entry * {valid,LRU,PA}
    ICache = malloc(I_cache_index * I_cache_associativity * 3 * sizeof(int)); // index * set * {valid, MRU, tag}

    DTLB = malloc(DTLB_entry * 4 * sizeof(int));
    DPTE = malloc(DPTE_entry * 3 * sizeof(int));
    DCache = malloc(D_cache_index * D_cache_associativity * 3 * sizeof(int));

    for(i=0 ; i<4*ITLB_entry ; i++) *(ITLB+i) = 0;
    for(i=0 ; i<3*IPTE_entry ; i++) *(IPTE+i) = 0;
    for(i=0 ; i<3*I_cache_index*I_cache_associativity ; i++) *(ICache+i) = 0;

    for(i=0 ; i<4*DTLB_entry ; i++) *(DTLB+i) = 0;
    for(i=0 ; i<3*DPTE_entry ; i++) *(DPTE+i) = 0;
    for(i=0 ; i<3*D_cache_index*D_cache_associativity ; i++) *(DCache+i) = 0;

    halt = 0;

    fclose(fp_i);
    fclose(fp_d);

    InitialImf();

    return;
}

void Ending()
{
    fclose(fp_r);
    free(ICache);
    free(ITLB);
    free(IPTE);
    free(DCache);
    free(DTLB);
    free(DPTE);
    FILE *fptr_report;
    fptr_report = fopen("report.rpt", "wb");
    fprintf( fptr_report, "ICache :\n");
    fprintf( fptr_report, "# hits: %u\n", ICache_hit );
    fprintf( fptr_report, "# misses: %u\n\n", ICache_miss );
    fprintf( fptr_report, "DCache :\n");
    fprintf( fptr_report, "# hits: %u\n", DCache_hit );
    fprintf( fptr_report, "# misses: %u\n\n", DCache_miss );
    fprintf( fptr_report, "ITLB :\n");
    fprintf( fptr_report, "# hits: %u\n", ITLB_hit );
    fprintf( fptr_report, "# misses: %u\n\n", ITLB_miss );
    fprintf( fptr_report, "DTLB :\n");
    fprintf( fptr_report, "# hits: %u\n", DTLB_hit );
    fprintf( fptr_report, "# misses: %u\n\n", DTLB_miss);
    fprintf( fptr_report, "IPageTable :\n");
    fprintf( fptr_report, "# hits: %u\n", IPTE_hit );
    fprintf( fptr_report, "# misses: %u\n\n", IPTE_miss);
    fprintf( fptr_report, "DPageTable :\n");
    fprintf( fptr_report, "# hits: %u\n", DPTE_hit );
    fprintf( fptr_report, "# misses: %u\n\n", DPTE_miss );
    fclose(fptr_report);
    return;
}

void InitialImf()
{
    int i;
    fprintf(fp_r,"cycle 0\n");
    for(i=0 ; i<32 ; i++) fprintf(fp_r,"$%02d: 0x%08X\n", i, s[i]);
    fprintf(fp_r,"$HI: 0x%08X\n", HI);
    fprintf(fp_r,"$LO: 0x%08X\n", LO);
    fprintf(fp_r,"PC: 0x%08X\n", PC);
    fprintf(fp_r,"\n\n");
    return;
}

void PrintImf()
{
    int i;
    fprintf(fp_r,"cycle %d\n",cycle);
    for(i=0 ; i<32 ; i++)
    {
        if( s[i]!=s_p[i] )
            fprintf(fp_r,"$%02d: 0x%08X\n", i, s[i]);
        s_p[i] = s[i];
    }
    if(HI!=HI_p)
    {
        fprintf(fp_r,"$HI: 0x%08X\n", HI);
        HI_p = HI;
    }
    if(LO!=LO_p)
    {
        fprintf(fp_r,"$LO: 0x%08X\n", LO);
        LO_p = LO;
    }
    fprintf(fp_r,"PC: 0x%08X\n",PC);
    fprintf(fp_r,"\n\n");
    return;
}
