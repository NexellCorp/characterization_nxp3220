#!/bin/bash
export BASEDIR=`realpath "$(cd "$(dirname "$0")" && pwd)/../.."`
export RESULT="${BASEDIR}/out/result"

if [[ ! -z $TARGET_RESULT ]]; then
	export RESULT="${BASEDIR}/out/${TARGET_RESULT}"
fi

export BL_TOOLCHAIN="${BASEDIR}/characterization/crosstools/gcc-arm-none-eabi-6-2017-q2-update/bin/arm-none-eabi-"
export LINUX_TOOLCHAIN="${BASEDIR}/characterization/crosstools/gcc-linaro-7.2.1-2017.11-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-"

if [[ -z $TARGET_BL1_DIR ]]; then
	export BL1_DIR=${BASEDIR}/firmwares/binary
else
	export BL1_DIR=${TARGET_BL1_DIR}
fi

export BL2_DIR=${BASEDIR}/firmwares/bl2-nxp3220
export BL32_DIR=${BASEDIR}/firmwares/bl32-nxp3220
export UBOOT_DIR=${BASEDIR}/u-boot-2018.5
export KERNEL_DIR=${BASEDIR}/kernel-4.14
export BR2_DIR=${BASEDIR}/buildroot

export TOOL_BINGEN="${BASEDIR}/characterization/bin/bingen"
export TOOL_BINENC="${BASEDIR}/characterization/bin/aescbc_enc"
export TOOL_BINECC="${BASEDIR}/characterization/bin/nandbingen"
export TOOL_MKPARAM="${BASEDIR}/characterization/scripts/mk_bootparam.sh"
export TOOL_MKUBIFS="${BASEDIR}/characterization/scripts/mk_ubifs.sh"
export TOOL_MKEXT4="make_ext4fs"

export BL1_BIN="bl1-nxp3220.bin"
export BL1_LOADADDR=0xFFFF0000
export BL1_NSIH="${BASEDIR}/characterization/files/nsih_bl1.txt"
export BL1_BOOTKEY="${BASEDIR}/characterization/files/bootkey"
export BL1_USERKEY="${BASEDIR}/characterization/files/userkey"
export BL1_AESKEY="${BASEDIR}/characterization/files/aeskey.txt"
export BL1_VECTOR="${BASEDIR}/characterization/files/aesvector.txt"

export BL2_BIN="bl2-${TARGET_BL2_BOARD}.bin"
export BL2_LOADADDR=0xFFFF9000
export BL2_NSIH="${BL2_DIR}/reference-nsih/$TARGET_BL2_NSIH"
export BL2_BOOTKEY="${BASEDIR}/characterization/files/bootkey"
export BL2_USERKEY="${BASEDIR}/characterization/files/userkey"
export BL2_CHIP=${TARGET_BL2_CHIP}
export BL2_BOARD=${TARGET_BL2_BOARD}
export BL2_PMIC=${TARGET_BL2_PMIC}

export BL32_BIN="bl32.bin"
export BL32_LOADADDR=${TARGET_BL32_LOADADDR}
export BL32_NSIH="${BL32_DIR}/reference-nsih/nsih_general.txt"
export BL32_BOOTKEY="${BASEDIR}/characterization/files/bootkey"
export BL32_USERKEY="${BASEDIR}/characterization/files/userkey"
export BL32_AESKEY="${BASEDIR}/characterization/files/aeskey.txt"
export BL32_VECTOR="${BASEDIR}/characterization/files/aesvector.txt"

export UBOOT_BIN="u-boot.bin"
export UBOOT_LOADADDR=0x43c00000
export UBOOT_NSIH="${BASEDIR}/characterization/files/nsih_uboot.txt"
export UBOOT_DEFCONFIG=${TARGET_UBOOT_DEFCONFIG}
export UBOOT_BOOTKEY="${BASEDIR}/characterization/files/bootkey"
export UBOOT_USERKEY="${BASEDIR}/characterization/files/userkey"
export UBOOT_LOGO_BMP="${BASEDIR}/characterization/files/logo.bmp"

export KERNEL_DEFCONFIG=${TARGET_KERNEL_DEFCONFIG}
export KERNEL_BIN=${TARGET_KERNEL_IMAGE}
export DTB_BIN=${TARGET_KERNEL_DTB}.dtb

export BR2_DEFCONFIG=${TARGET_BR2_DEFCONFIG}

export IMAGE_TYPE=${TARGET_IMAGE_TYPE}
export IMAGE_BOOT_SIZE=${TARGET_BOOT_IMAGE_SIZE}
export IMAGE_ROOT_SIZE=${TARGET_ROOT_IMAGE_SIZE}
export IMAGE_DATA_SIZE=${TARGET_DATA_IMAGE_SIZE}

export TOOL_FILES=(
	"${BASEDIR}/characterization/scripts/partmap_fastboot.sh"
	"${BASEDIR}/characterization/scripts/partmap_diskimg.sh"
	"${BASEDIR}/characterization/scripts/usb-down.sh"
	"${BASEDIR}/characterization/scripts/configs/udown.bootloader.sh"
	"${BASEDIR}/characterization/bin/linux-usbdownloader"
	"${BASEDIR}/characterization/bin/simg2dev"
	"${BASEDIR}/characterization/files/partmap_*.txt"
)

