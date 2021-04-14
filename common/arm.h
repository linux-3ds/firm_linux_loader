#ifndef _ARM_H
#define _ARM_H

#ifndef __ASSEMBLER__

#if __ARM_ARCH < 6
static inline void wfi(void)
{
	asm (
		"mov r0, #0\n\t"
		"mcr p15, 0, r0, c7, c0, 4\n\t"::: "r0", "cc", "memory");
}
#else
static inline void wfi(void) { asm ("wfi":::"memory"); }
#endif

static inline void wfe(void) { asm ("wfe":::"memory"); }

static inline void cpsid_aif(void) { asm ("cpsid aif":::"memory"); }

static inline unsigned int cpuid(void)
{
	unsigned int id;
	asm ("mrc p15, 0, %0, c0, c0, 5" : "=r"(id));
	return id & 3;
}

#endif
#endif
