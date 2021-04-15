#ifndef _SCU_H
#define _SCU_H

#define SCU_BASE_ADDR		(0x17E00000)

// https://developer.arm.com/documentation/ddi0360/f/mpcore-private-memory-region/about-the-mpcore-private-memory-region/scu-specific-registers?lang=en
#define SCU_CTRL_REG		0x00
#define SCU_CPU_STAT		0x08
#define SCU_INV_ALL_REG		0x0C
#define SCU_CTRL_ENABLE		(1 << 0)

#ifndef __ASSEMBLER__

static inline unsigned int scu_get_cpu_stat(void)
{
	return *(volatile unsigned int*)(SCU_BASE_ADDR + SCU_CPU_STAT);
}

static inline void scu_set_cpu_stat(unsigned int val)
{
	*(volatile unsigned int*)(SCU_BASE_ADDR + SCU_CPU_STAT) = val;
}

#endif
#endif
