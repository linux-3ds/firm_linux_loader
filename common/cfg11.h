#ifndef _CFG11_H
#define _CFG11_H

// Via: https://www.3dbrew.org/wiki/CONFIG11_Registers

#define CFG11_SOCINFO			(0x10140FFC)
#define CFG11_BOOTROM_OVERLAY_CNT	(0x10140420)
#define CFG11_BOOTROM_OVERLAY_VAL	(0x10140424)

#ifndef __ASSEMBLER__

#include <stdbool.h>
#include <stdint.h>

static inline short get_cfg11_socinfo(void)
{
	return *(short*)CFG11_SOCINFO & 0x7;
}

static inline bool is_lgr1(void) { return get_cfg11_socinfo() == 0x3; }
static inline bool is_lgr2(void) { return get_cfg11_socinfo() == 0x7; }
static inline bool is_lgr(void) { return is_lgr1() || is_lgr2(); }

static inline void bootrom_overlay_enable(void)
{
	*(volatile char*)CFG11_BOOTROM_OVERLAY_CNT = 1;
}

static inline void bootrom_overlay_disable(void)
{
	*(volatile char*)CFG11_BOOTROM_OVERLAY_CNT = 0;
}

static inline void set_bootrom_overlay_start(void (*start)(void))
{
	*(volatile uintptr_t*)CFG11_BOOTROM_OVERLAY_VAL = (uintptr_t)start;
}

#endif
#endif
