#include "arm.h"
#include "cache.h"
#include "cfg11.h"
#include "common.h"
#include "draw.h"
#include "fs.h"
#include "hid.h"
#include "i2c.h"
#include "linux_config.h"

__attribute__((noreturn))
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

int main(int argc, char *argv[])
{
	const char *dtb_filename;

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

	dtb_filename = is_lgr() ? KTR_DTB_FILENAME : CTR_DTB_FILENAME;
	if (!load_file(dtb_filename, PARAMS_ADDR)) {
		Debug("Failed to load %s", dtb_filename);
		goto error;
	}

	if (!load_file(ARM9LINUXFW_FILENAME, ARM9LINUXFW_ADDR)) {
		Debug("Failed to load arm9linuxfw");
		goto error;
	}

	flushCaches();

	DeinitFS();

	/* Make the ARM11 jump to the Linux payload */
	*(vu32 *)SYNC_ADDR = 1;

	flushCaches();

	/* Jump to the arm9linuxfw */
	((void (*)(void))ARM9LINUXFW_ADDR)();

error:
	DeinitFS();
	wait_any_key_poweroff();
}
