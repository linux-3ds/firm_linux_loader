#ifndef _GIC_H
#define _GIC_H

// These are only accessible from arm11 core...

#define MPCORE_PRIV_REG_BASE	(0x17E00000)
#define GIC_CPU_REGS_BASE	(MPCORE_PRIV_REG_BASE + 0x0100)
#define GIC_DIST_REGS_BASE	(MPCORE_PRIV_REG_BASE + 0x1000)

/*
 * Names/values via:
 * https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/include/linux/irqchip/arm-gic.h
 */

#define GIC_CPU_CTRL		(GIC_CPU_REGS_BASE + 0x00)
#define GIC_DIST_ENABLE_SET	(GIC_DIST_REGS_BASE + 0x100)
#define GIC_DIST_ENABLE_CLEAR	(GIC_DIST_REGS_BASE + 0x180)
#define GIC_DIST_PENDING_CLEAR	(GIC_DIST_REGS_BASE + 0x280)
#define GIC_DIST_PRI		(GIC_DIST_REGS_BASE + 0x400)
#define GIC_DIST_TARGET		(GIC_DIST_REGS_BASE + 0x800)

#define BIT(n) (1 << (n))

#ifndef __ASSEMBLER__

static inline void gic_enable_control(void) {
	*(volatile int*)GIC_CPU_CTRL = 1;
}

static inline void gic_disable_all(void)
{
	volatile int *gic_enable_clear = (volatile int*)GIC_DIST_ENABLE_CLEAR;
	gic_enable_clear[0] = -1;
	gic_enable_clear[1] = -1;
	gic_enable_clear[2] = -1;
	gic_enable_clear[3] = -1;
}

static inline void gic_x_interrupt(volatile int* addr, int id)
{
	addr[0] = BIT(id);
	addr[1] = BIT(id % 32);
	addr[2] = BIT(id % 32);
	addr[3] = BIT(id % 32);
}

static inline void gic_clear_interrupt(int id)
{
	gic_x_interrupt((volatile int*)GIC_DIST_PENDING_CLEAR, id);
}

static inline void gic_set_prio(int id, int prio)
{
	*(volatile int*)(GIC_DIST_PRI + id) = prio;
}

// https://developer.arm.com/documentation/ddi0360/f/mpcore-distributed-interrupt-controller/interrupt-distributor-registers/interrupt-cpu-targets-registers--0x800-0x8fc?lang=en
static inline void gic_set_target(int id, int core)
{
	*(volatile int*)(GIC_DIST_TARGET + id) = BIT(core);
}

static inline void gic_enable_interrupt(int id)
{
	gic_x_interrupt((volatile int*)GIC_DIST_ENABLE_SET, id);
}

#endif
#endif
