#!/bin/bash

BASEDIR="$(cd "$(dirname "$0")" && pwd)/../.."
RESULT="$BASEDIR/result"

# Toolchains for Bootloader and Linux
BL_TOOLCHAIN="$BASEDIR/characterization/crosstools/gcc-arm-none-eabi-6-2017-q2-update/bin/arm-none-eabi-"
LINUX_TOOLCHAIN="$BASEDIR/characterization/crosstools/gcc-linaro-7.2.1-2017.11-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-"

# Build PATH
BR2_DIR=$BASEDIR/buildroot
UBOOT_DIR=$BASEDIR/u-boot-2018.5
KERNEL_DIR=$BASEDIR/kernel-4.14
BIN_DIR="$BASEDIR/characterization/bin"
FILES_DIR="$BASEDIR/characterization/files"
BINARY_DIR="$BASEDIR/characterization/firmwares"
MAKE_ENV="$UBOOT_DIR/tools/mkenvimage"

# FILES
UBOOT_NSIH="$FILES_DIR/nsih_uboot.txt"
BOOT_KEY="$FILES_DIR/bootkey"
USER_KEY="$FILES_DIR/userkey"
BINGEN_EXE="$BIN_DIR/bingen"
AESCBC_EXE="$BIN_DIR/aescbc_enc"
AES_KEY=$(<$FILES_DIR/aeskey.txt)
AES_VECTOR=$(<$FILES_DIR/aesvector.txt)

#build parameter
TARGET_BL1_NAME=nxp3220

TARGET_BL2_CHIP=nxp3220
TARGET_BL2_BOARD=vtk
TARGET_BL2_PMIC=nxe1500
TARGET_BL2_NSIH=nsih_vtk_ddr3_800Mhz

TARGET_BL32_LOADADDRESS=0x5F000000
TARGET_BL32_LAUNCHADDRESS=0x5F000000

# BL1 BUILD
BL1_NSIHFILE="$FILES_DIR/nsih_bl1.txt"
BL1_AESCBC_ENC="$AESCBC_EXE -n $RESULT/bl1-nxp3220.bin -k $AES_KEY -v $AES_VECTOR -m enc -b 128"
BL1_BINGEN="$BINGEN_EXE -n $BL1_NSIHFILE -i $RESULT/bl1-nxp3220.bin
		-b $BOOT_KEY -u $USER_KEY -k bl1 -l 0xFFFF0000 -s 0xFFFF0000 -t"
BL1_BINGEN_ENC="$BINGEN_EXE -n $BL1_NSIHFILE -i $RESULT/bl1-nxp3220.bin.enc
		-b $BOOT_KEY -u $USER_KEY -k bl1 -l 0xFFFF0000 -s 0xFFFF0000 -t"
BL1_POSTCMD="$BL1_AESCBC_ENC; $BL1_BINGEN_ENC; $BL1_BINGEN"

# BL2 BUILD
BL2_DIR=$BASEDIR/firmwares/bl2-nxp3220
BL2_MAKEOPT="CHIPNAME=${TARGET_BL2_CHIP} BOARD=${TARGET_BL2_BOARD} PMIC=${TARGET_BL2_PMIC}"
BL2_NSIH="$BL2_DIR/reference-nsih/${TARGET_BL2_NSIH}.txt"

BL2_BINGEN="$BINGEN_EXE -n $BL2_NSIH -i $RESULT/bl2-${TARGET_BL2_BOARD}.bin
		-b $BOOT_KEY -u $USER_KEY -k bl2 -l 0xFFFF9000 -s 0xFFFF9000 -t"
BL2_POSTCMD="$BL2_BINGEN; \
		cp $RESULT/bl2-${TARGET_BL2_BOARD}.bin.raw $RESULT/bl2.bin.raw"

# BL32 BUILD
BL32_DIR=$BASEDIR/firmwares/bl32-nxp3220
BL32_NSIH="$BL32_DIR/reference-nsih/nsih_general.txt"

BL32_BINGEN="$BINGEN_EXE -n $BL32_NSIH -i $RESULT/bl32.bin
		-b $BOOT_KEY -u $USER_KEY -k bl32 -l ${TARGET_BL32_LOADADDRESS} -s ${TARGET_BL32_LAUNCHADDRESS} -t"
BL32_BINGEN_ENC="$BINGEN_EXE -n $BL32_NSIH -i $RESULT/bl32.bin.enc
		-b $BOOT_KEY -u $USER_KEY -k bl32 -l ${TARGET_BL32_LOADADDRESS} -s ${TARGET_BL32_LAUNCHADDRESS} -t"

BL32_AESCBC_ENC="$AESCBC_EXE -n $RESULT/bl32.bin -k $AES_KEY -v $AES_VECTOR -m enc -b 128"
BL32_POSTCMD="$BL32_AESCBC_ENC; $BL32_BINGEN_ENC; $BL32_BINGEN"


# BINGEN
UBOOT_BINGEN="$BINGEN_EXE -n $UBOOT_NSIH -i $RESULT/u-boot.bin
		-b $BOOT_KEY -u $USER_KEY -k bl33 -l 0x43C00000 -s 0x43C00000 -t"

# PARAMS
MAKE_PARAMS="$MAKE_ENV -s 16384 -o $RESULT/params.bin 
		characterization/files/nxp3220_vtk_asv_cpufreq.txt"

# Images BUILD	
MAKE_EXT4FS_EXE="$BASEDIR/characterization/bin/make_ext4fs"
MAKE_BOOTIMG="mkdir -p $RESULT/boot; \
		cp -a $RESULT/zImage $RESULT/boot; \
		cp -a $RESULT/*dtb $RESULT/boot; \
		$MAKE_EXT4FS_EXE -b 4096 -L boot -l 33554432 $RESULT/boot.img $RESULT/boot/"

MAKE_ROOTIMG="$MAKE_EXT4FS_EXE -b 4096 -L rootfs -l 1073741824 $RESULT/rootfs.img $RESULT/rootfs"

MAKE_DISKIMG="mkdir -p $RESULT/disk; \
		$MAKE_EXT4FS_EXE -b 4096 -L disk -l 1073741824 $RESULT/disk.img $RESULT/disk"


MAKE_DISK="$MAKE_DISK_IMG -f $PARTMAP -s 2 -r 0"

# Build Targets
BUILD_IMAGES=(
	"MACHINE= nxp3220",
	"ARCH  	= arm",
	"TOOL	= $LINUX_TOOLCHAIN",
	"RESULT = $RESULT",
	"bl1   	=
		OUTPUT	: $BASEDIR/firmwares/binary/bl1-nxp3220.bin*,
		POSTCMD : $BL1_POSTCMD",
	"bl2   	=
		PATH  	: $BL2_DIR,
		TOOL  	: $BL_TOOLCHAIN,
		OPTION	: $BL2_MAKEOPT,
		OUTPUT	: out/bl2-${TARGET_BL2_BOARD}.bin*,
		POSTCMD : $BL2_POSTCMD,
		JOBS  	: 1", # must be 1
	"bl32   	=
		PATH  	: $BL32_DIR,
		TOOL  	: $BL_TOOLCHAIN,
		OUTPUT	: out/bl32.bin*,
		POSTCMD	: $BL32_POSTCMD,
		JOBS  	: 1", # must be 1
	"uboot 	=
		PATH  	: $UBOOT_DIR,
		CONFIG	: nxp3220_vtk_defconfig,
		OUTPUT	: u-boot.bin,
		POSTCMD	: $UBOOT_BINGEN"
	"br2   	=
		PATH  	: $BR2_DIR,
		CONFIG	: nxp3220_asv_cpu_defconfig,
		OUTPUT	: output/target,
		COPY  	: rootfs",
	"kernel	=
		PATH  	: $KERNEL_DIR,
		CONFIG	: nxp3220_asv_cpufreq_defconfig,
		IMAGE 	: zImage,
		OUTPUT	: arch/arm/boot/zImage",
	"dtb   	=
		PATH  	: $KERNEL_DIR,
		IMAGE 	: nxp3220-asv-cpufreq.dtb,
		OUTPUT	: arch/arm/boot/dts/nxp3220-asv-cpufreq.dtb",
	"bootimg =
		POSTCMD	: $MAKE_BOOTIMG",
	"params =
		POSTCMD : $MAKE_PARAMS",
	"root	=
		POSTCMD : $MAKE_ROOTIMG",
	"disk	=
		POSTCMD : $MAKE_DISKIMG",
)
