#ifndef _PDN_H
#define _PDN_H

// Via: https://www.3dbrew.org/wiki/PDN_Registers

#define PDN_LGR_SOCMODE	0x10141300
#define PDN_LGR_CNT	0x10141304

#ifndef __ASSEMBLER__

static inline short get_pdn_lgr_socmode(void)
{
	return *(volatile short*)PDN_LGR_SOCMODE;
}

static inline void set_pdn_lgr_socmode(short mode)
{
	*(volatile short*)PDN_LGR_SOCMODE = mode;
}

static inline void set_pdn_lgr_cnt(short x)
{
	*(volatile short*)PDN_LGR_CNT = x;
}

#endif
#endif
