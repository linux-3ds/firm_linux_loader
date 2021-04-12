#ifndef _ARM_H
#define _ARM_H

#ifndef __ASSEMBLER__

#if __ARM_ARCH < 6
static inline void wfi(void)
{
	asm volatile (
		"mov r0, #0\n\t"
		"mcr p15, 0, r0, c7, c0, 4\n\t"::: "r0", "cc", "memory");
}
#else
static inline void wfi(void) { asm volatile ("wfi":::"memory"); }
#endif

#endif
#endif
