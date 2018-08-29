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

TARGET 	:=nx_vpu_test
CONFIG 	:=$(TARGET)/Makefile

all : $(TARGET)

CONFIGURE :
ifneq ($(shell test -e $(CONFIG) && echo -n yes),yes)
	# set preifx for install "DESTDIR/lib, ...
	cd $(shell pwd)/$(TARGET) && \
	./autogen.sh && \
	./configure --host=$(CROSS_COMPILE) --build=i686-linux --prefix=$(abspath $(DESTDIR))
endif

$(TARGET) : CONFIGURE
	cd $(shell pwd)/$(TARGET) && \
	make DESTDIR= && \
	make install DESTDIR= && \
	cp -av $(shell pwd)/$(TARGET)/src/data $(abspath $(DESTDIR))

.PHONY: clean distclean

clean :
	make clean -C $(TARGET)

distclean :
	make distclean -C $(TARGET)


