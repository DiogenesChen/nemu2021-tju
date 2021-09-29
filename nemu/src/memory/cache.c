#include "nemu.h"
#include "memory/cache.h"
#include <time.h>
#include "burst.h"
#include <stdlib.h>

void init_cache() {
  int i;
  for (i = 0; i < CACHE_L1_SET_NUM * CACHE_L1_WAY_NUM; i++) {
    cache_L1[i].validVal = false;
  }
  for (i = 0; i < CACHE_L2_SET_NUM * CACHE_L2_WAY_NUM; i++) {
    cache_L2[i].dirtyVal = false;
    cache_L2[i].validVal = false;
  }
  return;
}

void ddr3_read_me(hwaddr_t addr, void* data);

// return whole index of way in cacheL1
int read_cache_L1(hwaddr_t addr) {
  uint32_t setIndex = ((addr >> CACHE_BLOCK_BIT) & (CACHE_L1_SET_NUM - 1));
  uint32_t tag = (addr >> (CACHE_BLOCK_BIT + CACHE_L1_SET_BIT));
  // uint32_t block_start = ((addr >> CACHE_BLOCK_BIT) << CACHE_BLOCK_BIT);

  int wayIndex;
  int whole_begin_wayIndex = setIndex * CACHE_L1_WAY_NUM;
  int whole_end_wayIndex = (setIndex + 1) * CACHE_L1_WAY_NUM;
  for (wayIndex = whole_begin_wayIndex; wayIndex < whole_end_wayIndex; wayIndex++)
    if (cache_L1[wayIndex].validVal && cache_L1[wayIndex].tag == tag) // Hit!
      return wayIndex;
  // Hit loss!
  // go to cacheL2
  srand(time(0));
  int wayIndex_L2 = read_cache_L2(addr);
  wayIndex = whole_begin_wayIndex + rand() % CACHE_L1_WAY_NUM;
  memcpy(cache_L1[wayIndex].data, cache_L2[wayIndex_L2].data, CACHE_BLOCK_SIZE);

  cache_L1[wayIndex].validVal = true;
  cache_L1[wayIndex].tag = tag;
  return wayIndex;
}

void ddr3_write_me(hwaddr_t addr, void* data, uint8_t* mask);

// return whole index of way in cacheL2
int read_cache_L2(hwaddr_t addr) {
  uint32_t setIndex = ((addr >> CACHE_BLOCK_BIT) & (CACHE_L2_SET_NUM - 1));
  uint32_t tag = (addr >> (CACHE_BLOCK_BIT + CACHE_L2_SET_BIT));
  uint32_t block_start = ((addr >> CACHE_BLOCK_BIT) << CACHE_BLOCK_BIT);

  int wayIndex;
  int whole_begin_wayIndex = setIndex * CACHE_L2_WAY_NUM;
  int whole_end_wayIndex = (setIndex + 1) * CACHE_L2_WAY_NUM;
  for (wayIndex = whole_begin_wayIndex; wayIndex < whole_end_wayIndex; wayIndex++)
    if (cache_L2[wayIndex].validVal && cache_L2[wayIndex].tag == tag) return wayIndex; // Hit!
  // Hit loss!
  srand(time(0));
  wayIndex = whole_begin_wayIndex + rand() % CACHE_L2_WAY_NUM;
  int i;
  if (cache_L2[wayIndex].validVal && cache_L2[wayIndex].dirtyVal) {
    // write down
    uint8_t tmp[BURST_LEN << 1];
    memset(tmp, 1, sizeof(tmp));
    uint32_t block_start_x = (cache_L2[wayIndex].tag << (CACHE_L2_SET_BIT + CACHE_BLOCK_BIT)) | (setIndex << CACHE_BLOCK_BIT);
    for (i = 0; i < CACHE_BLOCK_SIZE / BURST_LEN; i++) {
      ddr3_write_me(block_start_x + BURST_LEN * i, cache_L2[wayIndex].data + BURST_LEN * i, tmp);
    }
  }
  for (i = 0; i < CACHE_BLOCK_SIZE / BURST_LEN; i++) {
    ddr3_read_me(block_start + BURST_LEN * i, cache_L2[wayIndex].data + BURST_LEN * i);
  }
  cache_L2[wayIndex].validVal = true;
  cache_L2[wayIndex].dirtyVal = false;
  cache_L2[wayIndex].tag = tag;
  return wayIndex;
}

void dram_write(hwaddr_t addr, size_t len, uint32_t data);

void write_cache_L1(hwaddr_t addr, size_t len, uint32_t data) {
  uint32_t setIndex = ((addr >> CACHE_BLOCK_BIT) & (CACHE_L1_SET_NUM - 1));
  uint32_t tag = (addr >> (CACHE_BLOCK_BIT + CACHE_L1_SET_BIT));
  uint32_t block_bias = addr & (CACHE_BLOCK_SIZE - 1);
  int wayIndex;
  int whole_begin_wayIndex = setIndex * CACHE_L1_WAY_NUM;
  int whole_end_wayIndex = (setIndex + 1) * CACHE_L1_WAY_NUM;
  for (wayIndex = whole_begin_wayIndex; wayIndex < whole_end_wayIndex; wayIndex++) {
    if (cache_L1[wayIndex].validVal && cache_L1[wayIndex].tag == tag) {
      // Hit!
      // write through
      if (block_bias + len > CACHE_BLOCK_SIZE) {
        dram_write(addr, CACHE_BLOCK_SIZE - block_bias, data);
        memcpy(cache_L1[wayIndex].data + block_bias, &data, CACHE_BLOCK_SIZE - block_bias);
        write_cache_L2(addr, CACHE_BLOCK_SIZE - block_bias, data);
        write_cache_L1(addr + CACHE_BLOCK_SIZE - block_bias, len - (CACHE_BLOCK_SIZE - block_bias), data >> (CACHE_BLOCK_SIZE - block_bias));
      } else {
        dram_write(addr, len, data);
        memcpy(cache_L1[wayIndex].data + block_bias, &data, len);
        write_cache_L2(addr, len, data);
      }
      return;
    }
  }
  //  Hit loss!
  // not write allocate
  write_cache_L2(addr, len, data);
  return;
}

void write_cache_L2(hwaddr_t addr, size_t len, uint32_t data) {
  uint32_t setIndex = ((addr >> CACHE_BLOCK_BIT) & (CACHE_L2_SET_NUM - 1));
  uint32_t tag = (addr >> (CACHE_BLOCK_BIT + CACHE_L2_SET_BIT));
  uint32_t block_bias = addr & (CACHE_BLOCK_SIZE - 1);
  int wayIndex;
  int whole_begin_wayIndex = setIndex * CACHE_L2_WAY_NUM;
  int whole_end_wayIndex = (setIndex + 1) * CACHE_L2_WAY_NUM;
  for (wayIndex = whole_begin_wayIndex; wayIndex < whole_end_wayIndex; wayIndex++) {
    if (cache_L2[wayIndex].validVal && cache_L2[wayIndex].tag == tag) {
      // Hit!
      // write back
      cache_L2[wayIndex].dirtyVal = true;
      if (block_bias + len > CACHE_BLOCK_SIZE) {
        memcpy(cache_L2[wayIndex].data + block_bias, &data, CACHE_BLOCK_SIZE - block_bias);
        write_cache_L2(addr + CACHE_BLOCK_SIZE - block_bias, len - (CACHE_BLOCK_SIZE - block_bias), data >> (CACHE_BLOCK_SIZE - block_bias));
      } else {
        memcpy(cache_L2[wayIndex].data + block_bias, &data, len);
      }
      return;
    }
  }
  //  Hit loss!
  // write allocate
  wayIndex = read_cache_L2(addr);
  cache_L2[wayIndex].dirtyVal = true;
  memcpy(cache_L2[wayIndex].data + block_bias, &data, len);
  return;
}
