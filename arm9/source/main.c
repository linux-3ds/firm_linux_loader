#include "common.h"
#include "draw.h"
#include "hid.h"
#include "fs.h"
#include "i2c.h"
#include "cache.h"
#include "linux_config.h"

static void mcu_poweroff()
{
	i2cWriteRegister(I2C_DEV_MCU, 0x20, 1 << 0);
	while (1) ;
}

static void wait_any_key_pressed()
{
	while (~HID_PAD & BUTTON_NONE)
		;

	while (!(~HID_PAD & BUTTON_NONE))
		;
}

static void wait_any_key_poweroff()
{
	Debug("Press any key to poweroff.");
	wait_any_key_pressed();
	mcu_poweroff();
}

static int load_file(const char *filename, uint32_t addr)
{
	size_t size;

	Debug("Loading %s...", filename);

	if (!FileOpen(filename)) {
		DebugColor(COLOR_RED, "Loading of %s failed!", filename);
		return 0;
	}

	size = FileRead((void *)addr, ~0UL, 0);

	Debug("File %s loaded:", filename);
	Debug("    size: %d B", size);

	FileClose();

	return 1;
}

static bool is_ktr(void)
{
	return (*(int*)SOCINFO & 2) != 0;
}

int main(int argc, char *argv[])
{
	const char *dtb_filename;
	int has_arm9linuxfw = 0;

	InitScreenFbs(argc, argv);
	ClearScreenFull(true, true);
	DebugClear();

	Debug("-- FIRM Linux loader by xerpi --");

	if (InitFS()) {
		Debug("Initializing SD card... success");
	} else {
		Debug("Initializing SD card... failed");
		wait_any_key_poweroff();
	}

	if (!load_file(LINUXIMAGE_FILENAME, ZIMAGE_ADDR)) {
		Debug("Failed to load " LINUXIMAGE_FILENAME);
		goto error;
	}

	dtb_filename = is_ktr() ? KTR_DTB_FILENAME : CTR_DTB_FILENAME;
	if (!load_file(dtb_filename, PARAMS_ADDR)) {
		Debug("Failed to load %s", dtb_filename);
		goto error;
	}

	if (!load_file(ARM9LINUXFW_FILENAME, ARM9LINUXFW_ADDR)) {
		Debug("Continuing without an arm9linuxfw...");
	} else {
		has_arm9linuxfw = 1;
	}

	flushCaches();

	DeinitFS();

	/* Make the ARM11 jump to the Linux payload */
	*(vu32 *)SYNC_ADDR = 1;

	flushCaches();

	/* Jump to the arm9linuxfw (if any) */
	if (has_arm9linuxfw) {
		((void (*)(void))ARM9LINUXFW_ADDR)();
	} else {
		/*
		 * If we are here, there's no arm9linuxfw, so we just
		 * busy loop in WFI state to save battery.
		 */
		while (1) {
			/* Enter wait-for-interrupt state */
			__asm__(
				"mov r0, #0\n\t"
				"mcr p15, 0, r0, c7, c0, 4\n\t"
			);
		}
	}

error:
	DeinitFS();
	wait_any_key_poweroff();
}
