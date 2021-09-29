#include "common.h"
#include "memory/tlb.h"
#include "memory/cache.h"
#include "nemu.h"
#include "burst.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

lnaddr_t seg_translate(swaddr_t addr, size_t len, uint8_t sreg_id) {
  if (cpu.cr0.protect_enable == 0) return addr;
  else {
    return cpu.sreg[sreg_id].base + addr;
  }
}

hwaddr_t page_translate(lnaddr_t addr) {
  if (cpu.cr0.protect_enable == 1 && cpu.cr0.paging == 1) {
    uint32_t dir = addr >> 22;
    uint32_t page = (addr >> 12) & 0x3ff;
    uint32_t bias = addr & 0xfff;

    int i = read_tlb(addr);
    if (i != -1) return (tlb[i].page_num << 12) + bias;

    uint32_t dir_start = cpu.cr3.page_directory_base;
    uint32_t dir_position = (dir_start << 12) + (dir << 2);
    Page_Descriptor first_content;
    first_content.val = hwaddr_read(dir_position, 4);
    Assert(first_content.p == 1, "Page unavailable");
    uint32_t page_start = first_content.addr;
    uint32_t page_pos = (page_start << 12) + (page << 2);
    Page_Descriptor second_content;
    second_content.val =  hwaddr_read(page_pos, 4);
    Assert(second_content.p == 1, "Page unavailable");
    uint32_t addr_start = second_content.addr;
    hwaddr_t hwaddr = (addr_start << 12) + bias;
    write_tlb(addr, hwaddr);
    return hwaddr;
  } else {
    return addr;
  }
}


/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
  int cache_L1_way_1_index = read_cache_L1(addr);
  uint32_t block_bias = addr & (CACHE_BLOCK_SIZE - 1);
  uint8_t ret[BURST_LEN << 1];
  //printf("%d\n", block_bias);
  if (block_bias + len > CACHE_BLOCK_SIZE) {
    int cache_L1_way_2_index = read_cache_L1(addr + CACHE_BLOCK_SIZE - block_bias);
    memcpy(ret, cache_L1[cache_L1_way_1_index].data + block_bias, CACHE_BLOCK_SIZE - block_bias);
    memcpy(ret  + CACHE_BLOCK_SIZE - block_bias, cache_L1[cache_L1_way_2_index].data, len - (CACHE_BLOCK_SIZE - block_bias));
  } else {
    memcpy(ret, cache_L1[cache_L1_way_1_index].data + block_bias, len);
  }
  int tmp = 0;
  uint32_t ans = unalign_rw(ret + tmp, 4) & (~0u >> ((4 - len) << 3));
  return ans;
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
  write_cache_L1(addr, len, data);
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
  assert(len == 1 || len == 2 || len == 4);
  uint32_t cur_bias = addr & 0xfff;
  if (cur_bias + len - 1 > 0xfff) {
    //assert(0); depend on the mechine
    size_t len1 = 0xfff - cur_bias + 1;
    size_t len2 = len - len1;
    uint32_t addr_len1 = lnaddr_read(addr, len1);
    uint32_t addr_len2 = lnaddr_read(addr + len1, len2);
    uint32_t value = (addr_len2 << (len1 << 3)) | addr_len1;
    return value;

  } else {
    hwaddr_t hwaddr = page_translate(addr);
    return hwaddr_read(hwaddr, len);
  }
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
  assert(len == 1 || len == 2 || len == 4);
  uint32_t cur_bias = addr & 0xfff;
  if (cur_bias + len - 1 > 0xfff) {
    //assert(0); depend on the mechine
    size_t len1 = 0xfff - cur_bias + 1;
    size_t len2 = len - len1;
    lnaddr_write(addr, len1, data & ((1 << (len1 << 3)) - 1));
    lnaddr_write(addr + len1, len2, data >> (len1 << 3));
  } else {
    hwaddr_t hwaddr = page_translate(addr);
    hwaddr_write(hwaddr, len, data);
  }
}

uint32_t swaddr_read(swaddr_t addr, size_t len) {
#ifdef DEBUG
  assert(len == 1 || len == 2 || len == 4);
#endif
  lnaddr_t lnaddr = seg_translate(addr, len, current_sreg);
  return lnaddr_read(lnaddr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data) {
#ifdef DEBUG
  assert(len == 1 || len == 2 || len == 4);
#endif
  lnaddr_t lnaddr = seg_translate(addr, len, current_sreg);
  lnaddr_write(lnaddr, len, data);
}