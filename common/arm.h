#ifndef _ARM_H
#define _ARM_H

#ifndef __ASSEMBLY__

#if __ARM_ARM < 6
static inline void wfi(void)
{
	asm volatile (
		"mov r0, #0\n\t"
		"mcr p15, 0, r0, c7, c0, 4\n\t");
}
#else
static inline void wfi(void) { asm volatile ("wfi":::"memory"); }
#endif

#endif
#endif