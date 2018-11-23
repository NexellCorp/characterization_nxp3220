#!/bin/bash

BASEDIR="$(cd "$(dirname "$0")" && pwd)/../.."
RESULT="$BASEDIR/result"
TARGET=artik310s

DN_IMAGES=(
	"TARGET: $TARGET"
	"BOARD : vtk"
	"bl1   : -b $RESULT/bl1-nxp3220.bin.raw  -a 0xFFFF0000 -j 0xFFFF0000"
	"bl2   : -b $RESULT/bl2-vtk.bin.raw -a 0xFFFF9000 -j 0xFFFF9000"
#	"sss   : -b $RESULT/sss.raw -a 0x23D18000 -j 0x23D18000"
	"bl32  : -b $RESULT/bl32.bin.raw -a 0x5F000000 -j 0x5F000000"
	"uboot : -b $RESULT/u-boot.bin.raw -a 0x43C00000 -j 0x43C00000" 
)

DN_ENC_IMAGES=(
	"TARGET: $TARGET"
	"BOARD : vtk"
	"bl1   : -b $RESULT/bl1-nxp3220.bin.enc.raw  -a 0xFFFF0000 -j 0xFFFF0000"
	"bl2   : -b $RESULT/bl2-vtk.bin.raw -a 0xFFFF9000 -j 0xFFFF9000"
#	"sss   : -b $RESULT/sss.raw -a 0x23D18000 -j 0x23D18000"
	"bl32  : -b $RESULT/bl32.bin.raw -a 0x5F000000 -j 0x5F000000"
	"uboot : -b $RESULT/u-boot.bin.raw -a 0x43C00000 -j 0x43C00000" 
)
