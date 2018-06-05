//========================================================//
//  cache.c                                               //
//  Source file for the Cache Simulator                   //
//                                                        //
//  Implement the I-cache, D-Cache and L2-cache as        //
//  described in the README                               //
//========================================================//

#include "cache.h"
#include <string.h>
#include <stdio.h>

#define DEBUG 0
#define bit_ADDR 32

//
// TODO:Student Information
//
const char *studentName = "Huan Nguyen";
const char *studentID   = "A12871523";
const char *email       = "hpn007@ucsd.edu";

uint32_t numBits(uint32_t input);
void getInfo(uint32_t input, uint32_t aBitBlock, uint32_t aBitIndex, uint32_t* rTag, uint32_t* rIdx);
void invalidate(uint32_t addr);

//Each memory slot hold a block of memory inside the cache
//This is the atom of the cache
typedef struct 
{
	uint32_t tag;

	uint32_t valid;
	uint64_t lru;
} MemSlot;

//Each MemBlock hold (n-way) blocks of MemSlot
typedef struct 
{
	MemSlot* slots;	
	uint64_t clock_val;
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

uint32_t bit_block;
uint32_t bit_1D_index;
uint32_t bit_1I_index;
uint32_t bit_2L_index;

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

	if (DEBUG)
	{
		fprintf(stderr, "Num Bit ICache Set:    %d\n", numBits(icacheSets));
		fprintf(stderr, "Num Bit ICache Assoc:  %d\n", numBits(icacheAssoc));
		fprintf(stderr, "Num Bit Dcache Set:    %d\n", numBits(dcacheSets));
		fprintf(stderr, "Num Bit Dcache Assoc:  %d\n", numBits(dcacheAssoc));
		fprintf(stderr, "Num Bit l2cache Set:   %d\n", numBits(l2cacheSets));
		fprintf(stderr, "Num Bit l2cache Assoc: %d\n", numBits(l2cacheAssoc));
		fprintf(stderr, "Num Bit Block Offset:  %d\n", numBits(blocksize));
	}

	bit_block = numBits(blocksize);
	bit_1D_index = numBits(dcacheSets);
	bit_1I_index = numBits(icacheSets);
	bit_2L_index = numBits(l2cacheSets);

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
			curBlock->clock_val = 0;
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
			curBlock->clock_val = 0;
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
			curBlock->clock_val = 0;
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
	
	//Uninitialized Cache
	if (!is_1I_init)
		return l2cache_access(addr);
	
	icacheRefs++;
	//Figure out block offset, index, and tag
	uint32_t tag;
	uint32_t idx;

	//Get tag and index 
	getInfo(addr, bit_block, bit_1I_index, &tag, &idx);

	if (DEBUG)
	{
		fprintf(stderr, "\n-------------I Cache---------------\n");
		fprintf(stderr, "Input: %X\n", addr);
		fprintf(stderr, "Index: %X\n", idx);
		fprintf(stderr, "Tag:   %X\n", tag);
	}

	//Now we should have tag and index
	
	//Check if the given block is in the cache
	//Get the specified set
	MemBlock* curSet = &(cache_1I[idx]);

	int i = 0;
	int occupied = 0;
	//Looking into (n-way) to see if we have a matching tag
	for (i = 0; i < icacheAssoc; i++)
	{
		MemSlot* curSlot = &(curSet->slots[i]);
		if (curSlot->valid == TRUE)
			occupied++;

		if (DEBUG)
			fprintf(stderr, "Slot %d: %x\n", i, curSlot->tag);

		//Cache Hit
		if (curSlot->tag == tag && curSlot->valid == TRUE)
		{
			if (DEBUG)
				fprintf(stderr, "L1 Cache HIT\n");
			//Update the LRU
			uint64_t curClock = ++curSet->clock_val;
			curSlot->lru = curClock;
			if (DEBUG)
				fprintf(stderr, "Total Hit Time: %d\n", icacheHitTime);
			return icacheHitTime;
			//found = TRUE;
			//break;
		}
	}

	//Cache Miss
	if (DEBUG)
		fprintf(stderr, "L1 Cache MISS\n");
	icacheMisses++;
	
	//TODO: Try to contact L2 Cache before put it in 
	uint32_t l2Pen = l2cache_access(addr);	
	icachePenalties+=l2Pen;
	
	//IF there is an invalid slot
	if (occupied < icacheAssoc)
	{
		for (i = 0; i < icacheAssoc; i++)
		{
			MemSlot* curSlot = &(curSet->slots[i]);
			if (!curSlot->valid)
			{
				//Begin replacement
				curSet->slots[i].tag = tag;
				curSet->slots[i].valid = TRUE;
				curSet->slots[i].lru = ++curSet->clock_val;
				//Done replace. Return hit time and penalty for accessing L2
				if (DEBUG)
					fprintf(stderr, "Total Hit Time: %d\n", icacheHitTime + l2Pen);
				return icacheHitTime + l2Pen;
			}
		}
	}

	else
	{
		//Looking for minimum LRU value
		uint64_t minVal = curSet->slots[0].lru;
		int minIndex = 0;
		for (i = 0; i < icacheAssoc; i++)
		{
			MemSlot* curSlot = &(curSet->slots[i]);
			if (curSlot->lru < minVal)
			{
				minVal = curSlot->lru;
				minIndex = i;
			}
		}
		//Begin replacement
		curSet->slots[minIndex].tag = tag;
		curSet->slots[minIndex].valid = TRUE;
		curSet->slots[minIndex].lru = ++curSet->clock_val;
	}
	//Done replace. Return hit time and penalty for accessing L2
	if (DEBUG)
		fprintf(stderr, "Total Hit Time: %d\n", icacheHitTime + l2Pen);
	return icacheHitTime + l2Pen;
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
	
	//Uninitialized Cache
	if (!is_1D_init)
		return l2cache_access(addr);
	
	dcacheRefs++;
	//Figure out block offset, index, and tag
	uint32_t tag;
	uint32_t idx;

	//Get tag and index 
	getInfo(addr, bit_block, bit_1D_index, &tag, &idx);

	if (DEBUG)
	{
		fprintf(stderr, "\n-------------D Cache---------------\n");
		fprintf(stderr, "Input: %X\n", addr);
		fprintf(stderr, "Index: %X\n", idx);
		fprintf(stderr, "Tag:   %X\n", tag);
	}

	//Now we should have tag and index
	
	//Check if the given block is in the cache
	//Get the specified set
	MemBlock* curSet = &(cache_1D[idx]);

	int i = 0;
	int occupied = 0;
	//Looking into (n-way) to see if we have a matching tag
	for (i = 0; i < dcacheAssoc; i++)
	{
		MemSlot* curSlot = &(curSet->slots[i]);
		if (curSlot->valid == TRUE)
			occupied++;

		if (DEBUG)
			fprintf(stderr, "Slot %d: %x\n", i, curSlot->tag);

		//Cache Hit
		if (curSlot->tag == tag && curSlot->valid == TRUE)
		{
			if (DEBUG)
				fprintf(stderr, "L1 Cache HIT\n");
			//Update the LRU
			uint64_t curClock = ++curSet->clock_val;
			curSlot->lru = curClock;
			if (DEBUG)
				fprintf(stderr, "Total Hit Time: %d\n", dcacheHitTime);
			return dcacheHitTime;
			//found = TRUE;
			//break;
		}
	}

	//Cache Miss
	if (DEBUG)
		fprintf(stderr, "L1 Cache MISS\n");
	dcacheMisses++;
	
	//TODO: Try to contact L2 Cache before put it in 
	uint32_t l2Pen = l2cache_access(addr);	
	dcachePenalties+=l2Pen;
	
	//IF there is an invalid slot
	if (occupied < dcacheAssoc)
	{
		for (i = 0; i < dcacheAssoc; i++)
		{
			MemSlot* curSlot = &(curSet->slots[i]);
			if (!curSlot->valid)
			{
				//Begin replacement
				curSet->slots[i].tag = tag;
				curSet->slots[i].valid = TRUE;
				curSet->slots[i].lru = ++curSet->clock_val;
				//Done replace. Return hit time and penalty for accessing L2
				if (DEBUG)
					fprintf(stderr, "Total Hit Time: %d\n", dcacheHitTime + l2Pen);
				return dcacheHitTime + l2Pen;
			}
		}
	}

	else
	{
		//Looking for minimum LRU value
		uint64_t minVal = curSet->slots[0].lru;
		int minIndex = 0;
		for (i = 0; i < dcacheAssoc; i++)
		{
			MemSlot* curSlot = &(curSet->slots[i]);
			if (curSlot->lru < minVal)
			{
				minVal = curSlot->lru;
				minIndex = i;
			}
		}
		//Begin replacement
		curSet->slots[minIndex].tag = tag;
		curSet->slots[minIndex].valid = TRUE;
		curSet->slots[minIndex].lru = ++curSet->clock_val;
	}
	//Done replace. Return hit time and penalty for accessing L2
	if (DEBUG)
		fprintf(stderr, "Total Hit Time: %d\n", dcacheHitTime + l2Pen);
	return dcacheHitTime + l2Pen;
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
	
	//Uninitialized Cache
	if (!is_2L_init)
		return memspeed;
	l2cacheRefs++;	
	//Figure out block offset, index, and tag
	uint32_t tag;
	uint32_t idx;

	//Get tag and index 
	getInfo(addr, bit_block, bit_2L_index, &tag, &idx);

	if (DEBUG)
	{
		fprintf(stderr, "-------------L2 Cache--------------\n");
		fprintf(stderr, "Input: %X\n", addr);
		fprintf(stderr, "Index: %X\n", idx);
		fprintf(stderr, "Tag:   %X\n", tag);
	}

	//Now we should have tag and index
	
	//Check if the given block is in the cache
	//Get the specified set
	MemBlock* curSet = &(cache_2L[idx]);

	int i = 0;
	int occupied = 0;
	//Looking into (n-way) to see if we have a matching tag
	for (i = 0; i < l2cacheAssoc; i++)
	{
		MemSlot* curSlot = &(curSet->slots[i]);
		if (curSlot->valid == TRUE)
			occupied++;

		if (DEBUG)
			fprintf(stderr, "Slot %d: %x\n", i, curSlot->tag);

		//Cache Hit
		if (curSlot->tag == tag && curSlot->valid == TRUE)
		{
			if (DEBUG)
				fprintf(stderr, "L2 Cache HIT\n");
			//Update the LRU
			uint64_t curClock = ++curSet->clock_val;
			curSlot->lru = curClock;
			return l2cacheHitTime;
		}
	}

	//Cache Miss
	if (DEBUG)
		fprintf(stderr, "L2 Cache MISS\n");
	l2cacheMisses++;
	l2cachePenalties += memspeed;
	
	//TODO: Try to contact L2 Cache before put it in 
	
	
	//IF there is an invalid slot
	if (occupied < l2cacheAssoc)
	{
		for (i = 0; i < l2cacheAssoc; i++)
		{
			MemSlot* curSlot = &(curSet->slots[i]);
			if (!curSlot->valid)
			{
				//Begin replacement
				curSet->slots[i].tag = tag;
				curSet->slots[i].valid = TRUE;
				curSet->slots[i].lru = ++curSet->clock_val;
				return l2cacheHitTime + memspeed;
			}
		}
	}

	//Full cache
	else
	{
		//Looking for minimum LRU value
		uint64_t minVal = curSet->slots[0].lru;
		int minIndex = 0;
		for (i = 0; i < l2cacheAssoc; i++)
		{
			MemSlot* curSlot = &(curSet->slots[i]);
			if (curSlot->lru < minVal)
			{
				minVal = curSlot->lru;
				minIndex = i;
			}
		}

		if (inclusive)
		{
			//Evict something out of the Cache
			//We need to reconstruct the address stored in the slot
			uint32_t slotTag = curSet->slots[minIndex].tag;
			uint32_t original = slotTag << bit_2L_index;
			original = original | idx;
			original = original << bit_block;
			if (DEBUG)
				fprintf(stderr, "L2 Evict Original: %X\n", original);
			invalidate(original);
		}
		//Begin replacement
		curSet->slots[minIndex].tag = tag;
		curSet->slots[minIndex].valid = TRUE;
		curSet->slots[minIndex].lru = ++curSet->clock_val;
	}
	return l2cacheHitTime + memspeed;
}


/*--------------Helper methods----------------*/

//Take in 32 bits number and return the number of bits
uint32_t numBits(uint32_t input)
{
	//Mask for the last bit/*{{{*/
	uint32_t mask = 1;
	uint32_t curBit = 0;
	uint32_t numBit = 0;

	int i = 1;

	//Skip first bit because that would be 2^0
	input = input >> 1;

	for (i = 1; i < 32; i++)
	{
		curBit = input & mask;
		if (curBit != 0)
			numBit = i;

		input = input >> 1;
	}
	return numBit;/*}}}*/
}

void getInfo(uint32_t input, uint32_t aBitBlock, uint32_t aBitIndex, uint32_t* rTag, uint32_t* rIdx)
{
	//Get rid of the block /*{{{*/
	input = input >> aBitBlock;

	uint32_t mask = -1;
	int numMask = 32 - aBitIndex;
	mask = mask << numMask >> numMask;

	//Get the index
	*rIdx = input & mask;
	
	//Create new mask for the tag
	uint32_t aBitTag = 32 - aBitBlock - aBitIndex;
	numMask = 32 - aBitTag;
	mask = -1;
	mask = mask << numMask >> numMask;

	input = input >> aBitIndex;
	*rTag = input & mask;
/*}}}*/
}

void invalidate(uint32_t addr)
{
	uint32_t invalid_1I_tag;/*{{{*/
	uint32_t invalid_1I_idx;
	uint32_t invalid_1D_tag;
	uint32_t invalid_1D_idx;
	getInfo(addr, bit_block, bit_1D_index, &invalid_1D_tag, &invalid_1D_idx);
	getInfo(addr, bit_block, bit_1I_index, &invalid_1I_tag, &invalid_1I_idx);

	MemBlock* curSet;
	int i = 0;

	if (is_1D_init)
	{
		curSet = &(cache_1D[invalid_1D_idx]);
		//Looking into (n-way) to see if we have a matching tag
		for (i = 0; i < dcacheAssoc; i++)
		{
			MemSlot* curSlot = &(curSet->slots[i]);
			//Cache Hit
			if (curSlot->tag == invalid_1D_tag)
			{
				if (DEBUG)
					fprintf(stderr, "Eliminated TAG in 1D: %X\n", invalid_1D_tag);
				curSlot->valid = FALSE;
				return;
			}
		}
	}
	
	if (is_1I_init)
	{
		curSet = &(cache_1I[invalid_1I_idx]);
		//Looking into (n-way) to see if we have a matching tag
		for (i = 0; i < icacheAssoc; i++)
		{
			MemSlot* curSlot = &(curSet->slots[i]);
			//Cache Hit
			if (curSlot->tag == invalid_1I_tag)
			{
				if (DEBUG)
					fprintf(stderr, "Eliminated TAG in 1I: %X\n", invalid_1I_tag);
				curSlot->valid = FALSE;
				return;
			}
		}
	}/*}}}*/
}
