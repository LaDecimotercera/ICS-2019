#include "nemu.h"

#define PTE_ADDR(pte)    ((uint32_t)(pte) & ~0xfff)
#define PDX(va)          (((uint32_t)(va) >> PDXSHFT) & 0x3ff)
#define PTX(va)          (((uint32_t)(va) >> PTXSHFT) & 0x3ff)
#define OFF(va)          ((uint32_t)(va) & 0xfff)
#define PTXSHFT          12      // Offset of PTX in a linear address
#define PDXSHFT          22      // Offset of PDX in a linear address
// Page table/directory entry flags
#define PTE_P          0x001   // Present
#define PTE_W          0x002   // Writeable
#define PTE_U          0x004   // User
#define PTE_PWT        0x008   // Write-Through
#define PTE_PCD        0x010   // Cache-Disable
#define PTE_A          0x020   // Accessed
#define PTE_D          0x040   // Dirty

// +--------10------+-------10-------+---------12----------+
// | Page Directory |   Page Table   | Offset within Page  |
// |      Index     |      Index     |                     |
// +----------------+----------------+---------------------+
//  \--- PDX(va) --/ \--- PTX(va) --/\------ OFF(va) ------/

paddr_t page_translate(vaddr_t vaddr) {
  /*paddr_t dir = PDX(addr);
  paddr_t page = PTX(addr);
  paddr_t offset = OFF(addr);
  paddr_t PDT_base = cpu.cr3.page_directory_base;*/
  
  /*paddr_t PDT_base = PTE_ADDR(cpu.cr3.val);
  assert(paddr_read(PDT_base + PDX(addr) * sizeof(PDE), sizeof(PDE)) & PTE_P);
  paddr_t PTE_base = PTE_ADDR(paddr_read(PDT_base + PDX(addr) * sizeof(PDE), sizeof(PDE))); 
  assert(paddr_read(PTE_base + PTX(addr) * sizeof(PTE), sizeof(PTE)) & PTE_P);
  paddr_t PF_base = PTE_ADDR(paddr_read(PTE_base + PTX(addr) * sizeof(PTE), sizeof(PTE)));
  paddr_t paddr = PF_base | OFF(addr);
  return paddr;*/
  paddr_t dir = PTE_ADDR(cpu.cr3.val);
  assert(paddr_read(dir + sizeof(paddr_t) * PDX(vaddr), sizeof(paddr_t)) & PTE_P);
  
  paddr_t pg = PTE_ADDR(paddr_read(dir + sizeof(paddr_t) * PDX(vaddr), sizeof(paddr_t)));
  assert(paddr_read(pg + sizeof(paddr_t) * PTX(vaddr), sizeof(paddr_t)) & PTE_P);
  
  //Log("Page translate paddr: 0x%08x", PTE_ADDR(paddr_read(pg + sizeof(paddr_t) * PTX(vaddr), sizeof(paddr_t))) | OFF(vaddr));
  return (PTE_ADDR(paddr_read(pg + sizeof(paddr_t) * PTX(vaddr), sizeof(paddr_t))) | OFF(vaddr)); 
}

uint32_t isa_vaddr_read(vaddr_t addr, int len) {
  if (cpu.cr0.paging == 1) {
    if (PTE_ADDR(addr) != PTE_ADDR(addr + len - 1)) {
      /* this is a special case, you can handle it later. */
      assert(0);
    }
    else {
      paddr_t paddr = page_translate(addr);
      return paddr_read(paddr, len);
    }
  }
  return paddr_read(addr, len);
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len) {
  if (cpu.cr0.paging == 1) {
    if (PTE_ADDR(addr) != PTE_ADDR(addr + len - 1)) {
      assert(0);
    } 
    else {
      paddr_t paddr = page_translate(addr);
      return paddr_write(paddr, data, len);
    }
  }
  return paddr_write(addr, data, len);
}
