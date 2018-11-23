#!/bin/bash

BASEDIR="$(cd "$(dirname "$0")" && pwd)/../.."
RESULT="$BASEDIR/result"

# Toolchains for Bootloader and Linux
LINUX_TOOLCHAIN="$BASEDIR/characterization/crosstools/gcc-linaro-7.2.1-2017.11-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-"
#LINUX_TOOLCHAIN="$BASEDIR/characteriztion/tools/gcc-linaro-7.2.1-2017.11-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-"

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

# BINGEN
UBOOT_BINGEN="$BINGEN_EXE -n $UBOOT_NSIH -i $RESULT/u-boot.bin
		-b $BOOT_KEY -u $USER_KEY -k bl33 -l 0x43C00000 -s 0x43C00000 -t"

		# PARAMS
MAKE_PARAMS="$MAKE_ENV -s 16384 -o $RESULT/params.bin 
		characterization/files/nxp3220_vtk_asv_busfreq.txt"

# Images BUILD	
MAKE_EXT4FS_EXE="$BASEDIR/characterization/bin/make_ext4fs"
MAKE_BOOTIMG="mkdir -p $RESULT/boot; \
		cp -a $RESULT/zImage $RESULT/boot; \
		cp -a $RESULT/*dtb $RESULT/boot; \
		$MAKE_EXT4FS_EXE -b 4096 -L boot -l 33554432 $RESULT/boot.img $RESULT/boot/"

MAKE_ROOTIMG="mkdir -p $RESULT/root; \
		$MAKE_EXT4FS_EXE -b 4096 -L root -l 1073741824 $RESULT/root.img $RESULT/root/"

# Build Targets
BUILD_IMAGES=(
	"MACHINE= nxp3220",
	"ARCH  	= arm",
	"TOOL	= $LINUX_TOOLCHAIN",
	"RESULT = $RESULT",
	"bl1   	=
		OUTPUT	: $BINARY_DIR/*",
	"uboot 	=
		PATH  	: $UBOOT_DIR,
		CONFIG	: nxp3220_vtk_defconfig,
		OUTPUT	: u-boot.bin,
		POSTCMD	: $UBOOT_BINGEN"
	"br2   	=
		PATH  	: $BR2_DIR,
		CONFIG	: nxp3220_asv_bus_defconfig,
		OUTPUT	: output/target,
		COPY  	: rootfs",
	"kernel	=
		PATH  	: $KERNEL_DIR,
		CONFIG	: nxp3220_vtk_asv_busfreq_defconfig,
		IMAGE 	: zImage,
		OUTPUT	: arch/arm/boot/zImage",
	"dtb   	=
		PATH  	: $KERNEL_DIR,
		IMAGE 	: nxp3220-vtk-asv-devfs.dtb,
		OUTPUT	: arch/arm/boot/dts/nxp3220-vtk-asv-devfs.dtb",
	"bootimg =
		POSTCMD	: $MAKE_BOOTIMG",
	"params =
		POSTCMD : $MAKE_PARAMS",
	"root	=
		POSTCMD :$MAKE_ROOTIMG",
)
