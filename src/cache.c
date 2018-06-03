//========================================================//
//  cache.c                                               //
//  Source file for the Cache Simulator                   //
//                                                        //
//  Implement the I-cache, D-Cache and L2-cache as        //
//  described in the README                               //
//========================================================//

#include "cache.h"
#include <string.h>

//
// TODO:Student Information
//
const char *studentName = "Huan Nguyen";
const char *studentID   = "A12871523";
const char *email       = "hpn007@ucsd.edu";

//Each memory slot hold a block of memory inside the cache
//This is the atom of the cache
typedef struct 
{
	uint32_t tag;

	uint32_t valid;
	uint32_t dirty;
} MemSlot;

//Each MemBlock hold (n-way) blocks of MemSlot
typedef struct 
{
	MemSlot* slots;	
} MemBlock;


//Each cache will hold (index) amount of MemBlocks

//------------------------------------//
//        Cache Configuration         //
//------------------------------------//

uint32_t icacheSets;     // Number of sets in the I$
uint32_t icacheAssoc;    // Associativity of the I$
uint32_t icacheHitTime;  // Hit Time of the I$

uint32_t dcacheSets;     // Number of sets in the D$
uint32_t dcacheAssoc;    // Associativity of the D$
uint32_t dcacheHitTime;  // Hit Time of the D$

uint32_t l2cacheSets;    // Number of sets in the L2$
uint32_t l2cacheAssoc;   // Associativity of the L2$
uint32_t l2cacheHitTime; // Hit Time of the L2$
uint32_t inclusive;      // Indicates if the L2 is inclusive

uint32_t blocksize;      // Block/Line size
uint32_t memspeed;       // Latency of Main Memory

//------------------------------------//
//          Cache Statistics          //
//------------------------------------//

uint64_t icacheRefs;       // I$ references
uint64_t icacheMisses;     // I$ misses
uint64_t icachePenalties;  // I$ penalties

uint64_t dcacheRefs;       // D$ references
uint64_t dcacheMisses;     // D$ misses
uint64_t dcachePenalties;  // D$ penalties

uint64_t l2cacheRefs;      // L2$ references
uint64_t l2cacheMisses;    // L2$ misses
uint64_t l2cachePenalties; // L2$ penalties

//------------------------------------//
//        Cache Data Structures       //
//------------------------------------//

//
//TODO: Add your Cache data structures here
//
uint32_t is_1D_init;
uint32_t is_1I_init;
uint32_t is_2L_init;

MemBlock* cache_1D;
MemBlock* cache_1I;
MemBlock* cache_2L;

//------------------------------------//
//          Cache Functions           //
//------------------------------------//

// Initialize the Cache Hierarchy
//
	void
init_cache()
{
	// Initialize cache stats
	icacheRefs        = 0;
	icacheMisses      = 0;
	icachePenalties   = 0;
	dcacheRefs        = 0;
	dcacheMisses      = 0;
	dcachePenalties   = 0;
	l2cacheRefs       = 0;
	l2cacheMisses     = 0;
	l2cachePenalties  = 0;

	//
	//TODO: Initialize Cache Simulator Data Structures
	//
	is_1D_init = FALSE;
	is_1I_init = FALSE;
	is_2L_init = FALSE;

	//If icache is specified, then initialize the cache
	if (icacheSets)
	{
		is_1I_init = TRUE;
		int i, j;

		//Malloc (sets) amount of MemBlock
		cache_1I = malloc (sizeof(MemBlock) * icacheSets);

		//For each set, initialize (n-way) slots
		for (i = 0; i < icacheSets; i++)
		{
			MemBlock* curBlock = &(cache_1I[i]);
			curBlock->slots = malloc (sizeof(MemSlot) * icacheAssoc);
			for(j = 0; j < icacheAssoc; j++)
			{
				MemSlot* curSlot = &(curBlock->slots[j]);
				memset(curSlot, 0, sizeof(MemSlot));
			}
		}
	}
	
	//If dcache is specified, then initialize the cache
	if (dcacheSets)
	{
		is_1D_init = TRUE;
		int i, j;

		//Malloc (sets) amount of MemBlock
		cache_1D = malloc (sizeof(MemBlock) * dcacheSets);

		//For each set, initialize (n-way) slots
		for (i = 0; i < dcacheSets; i++)
		{
			MemBlock* curBlock = &(cache_1D[i]);
			curBlock->slots = malloc (sizeof(MemSlot) * dcacheAssoc);
			for(j = 0; j < dcacheAssoc; j++)
			{
				MemSlot* curSlot = &(curBlock->slots[j]);
				memset(curSlot, 0, sizeof(MemSlot));
			}
		}
	}
	
	//If l2cache is specified, then initialize the cache
	if (l2cacheSets)
	{
		is_2L_init = TRUE;
		int i, j;

		//Malloc (sets) amount of MemBlock
		cache_2L = malloc (sizeof(MemBlock) * l2cacheSets);

		//For each set, initialize (n-way) slots
		for (i = 0; i < l2cacheSets; i++)
		{
			MemBlock* curBlock = &(cache_2L[i]);
			curBlock->slots = malloc (sizeof(MemSlot) * l2cacheAssoc);
			for(j = 0; j < l2cacheAssoc; j++)
			{
				MemSlot* curSlot = &(curBlock->slots[j]);
				memset(curSlot, 0, sizeof(MemSlot));
			}
		}
	}
	//End function
}

// Perform a memory access through the icache interface for the address 'addr'
// Return the access time for the memory operation
//
	uint32_t
icache_access(uint32_t addr)
{
	//
	//TODO: Implement I$
	//
	return memspeed;
}

// Perform a memory access through the dcache interface for the address 'addr'
// Return the access time for the memory operation
//
	uint32_t
dcache_access(uint32_t addr)
{
	//
	//TODO: Implement D$
	//
	return memspeed;
}

// Perform a memory access to the l2cache for the address 'addr'
// Return the access time for the memory operation
//
	uint32_t
l2cache_access(uint32_t addr)
{
	//
	//TODO: Implement L2$
	//
	return memspeed;
}
