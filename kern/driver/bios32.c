#include <bios32.h>
#include <x86.h>
#include <mmu.h>
#include <pmm.h>
#include <string.h>
#include <stdio.h>

static struct pseudodesc real_gdt_ptr;
static struct pseudodesc real_idt_ptr;
extern void init_temp_map();
extern void create_temp_map();
extern void destory_temp_map();

#define REALLOC(x) (x - KERNBASE)
void (*rebased_bios32_helper)() = (void*)(0x9000);

/*
 * Prepare some descriptors for calling bios service from protected mode
 * We're using the easiest method : switch to real mode to call bios temporarily
 * So, prepare some 16bit global descriptors, and interrupt descriptors
 * */
                                       
#define SEG_REAL(type, base, lim, access) \
    (struct segdesc) {                                      \
        ((lim) >> 12) & 0xffff, (base) & 0xffff,            \
        ((base) >> 16) & 0xff, (type), 1, (access) & 0x3, 1,             \
        (unsigned)(lim) >> 28, 0, 0, 0, 0,                  \
        (unsigned) (base) >> 24                             \
    }


void bios32_init() {
    // 16bit code segment
    gdt[6] = SEG_REAL(STA_X | STA_R, 0x0, 0xFFFFFFFF, DPL_KERNEL);
    // 16bit data segment
    gdt[7] = SEG_REAL(STA_W, 0x0, 0xFFFFFFFF, DPL_KERNEL);
    // gdt ptr
    real_gdt_ptr.pd_base = (uint32_t)gdt;
    real_gdt_ptr.pd_lim = sizeof(gdt) - 1;
    // idt ptr
    real_idt_ptr.pd_base = 0;
    real_idt_ptr.pd_lim = 0x3ff;
    uint32_t size = (uint32_t)bios32_helper_end - (uint32_t)bios32_helper;
    if(size > 4096){
         panic("That's not enough memory for bios32_helper, fix thix!\n");
    }
#if 0
    int i = 0;
    for (i = 0; i < 1024; i ++) {
        if (i < 256) {
            boot_pt1[i] = i * PGSIZE + (PTE_P | PTE_W);
        } else {
            boot_pt1[i] = 0;
        }
    }
#endif
    cprintf("bios init end\n");
}

/*
 * Switch to real mode, do bios, then switch back to protected mode
 * need to move code to 0x7c00 for execution
 * the function that actually do all the mode switch and init work is in bios32_helper.asm (bios32_helper)
 * */

/*
 * 此处需要映射物理地址0-1M到虚拟地址0-1M
 * */
void bios32_service(uint8_t int_num, register16_t * in_reg, register16_t * out_reg) {
    void * new_code_base = (void*)(0x9000);

    memcpy(&asm_gdt_entries, gdt, sizeof(gdt));

    real_gdt_ptr.pd_base = (uint32_t)REBASE((&asm_gdt_entries));
    memcpy(&asm_gdt_ptr, &real_gdt_ptr, sizeof(real_gdt_ptr));
    memcpy(&asm_idt_ptr, &real_idt_ptr, sizeof(real_idt_ptr));

    memcpy(&asm_in_reg_ptr, in_reg, sizeof(register16_t));
    memcpy(&asm_intnum_ptr, &int_num, sizeof(uint8_t));

    uint32_t size = (uint32_t)bios32_helper_end - (uint32_t)bios32_helper;
    memcpy(new_code_base, (void *)bios32_helper, size);
    rebased_bios32_helper();

    void *t = REBASE(&asm_out_reg_ptr);
    memcpy(out_reg, t, sizeof(register16_t));

    lgdt(&gdt_pd);
    lidt(&idt_pd);
}

