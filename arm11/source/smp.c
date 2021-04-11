#include "arm.h"
#include "cfg11.h"
#include "gic.h"
#include "pdn.h"

static void enable_irqs(void)
{
	gic_enable_control();
	gic_disable_all();
	gic_clear_interrupt(88);
	gic_set_prio(88, 0);
	gic_set_target(88, 0);
	gic_enable_interrupt(88);
}

static void enable_fcram_l2(void)
{
	set_pdn_lgr_cnt(0x01 | (is_lgr2() ? 0x100 : 0));
}

static void wait_cycles(unsigned short cycles)
{
	asm ("1:\n\t"
	     "subs %0, %0, #2\n\t"
	     "yield\n\t"
	     "bhi 1b\n\t"
	     :: "r"(cycles) : "cc");
}

static void upclock(void)
{
	set_pdn_lgr_socmode(is_lgr2() ? 0x05 : 0x03);

	// Loop until the ACK bit is set.
	do {
		wfi();
	} while (!(get_pdn_lgr_socmode() & (1 << 15)));

	// Write it back to ACK.
	set_pdn_lgr_socmode(get_pdn_lgr_socmode());
}

static void set_socmode(void)
{
	enable_irqs();
	enable_fcram_l2();
	wait_cycles(403);
	upclock();
	gic_clear_interrupt(88);
}

void enable_cores23(void)
{
	if (!is_lgr())
		return;

	set_socmode();
}
