#
# Make options
#
#	CROSS_COMPILE	: define cross compiler (default arm-linux-gnueabihf-gcc)
#	DEBUG=y		: use debug build
#	DESTDIR=<path>	: set library path
#	EXT_FLAGS=""
#	EXT_LDFLAGS=""
#
ifndef TOPDIR
TOPDIR := ..
endif
include $(TOPDIR)/env.mk

CROSS_COMPILE 	:= arm-linux-gnueabihf
DESTDIR 	:= ../result
CFLAGS  	:=$(EXT_CFLAGS)
CPPFLAGS	:=$(EXT_CPPFLAGS)
LDFLAGS 	:=$(EXT_LDFLAGS)

CFLAGS 	+= -I$(DESTDIR)/include/json-c
LDFLAGS += -L$(DESTDIR)/lib
LDFLAGS += -static

LIBS	+= -ljson-c
LIBS	+= -lpthread

CPPFLAGS += $(CFLAGS)
CPPFLAGS += -std=c++11

CPPOBJS	= main.o task.o
OBJS	= $(CPPOBJS)

TARGET	= devfreq

all : $(TARGET)

$(TARGET) : depend $(OBJS)
	$(CPP) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
ifneq ($(DEBUG),y)
	$(STRIP) $@
endif
	mkdir -p $(abspath $(DESTDIR))/bin
	cp $@ $(abspath $(DESTDIR))/bin

.PHONY: clean distclean

clean :
	rm -rf $(OBJS) $(TARGET) core .depend

distclean: clean

ifeq (.depend,$(wildcard .depend))
include .depend
endif

depend dep:
	$(CPP) -M $(CPPFLAGS) $(CPPOBJS:.o=.cpp) > .depend

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<

%.o: %.cpp
	$(CPP) $(CPPFLAGS) $(INCLUDE) -c -o $@ $<
