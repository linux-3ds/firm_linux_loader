#include "arm.h"
#include "cfg11.h"
#include "gic.h"
#include "linux_config.h"
#include "pdn.h"
#include "scu.h"

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

static void set_clock(short socmode)
{
	set_pdn_lgr_socmode(socmode);

	// Loop until the ACK bit is set.
	do {
		wfi();
	} while (!(get_pdn_lgr_socmode() & (1 << 15)));

	// Write it back to ACK.
	set_pdn_lgr_socmode(get_pdn_lgr_socmode());
}

static void upclock(void) { set_clock(is_lgr2() ? 0x05 : 0x03); }
static void downclock(void) { set_clock(is_lgr2() ? 0x01 : 0x02); }

static void set_socmode(void)
{
	enable_irqs();
	enable_fcram_l2();
	wait_cycles(403);
	upclock();
	gic_clear_interrupt(88);
}

__attribute__((noreturn))
void smp_boot(unsigned int core);

__attribute__((naked))
static void core23_entry(void)
{
	unsigned int ack, core;

	cpsid_aif();
	gic_enable_control();

	core = cpuid();
	set_pdn_lgr_cpu_cnt(core == 3 ? 3 : 2, 0x01);

	do {
		wfi();
		ack = gic_get_intack();
		gic_set_eoi(ack);
	} while (ack != core);

	smp_boot(core);
}

static void setup_overlays(void)
{
	bootrom_overlay_enable();
	set_bootrom_overlay_start(core23_entry);
	if (!(get_pdn_lgr_cpu_cnt(2) & 0x10))
		set_pdn_lgr_cpu_cnt(2, 0x02 | 0x01);
	if (!(get_pdn_lgr_cpu_cnt(3) & 0x10))
		set_pdn_lgr_cpu_cnt(3, 0x02 | 0x01);
	while ((get_pdn_lgr_cpu_cnt(2) & (0x10 | 0x02)) != 0x10);
	while ((get_pdn_lgr_cpu_cnt(3) & (0x10 | 0x02)) != 0x10);
	bootrom_overlay_disable();
}

static void online_cores23(void)
{
	scu_set_cpu_stat(scu_get_cpu_stat() & 0x0F);
	downclock();
	gic_clear_interrupt(88);
	setup_overlays();
	upclock();
	gic_clear_interrupt(88);
	gic_send_swi(2, 2);
	gic_send_swi(3, 3);
}

void enable_cores23(void)
{
	if (!is_lgr())
		return;

	set_socmode();
	online_cores23();
}
