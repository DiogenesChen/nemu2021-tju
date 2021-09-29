#ifndef __CACHE_H__
#define __CACHE_H__

#include "common.h"

#define CACHE_BLOCK_BIT 6
#define CACHE_L1_WAY_BIT 3
#define CACHE_L1_SET_BIT 7
#define CACHE_L2_WAY_BIT 4
#define CACHE_L2_SET_BIT 12
#define CACHE_L2_CAP (4 * 1024 * 1024)
#define CACHE_L1_CAP (64 * 1024)

#define CACHE_BLOCK_SIZE (1 << CACHE_BLOCK_BIT)
#define CACHE_L1_WAY_NUM (1 << CACHE_L1_WAY_BIT)
#define CACHE_L1_SET_NUM (1 << CACHE_L1_SET_BIT)

#define CACHE_L2_WAY_NUM (1 << CACHE_L2_WAY_BIT)
#define CACHE_L2_SET_NUM (1 << CACHE_L2_SET_BIT)

typedef struct{
    uint8_t data[CACHE_BLOCK_SIZE];
    uint32_t tag;
    bool validVal;
} L1;

L1 cache_L1[CACHE_L1_SET_NUM * CACHE_L1_WAY_NUM];

void init_cache();

int read_cache_L1(hwaddr_t);

void write_cache_L1(hwaddr_t, size_t, uint32_t);


typedef struct{
    uint8_t data[CACHE_BLOCK_SIZE];
    uint32_t tag;
    bool validVal;
    bool dirtyVal;
} L2;

L2 cache_L2[CACHE_L2_SET_NUM * CACHE_L2_WAY_NUM];

void init_cache();

int read_cache_L1(hwaddr_t);
int read_cache_L2(hwaddr_t);

void write_cache_L1(hwaddr_t, size_t, uint32_t);
void write_cache_L2(hwaddr_t, size_t, uint32_t);
#endif
