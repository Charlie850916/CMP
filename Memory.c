#include "function.h"
#include <math.h>
int GetICacheEntry(int index, int set, int num){ return *(ICache + index*I_cache_associativity*3 + set*3 + num); }

void SetICacheEntry(int index, int set, int num, int value){ *(ICache + index*I_cache_associativity*3 + set*3 + num) = value; }

void SetICacheMRU(int index, int set)
{
    if(I_cache_associativity==1) return;
    int i;
    SetICacheEntry(index, set, 1, 1);
    for(i=0 ; i<I_cache_associativity ; i++)
    {
        if(GetICacheEntry(index, i, 0) == 0) break;
        if(GetICacheEntry(index, i, 1) == 0) break;
        if(i==I_cache_associativity-1)
        {
            for(i=0 ; i<I_cache_associativity ; i++) SetICacheEntry(index, i, 1, 0);
            break;
        }
    }
    SetICacheEntry(index, set, 1, 1);
    return;
}

int GetDCacheEntry(int index, int set, int num){ return *(DCache + index*D_cache_associativity*3 + set*3 + num); }

void SetDCacheEntry(int index, int set, int num, int value){ *(DCache + index*D_cache_associativity*3 + set*3 + num) = value; }

void SetDCacheMRU(int index, int set)
{
    if(D_cache_associativity==1) return;
    int i;
    SetDCacheEntry(index, set, 1, 1);
    for(i=0 ; i<D_cache_associativity ; i++)
    {
        if(GetDCacheEntry(index, i, 0) == 0) break;
        if(GetDCacheEntry(index, i, 1) == 0) break;
        if(i==D_cache_associativity-1)
        {
            for(i=0 ; i<D_cache_associativity ; i++) SetDCacheEntry(index, i, 1, 0);
            break;
        }
    }
    SetDCacheEntry(index, set, 1, 1);
    return;
}

int I_VA2PA(int VA)
{
    int i, VPN = VA/I_page_size, offset = VA%I_page_size;  
    for(i=0 ; i<ITLB_entry ; i++) //search TLB
    {
        if( *(ITLB+4*i)==1 && *(ITLB+4*i+2) == VPN ) // update LRU
        {
            *(ITLB+4*i+1) = cycle;
            ITLB_hit++;
            return *(ITLB+4*i+3)*I_page_size + offset;
        }
    }
    ITLB_miss++;
    int LRU_index = 0;
    if( *(IPTE+3*VPN)==1 ) //search PTE
    {
        IPTE_hit++;
        for(i=0 ; i<ITLB_entry ; i++) // update TLB
        {
            if( *(ITLB+4*i) == 0)
            {
                LRU_index = i;
                break;
            }
            if( *(ITLB+4*i+1) < *(ITLB+4*LRU_index+1) ) LRU_index = i;
        }
        *(ITLB+4*LRU_index) = 1;
        *(ITLB+4*LRU_index+1) = cycle;
        *(ITLB+4*LRU_index+2) = VPN;
        *(ITLB+4*LRU_index+3) = *(IPTE+3*VPN+2);
        return *(IPTE+3*VPN+2) * I_page_size + offset;
    }
    IPTE_miss++;
    for(i=0 ; i<IPTE_entry ; i++) // calculate LRU_index of PTE
    {
        if( *(IPTE+3*i) == 1)
        {
            LRU_index = i;
            break;
        }
    }
    int PTE_valid_sum = 0, TLB_LRU_index = 0;
    for(i=0 ; i<IPTE_entry ; i++)
    {
        if( *(IPTE+3*i)==1 )
        {
            PTE_valid_sum++;
            if( *(IPTE+3*i+1) < *(IPTE+3*LRU_index+1) ) LRU_index = i;
        }
    }
    if(PTE_valid_sum < IMEM_entry) // does not use all memory
    {
        *(IPTE+3*VPN) = 1;
        *(IPTE+3*VPN+1) = cycle;
        *(IPTE+3*VPN+2) = PTE_valid_sum;
        for(i=0 ; i<ITLB_entry ; i++) // update TLB
        {
            if( *(ITLB+4*i) == 0 )
            {
                TLB_LRU_index = i;
                break;
            }
            if( *(ITLB+4*i+1) < *(ITLB+4*TLB_LRU_index+1) ) TLB_LRU_index = i;
        }
        *(ITLB+4*TLB_LRU_index) = 1;
        *(ITLB+4*TLB_LRU_index+1) = cycle;
        *(ITLB+4*TLB_LRU_index+2) = VPN;
        *(ITLB+4*TLB_LRU_index+3) = PTE_valid_sum;
        return PTE_valid_sum * I_page_size + offset;
    }
    else // update TLB & Cache
    {
        *(IPTE+3*LRU_index) = 0;
        *(IPTE+3*VPN) = 1;
        *(IPTE+3*VPN+1) = cycle;
        *(IPTE+3*VPN+2) = *(IPTE+3*LRU_index+2);
        for(i=0 ; i<ITLB_entry ; i++)
        {
            if( *(ITLB+4*i) == 1 && *(ITLB+4*i+2) == LRU_index )
            {
                *(ITLB+4*i) = 0;
                break;
            }
        }
        for(i=0 ; i<ITLB_entry ; i++)
        {
            if( *(ITLB+4*i) == 0 )
            {
                TLB_LRU_index = i;
                break;
            }
            if( *(ITLB+4*i+1) < *(ITLB+4*TLB_LRU_index+1) ) TLB_LRU_index = i;
        }
        *(ITLB+4*TLB_LRU_index) = 1;
        *(ITLB+4*TLB_LRU_index+1) = cycle;
        *(ITLB+4*TLB_LRU_index+2) = VPN;
        *(ITLB+4*TLB_LRU_index+3) = *(IPTE+3*LRU_index+2);
        int PA = *(IPTE+3*LRU_index+2)*I_page_size;
        int index, tag;
        while(PA < (*(IPTE+3*LRU_index+2)+1)*I_page_size)
        {
            index = (PA/4/I_cache_WordPerBlock) % I_cache_index;
            tag = (PA/4/I_cache_WordPerBlock) / I_cache_index;
            for(i=0 ; i<I_cache_associativity ; i++)
               if(GetICacheEntry(index, i, 0)==1 && GetICacheEntry(index, i, 2)==tag) SetICacheEntry(index, i, 0, 0);
            PA++;
        }
        return *(IPTE+3*LRU_index+2) * I_page_size + offset;
    }
}

void I_PAinCache(int PA)
{
    PA /= 4;
    int index = (PA/I_cache_WordPerBlock)%I_cache_index, tag = (PA/I_cache_WordPerBlock)/I_cache_index, i;
    for(i=0 ; i<I_cache_associativity ; i++)
    {
        if(GetICacheEntry(index, i, 0)==1 && GetICacheEntry(index, i, 2)==tag)
        {
            ICache_hit++;
            SetICacheMRU(index, i);
            return;
        }
    }
    ICache_miss++;
    for(i=0 ; i<I_cache_associativity ; i++)
    {
        if(GetICacheEntry(index, i, 0)==0)
        {
            SetICacheEntry(index, i, 0, 1);
            SetICacheMRU(index, i);
            SetICacheEntry(index, i, 2, tag);
            return;
        }
    }
    for(i=0 ; i<I_cache_associativity ; i++)
    {
        if(GetICacheEntry(index, i, 1)==0)
        {
            SetICacheMRU(index, i);
            SetICacheEntry(index, i, 2, tag);
            return;
        }
    }
    return;
}

int D_VA2PA(int VA)
{
    int i, VPN = VA/D_page_size, offset = VA%D_page_size;
    for(i=0 ; i<DTLB_entry ; i++) //search TLB
    {
        if( *(DTLB+4*i)==1 && *(DTLB+4*i+2) == VPN )
        {
            *(DTLB+4*i+1) = cycle;
            DTLB_hit++;
            return *(DTLB+4*i+3)*D_page_size + offset;
        }
    }
    DTLB_miss++;
    int LRU_index = 0;
    if( *(DPTE+3*VPN)==1 ) //search PTE
    {
        DPTE_hit++;
        for(i=0 ; i<DTLB_entry ; i++) // update TLB
        {
            if( *(DTLB+4*i) == 0)
            {
                LRU_index = i;
                break;
            }
            if( *(DTLB+4*i+1) < *(DTLB+4*LRU_index+1) ) LRU_index = i;
        }
        *(DTLB+4*LRU_index) = 1;
        *(DTLB+4*LRU_index+1) = cycle;
        *(DTLB+4*LRU_index+2) = VPN;
        *(DTLB+4*LRU_index+3) = *(DPTE+3*VPN+2);
        return *(DPTE+3*VPN+2) * D_page_size + offset;
    }
    DPTE_miss++;
    int PTE_valid_sum = 0, TLB_LRU_index = 0;
    for(i=0 ; i<DPTE_entry ; i++)
    {
        if( *(DPTE+3*i)==1 )
        {
            LRU_index = i;
            break;
        }
    }
    for(i=0 ; i<DPTE_entry ; i++)
    {
        if( *(DPTE+3*i)==1 )
        {
            PTE_valid_sum++;
            if( *(DPTE+3*i+1) < *(DPTE+3*LRU_index+1) ) LRU_index = i;
        }
    }
    if(PTE_valid_sum < DMEM_entry)
    {
        *(DPTE+3*VPN) = 1;
        *(DPTE+3*VPN+1) = cycle;
        *(DPTE+3*VPN+2) = PTE_valid_sum;
        for(i=0 ; i<DTLB_entry ; i++) // update TLB
        {
            if( *(DTLB+4*i) == 0 )
            {
                TLB_LRU_index = i;
                break;
            }
            if( *(DTLB+4*i+1) < *(DTLB+4*TLB_LRU_index+1) ) TLB_LRU_index = i;
        }
        *(DTLB+4*TLB_LRU_index) = 1;
        *(DTLB+4*TLB_LRU_index+1) = cycle;
        *(DTLB+4*TLB_LRU_index+2) = VPN;
        *(DTLB+4*TLB_LRU_index+3) = PTE_valid_sum;
        return PTE_valid_sum * D_page_size + offset;
    }
    else // update TLB Cache
    {
        *(DPTE+3*LRU_index) = 0;
        *(DPTE+3*VPN) = 1;
        *(DPTE+3*VPN+1) = cycle;
        *(DPTE+3*VPN+2) = *(DPTE+3*LRU_index+2);
        for(i=0 ; i<DTLB_entry ; i++)
        {
            if( *(DTLB+4*i) == 1 && *(DTLB+4*i+2) == LRU_index)
            {
                *(DTLB+4*i) = 0;
                break;
            }
        }
        for(i=0 ; i<DTLB_entry ; i++)
        {
            if( *(DTLB+4*i) == 0 )
            {
                TLB_LRU_index = i;
                break;
            }
            if( *(DTLB+4*i+1) < *(DTLB+4*TLB_LRU_index+1) ) TLB_LRU_index = i;
        }
        *(DTLB+4*TLB_LRU_index) = 1;
        *(DTLB+4*TLB_LRU_index+1) = cycle;
        *(DTLB+4*TLB_LRU_index+2) = VPN;
        *(DTLB+4*TLB_LRU_index+3) = *(DPTE+3*LRU_index+2);
        int PA = *(DPTE+3*LRU_index+2) * D_page_size, index, tag;
        while(PA < (*(DPTE+3*LRU_index+2)+1) * D_page_size )
        {
             index = (PA/4/D_cache_WordPerBlock) % D_cache_index;
             tag = (PA/4/D_cache_WordPerBlock) / D_cache_index;
             for(i=0 ; i<D_cache_associativity ; i++)
             {
                 if(GetDCacheEntry(index, i, 0) == 1 && GetDCacheEntry(index, i, 2) == tag)
                 {
                     SetDCacheEntry(index, i, 0, 0);
                 }
             }
             PA++;
        }
        return *(DPTE+3*LRU_index+2) * D_page_size + offset;
    }
}

void D_PAinCache(int PA)
{
    PA /= 4;
    int index = PA/D_cache_WordPerBlock%D_cache_index, tag = PA/D_cache_WordPerBlock/D_cache_index, i;
    for(i=0 ; i<D_cache_associativity ; i++)
    {
        if(GetDCacheEntry(index, i, 0)==1 && GetDCacheEntry(index, i, 2)==tag)
        {
            DCache_hit++;
            SetDCacheMRU(index, i);
            return;
        }
    }
    DCache_miss++;
    for(i=0 ; i<D_cache_associativity ; i++)
    {
        if(GetDCacheEntry(index, i, 0)==0)
        {
            SetDCacheEntry(index, i, 0, 1);
            SetDCacheMRU(index, i);
            SetDCacheEntry(index, i, 2, tag);
            return;
        }
    }
    for(i=0 ; i<D_cache_associativity ; i++)
    {
        if(GetDCacheEntry(index, i, 1)==0)
        {
            SetDCacheMRU(index, i);
            SetDCacheEntry(index, i, 2, tag);
            return;
        }
    }
}
