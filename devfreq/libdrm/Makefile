#
# Make options
#
#	CROSS_COMPILE	: define cross compiler (default arm-linux-gnueabihf-gcc)
#	DEBUG=y		: use debug build
#	DESTDIR=<path>	: set library path
#
ifndef TOPDIR
TOPDIR := ..
endif
include $(TOPDIR)/env.mk

CROSS_COMPILE 	:= arm-linux-gnueabihf
DESTDIR 	:= ../result

TARGET 	:=libdrm-2.4.91
CONFIG 	:=$(TARGET)/Makefile

all : $(TARGET)

CONFIGURE :
ifneq ($(shell test -e $(CONFIG) && echo -n yes),yes)
	# set preifx for install "DESTDIR/lib, ...
	cd $(shell pwd)/$(TARGET) && \
	./autogen.sh && \
	./configure --host=$(CROSS_COMPILE) --build=i686-linux --prefix= \
		--sysconfdir=/etc --localstatedir=/var --disable-gtk-doc \
		--disable-gtk-doc-html --disable-doc --disable-docs \
		--disable-documentation --with-xmlto=no --with-fop=no \
		--disable-dependency-tracking --enable-ipv6 --disable-nls \
		--disable-static --enable-shared --disable-cairo-tests \
		--disable-manpages --disable-intel --disable-radeon \
		--disable-amdgpu --disable-nouveau --disable-vmwgfx \
		--disable-omap-experimental-api --disable-etnaviv-experimental-api \
		--disable-exynos-experimental-api --disable-freedreno \
		--disable-tegra-experimental-api --disable-vc4 --disable-udev \
		--disable-valgrind	
endif

$(TARGET) : CONFIGURE
	cd $(shell pwd)/$(TARGET) && \
	make && \
	make install DESTDIR=$(abspath $(DESTDIR))

.PHONY: clean distclean

clean :
	make clean -C $(TARGET)

distclean :
	make distclean -C $(TARGET)


