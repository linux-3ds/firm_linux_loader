#ifndef _PDN_H
#define _PDN_H

// Via: https://www.3dbrew.org/wiki/PDN_Registers

#define PDN_LGR_SOCMODE	0x10141300
#define PDN_LGR_CNT	0x10141304
#define PDN_LGR_CPU_CNT	0x10141310

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

static inline unsigned char get_pdn_lgr_cpu_cnt(int core)
{
	return *(volatile unsigned char*)(PDN_LGR_CPU_CNT + core);
}

static inline void set_pdn_lgr_cpu_cnt(int core, char val)
{
	*(volatile unsigned char*)(PDN_LGR_CPU_CNT + core) = val;
}

#endif
#endif
