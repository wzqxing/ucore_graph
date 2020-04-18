#ifndef BIOS32_H
#define BIOS32_H
#include <defs.h>
#include <mmu.h>
#include <pmm.h>
typedef struct register16 {
    uint16_t di; 
    uint16_t si; 
    uint16_t bp; 
    uint16_t sp; 
    uint16_t bx; 
    uint16_t dx; 
    uint16_t cx; 
    uint16_t ax; 

    uint16_t ds; 
    uint16_t es; 
    uint16_t fs; 
    uint16_t gs; 
    uint16_t ss; 
    uint16_t eflags;
} register16_t;

extern void bios32_helper();
extern void bios32_helper_end();
extern void *asm_gdt_entries;
extern void *asm_gdt_ptr;
extern void *asm_idt_ptr;
extern void *asm_in_reg_ptr;
extern void *asm_out_reg_ptr;
extern void *asm_intnum_ptr;
extern struct segdesc gdt[8];
extern struct pseudodesc gdt_pd;
extern struct pseudodesc idt_pd;

#define REBASE(x) ((void*)(0x9000 + (uint32_t)x - (uint32_t)bios32_helper))

#define BIOS_GRAPHICS_SERVICE 0x10

void bios32_init();
void bios32_service(uint8_t int_num, register16_t * in_reg, register16_t * out_reg);

#endif
