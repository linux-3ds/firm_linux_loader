/* Linux settings */
#define ZIMAGE_ADDR      (0x20008000)
#define ZRELADDR         (0x20008000)
#define PARAMS_ADDR      (0x20000100)
#define INITRD_ADDR      (0x20000000)
#define MACHINE_NUMBER   (0xFFFFFFFF)
#define ARM9LINUXFW_ADDR (0x08080000)
#define SYNC_ADDR        (0x1FFFFFF8)

#define LINUXIMAGE_FILENAME  "linux/zImage"
#define DTB_FILENAME         "linux/nintendo3ds_ctr.dtb"
#define ARM9LINUXFW_FILENAME "linux/arm9linuxfw.bin"

/* 3DS memory layout */
#define VRAM_BASE     (0x18000000)
#define VRAM_SIZE     (0x00600000)
#define AXI_WRAM_BASE (0x1FF80000)
#define AXI_WRAM_SIZE (0x00080000)
#define FCRAM_BASE    (0x20000000)
#define FCRAM_SIZE    (0x08000000)
