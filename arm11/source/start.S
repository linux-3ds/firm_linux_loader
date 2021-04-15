#include "linux_config.h"

@ LCD Frambuffers stuff (Physical Addresses)
#define LCD_FB_PDC0           (0x10400400)
#define LCD_FB_PDC1           (0x10400500)
#define LCD_FB_A_ADDR_OFFSET  (0x68)
#define LCD_FB_FORMAT_OFFSET  (0x70)
#define LCD_FB_PDC0_FORMAT    (0x80341)
#define LCD_FB_SELECT_OFFSET  (0x78)
#define LCD_FB_STRIDE_OFFSET  (0x90)
#define LCD_FB_PDC0_STRIDE    (0x2D0)
#define LCD_FB_B_ADDR_OFFSET  (0x94)
#define FB_TOP_SIZE           (400 * 240 * 3)
#define FB_BOT_SIZE           (320 * 240 * 3)
#define FB_BASE_PA            (VRAM_BASE)
#define FB_TOP_LEFT1          (FB_BASE_PA)
#define FB_TOP_LEFT2          (FB_TOP_LEFT1  + FB_TOP_SIZE)
#define FB_TOP_RIGHT1         (FB_TOP_LEFT2  + FB_TOP_SIZE)
#define FB_TOP_RIGHT2         (FB_TOP_RIGHT1 + FB_TOP_SIZE)
#define FB_BOT_1              (FB_TOP_RIGHT2 + FB_TOP_SIZE)
#define FB_BOT_2              (FB_BOT_1      + FB_BOT_SIZE)

@ SCU
#define SCU_BASE_ADDR		0x17E00000
#define SCU_CTRL_REG		0x00
#define SCU_INV_ALL_REG		0x0C
#define SCU_CTRL_ENABLE		(1 << 0)

	.arm
	.section .text.start
	.cpu mpcore

	.global _start
_start:
	@ Disable FIQs, IRQs, imprecise aborts
	@ and enter SVC mode
	cpsid aif, #0x13

	@ Invalidate Entire Instruction Cache,
	@ also flushes the branch target cache
	mov r0, #0
	mcr p15, 0, r0, c7, c5, 0

	@ Clear and Invalidate Entire Data Cache
	mov r0, #0
	mcr p15, 0, r0, c7, c14, 0

	@ Data Synchronization Barrier
	mov r0, #0
	mcr p15, 0, r0, c7, c10, 4

	@ Disable the MMU and data cache
	@ (the MMU is already disabled)
	mrc p15, 0, r1, c1, c0, 0
	bic r1, r1, #0b101
	mcr p15, 0, r1, c1, c0, 0

	@ Clear exclusive records
	clrex

smp_start:
	@ Get the CPUID
	@ Branch to secondary boot if its not CPU0
	mrc p15, 0, r0, c0, c0, 5
	ands r0, r0, #3
	bne smp_boot

	@ Wait until ARM9 tells Linux address...
	ldr r0, =SYNC_ADDR
	mov r1, #0
	str r1, [r0]
wait_arm9:
	ldr r1, [r0]
	cmp r1, #0
	beq wait_arm9

	ldr r0, =SCU_BASE_ADDR

	@ Disable SCU
	ldr r1, [r0, #SCU_CTRL_REG]
	bic r1, r1, #SCU_CTRL_ENABLE
	str r1, [r0, #SCU_CTRL_REG]

	@ Invalidate SCU ways
	ldr r1, =0xFF
	str r1, [r0, #SCU_INV_ALL_REG]

	@ CRT initialization
	ldr r0, =__bss_start
	mov r1, #0
	ldr r2, =__bss_end
	add sp, r2, #1024
	sub r2, r2, r0
	bl memset

	bl enable_cores23

	@ Trigger CPU1
	ldr r3, =0x1FFFFFDC
	adr r4, smp_start
	str r4, [r3]

	ldr r3, =(SCU_BASE_ADDR + 0x1F00)
	ldr r4, =((2 << 16) | 1)
	str r4, [r3]

	@@@@@ Map Framebuffers @@@@@

	@@@ Top screen @@@
	ldr r0, =LCD_FB_PDC0

	@ Left eye
	ldr r1, =FB_TOP_LEFT1
	str r1, [r0, #(LCD_FB_A_ADDR_OFFSET + 0)]
	ldr r1, =FB_TOP_LEFT2
	str r1, [r0, #(LCD_FB_A_ADDR_OFFSET + 4)]

	@ Right eye
	ldr r1, =FB_TOP_RIGHT1
	str r1, [r0, #(LCD_FB_B_ADDR_OFFSET + 0)]
	ldr r1, =FB_TOP_RIGHT2
	str r1, [r0, #(LCD_FB_B_ADDR_OFFSET + 4)]

	@ Select framebuffer 0 and adjust format/stride
	mov r1, #0
	str r1, [r0, #LCD_FB_SELECT_OFFSET]
	ldr r1, =LCD_FB_PDC0_FORMAT
	str r1, [r0, #LCD_FB_FORMAT_OFFSET]
	mov r1, #LCD_FB_PDC0_STRIDE
	str r1, [r0, #LCD_FB_STRIDE_OFFSET]

	@@@ Bottom screen @@@
	ldr r0, =LCD_FB_PDC1

	ldr r1, =FB_BOT_1
	str r1, [r0, #(LCD_FB_A_ADDR_OFFSET + 0)]
	ldr r1, =FB_BOT_2
	str r1, [r0, #(LCD_FB_A_ADDR_OFFSET + 4)]

	@ Select framebuffer 0
	mov r1, #0
	str r1, [r0, #LCD_FB_SELECT_OFFSET]

	@@@@@ Jump to the kernel @@@@@

	@ Setup the registers before
	@ jumping to the kernel entry
	mov r0, #0
	ldr r1, =MACHINE_NUMBER
	ldr r2, =DTB_ADDR
	ldr lr, =ZIMAGE_ADDR

	@ Jump to the kernel!
	bx lr

smp_boot:
	@ r0 is the current CPU ID
	ldr r1, =SYNC_ADDR
	add r1, r1, r0, lsl #2
	mov r12, #0
	str r12, [r1]

wait_smpboot:
	ldr r12, [r1]
	cmp r12, #0
	wfeeq
	beq wait_smpboot

	bx r12

	.ltorg
