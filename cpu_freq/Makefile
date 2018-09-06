########################################################################        
 # Get Linux Build Enviornment:                                                  
 include env.mk      
######################################################################
# Build options
CFLAGS  += -static -W -O2
#INCLUDE += -I./zlib/lib/include 

######################################################################
# LIBRARY

LIBRARY += -lstdc++ -lpthread -lz
#STATIC_LIB :=   ./zlib/lib/lib/libz.a 
######################################################################
# Target
COBJS  	:= asv_command.o dvfsutil.o asv_test.o hwio.o
CPPOBJS += cpu_test.o  

TARGET 	:= asv_test

######################################################################
# Build
OBJS	:= $(COBJS) $(CPPOBJS)
OBJS2	:= $(COBJS2) $(CPPOBJS2)

all: $(TARGET) $(TARGET2)

$(TARGET): depend $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(STATIC_LIB) -o $@ $(LIBRARY)

#	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LIBRARY) 
clean:
	@find $(OBJTREE) -type f \
		\( -name '*.o'	-o -name $(TARGET) -o -name $(TARGET2) -o -name '*.map' \) \
		-print | xargs rm -f

#########################################################################
# Dependency
ifeq (.depend,$(wildcard .depend))
include .depend
endif

SRCS := $(COBJS:.o=.c) $(CPPOBJS:.o=.cpp)
SRCS += $(COBJS2:.o=.c) $(CPPOBJS2:.o=.cpp)
INCS := $(INCLUDE)
depend dep:
	$(CC)  -M  $(CFLAGS)   $(INCS) $(SRCS) > .depend
