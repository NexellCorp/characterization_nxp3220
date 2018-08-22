CROSS_COMPILE 	?= arm-linux-gnueabihf

DESTDIR	 	?= $(shell pwd)/result
ACFLAGS  	?=
ACPPFLAGS  	?=
ALDFLAGS 	?=

################################################################################
EXT_CFLAGS  	:=$(ACFLAGS)
EXT_CPPFLAGS  	:=$(ACPPFLAGS)
EXT_LDFLAGS 	:=$(ALDFLAGS)

EXT_CFLAGS  	+=-Wall -O2 -Wextra -Wcast-align -Wno-unused-parameter -Wshadow \
		-Wwrite-strings -Wcast-qual -fno-strict-aliasing -fstrict-overflow \
           	-fsigned-char -fno-omit-frame-pointer -fno-optimize-sibling-calls
EXT_CPPFLAGS  	+=-Wnon-virtual-dtor

EXT_CFLAGS  	+=-I$(DESTDIR)/include
EXT_LDFLAGS 	+=-L$(DESTDIR)/lib

################################################################################
ifneq ($(CROSS_COMPILE),)
CC              := $(CROSS_COMPILE)-gcc
CPP             := $(CROSS_COMPILE)-g++
CXX             := $(CROSS_COMPILE)-g++
AR              := $(CROSS_COMPILE)-ar
LD              := $(CROSS_COMPILE)-ld
NM              := $(CROSS_COMPILE)-nm
RANLIB  	:= $(CROSS_COMPILE)-ranlib
OBJCOPY         := $(CROSS_COMPILE)-objcopy
STRIP           := $(CROSS_COMPILE)-strip
endif

