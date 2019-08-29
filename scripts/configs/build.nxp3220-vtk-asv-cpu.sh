#!/bin/bash

TARGET_RESULT=result-nxp3220-asv

# For BL1
export BASEDIR=`realpath "$(cd "$(dirname "$0")" && pwd)/../.."`
TARGET_BL1_DIR=${BASEDIR}/firmwares/bl1-nxp3220

# For BL2
TARGET_BL2_CHIP=nxp3220
TARGET_BL2_BOARD=vtk
TARGET_BL2_PMIC=nxe1500
TARGET_BL2_NSIH=nsih_vtk_ddr3_800Mhz.txt

# For BL32
TARGET_BL32_LOADADDR=0x5F000000

# For Kernel
TARGET_KERNEL_DEFCONFIG=nxp3220_asv_cpufreq_defconfig
TARGET_KERNEL_DTB=nxp3220-asv-cpufreq
TARGET_KERNEL_IMAGE=zImage

# For U-boot
TARGET_UBOOT_DEFCONFIG=nxp3220_vtk_defconfig

# For Buildroot
TARGET_BR2_DEFCONFIG=nxp3220_asv_cpu_defconfig

# For Filesystem image
TARGET_IMAGE_TYPE="ext4"
TARGET_BOOT_IMAGE_SIZE=32M
TARGET_ROOT_IMAGE_SIZE=1G
TARGET_DATA_IMAGE_SIZE=6G

# build script
BUILD_CONFIG_DIR="$(cd "$(dirname "$0")" && pwd)"/configs
source $BUILD_CONFIG_DIR/build.common.sh

