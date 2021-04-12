#ifndef _CFG11_H
#define _CFG11_H

// Via: https://www.3dbrew.org/wiki/CONFIG11_Registers

#define CFG11_SOCINFO	(0x10140FFC)

#ifndef __ASSEMBLER__

#include <stdbool.h>

static inline short get_cfg11_socinfo(void)
{
	return *(short*)CFG11_SOCINFO & 0x7;
}

static inline bool is_lgr1(void) { return get_cfg11_socinfo() == 0x3; }
static inline bool is_lgr2(void) { return get_cfg11_socinfo() == 0x7; }
static inline bool is_lgr(void) { return is_lgr1() || is_lgr2(); }

#endif
#endif
