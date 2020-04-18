; A helper function for calling bios from protected mode
; Execute bios interrupts under given context
; Then Save the result context to a given address
; 0-3ff    1k 用于中断向量表
; 400-4ff 256B  bios数据区
; 500-7Bff  30kb  可自定义区域
; 7C00-7BFF 512b MBR被bios加载到的内存区域
; 7E00-9FBFF 608k 自定义区域
; 9FC00-9FFFF 1kb  扩展bios数据区
; A0000-AFFFF 64k  彩色显示适配器
; B0000 B7FFF 32k  黑白显示适配器
; B8000 BFFFF 32k  文本显示适配器
; C0000 C7FFF 32k 显示适配器bios
; C8000 EFFFF 160k 映射硬件适配器的rom或者内存映射i/o
; F0000 FFFEF 64kb-16b  真正的入口
; FFFF0 FFFFF 16b bios入口地址(跳转使用.因为空间有限)
; 
; 以下在实模式阶段.我们是可以随意使用的
; 7E00-9FBFF 608k 自定义区域
; 500-7Bff  30kb  可自定义区域

;        +1024   0xd000    保存数据
;        +1024   0xc000    stack bios 中断栈
;        +1024   0xb000    stack (real mode stack)
;        +1024   0xa000 
;                0x9000    code

[bits 32]

global bios32_helper
global bios32_helper_end

global asm_gdt_ptr
global asm_gdt_entries
global asm_idt_ptr
global asm_in_reg_ptr
global asm_out_reg_ptr
global asm_intnum_ptr


extern gdt_init
extern idt_init
extern new_gdt_entries;
extern new_gdt_ptr;
extern new_idt_ptr;
extern new_reg_ptr;
extern new_intnum_ptr;

%define REBASE(x)                              (((x) - bios32_helper) + 0x9000)
%define GDTENTRY(x)                            ((x) << 3)
%define CODE32                                 GDTENTRY(1)  ; 0x08
%define DATA32                                 GDTENTRY(2)  ; 0x10
%define CODE16                                 GDTENTRY(6)  ; 0x30
%define DATA16                                 GDTENTRY(7)  ; 0x38


PG_BIT_OFF equ 0x7fffffff
PG_BIT_ON equ 0x80000000

section .text
bios32_helper: use32
    pusha
    mov edx, esp
    ; Now in 32bit protected mode
    ; Disable interrupts
    cli
    ; Turn off paging
    mov ecx, cr0
    and ecx, PG_BIT_OFF
    mov cr0, ecx
    ; Zero cr3(save it in ebx before zeroing it)
    xor ecx, ecx
    mov ebx, cr3
    mov cr3, ecx

    ; Load new gdt
    lgdt [REBASE(asm_gdt_ptr)]

    ; Load idt
    lidt [REBASE(asm_idt_ptr)]
   
    jmp CODE16:REBASE(protected_mode_16)
protected_mode_16:use16
    ; Now in 16bit protected mode
    ; Update data segment selector
    mov ax, DATA16
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Turn off protected mode
    mov eax, cr0
    and  al,  ~0x01
    mov cr0, eax

    jmp 0x0:REBASE(real_mode_16)
real_mode_16:use16
    ; 16 bit real mode data segment
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov sp, 0xb000

    sti

    ; ### Save current context ###
    pusha
    mov cx, ss
    push cx
    mov cx, gs
    push cx
    mov cx, fs
    push cx
    mov cx, es
    push cx
    mov cx, ds
    push cx
    pushf

    mov ax, sp
    mov edi, temp_esp
    stosw

    ; ### Load the given context from asm_in_reg_ptr ###
    ; Temporaril change esp to asm_in_reg_ptr
    mov esp, REBASE(asm_in_reg_ptr)

    ; only use some general register from the given context
    popa

    ; set a new stack for bios interrupt
    mov sp, 0xc000
    ; opcode for int
    db 0xCD
asm_intnum_ptr:
    ; put the actual interrupt number here
    db 0x00
    ; ### Write current context to asm_out_reg_ptr ###
    mov esp, REBASE(asm_out_reg_ptr)
    add sp, 28

    pushf
    mov cx, ss
    push cx
    mov cx, gs
    push cx
    mov cx, fs
    push cx
    mov cx, es
    push cx
    mov cx, ds
    push cx
    pusha

    ; ### Restore current context ###
    mov esi, temp_esp
    lodsw
    mov sp, ax

    popf
    pop cx
    mov ds, cx
    pop cx
    mov es, cx
    pop cx
    mov fs, cx
    pop cx
    mov gs, cx
    pop cx
    mov ss, cx
    popa

    mov eax, cr0
    inc eax
    mov cr0, eax
    jmp CODE32:REBASE(protected_mode_32)
protected_mode_32:use32
    mov ax, DATA32
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; restore cr3
    mov cr3, ebx

    ; Turn on paging
    mov ecx, cr0
    or ecx, PG_BIT_ON
    mov cr0, ecx
    
    ; restore esp
    mov esp, edx

    sti
    popa
    ret
padding:
    db 0x0
    db 0x0
    db 0x0
asm_gdt_entries:
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
asm_gdt_ptr:
    dd 0x00000000
    dd 0x00000000
asm_idt_ptr:
    dd 0x00000000
    dd 0x00000000
asm_in_reg_ptr:
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
asm_out_reg_ptr:
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
temp_esp:
    dw 0x0000
bios32_helper_end:
