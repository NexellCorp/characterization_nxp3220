include env.mk

################################################################################
SRC_DIRS := main
LIB_DIRS := json
################################################################################

all: lib src
PHONY := all clean distclean src lib

lib:
	@for dir in $(LIB_DIRS); do \
		echo "========================================================\n" && \
		make DESTDIR=$(DESTDIR) DEBUG=$(DEBUG) \
                CROSS_COMPILE=$(CROSS_COMPILE) \
		EXT_CFLAGS="$(EXT_CFLAGS)" EXT_CPPFLAGS="$(EXT_CPPFLAGS)" \
		EXT_LDFLAGS="$(EXT_LDFLAGS)" \
		-C $$dir || exit $?; \
        done

src:
	@for dir in $(SRC_DIRS); do \
		echo "========================================================\n" && \
                make DESTDIR=$(DESTDIR) DEBUG=$(DEBUG) \
                CROSS_COMPILE=$(CROSS_COMPILE) \
		EXT_CFLAGS="$(EXT_CFLAGS)" EXT_CPPFLAGS="$(EXT_CPPFLAGS)" \
		EXT_LDFLAGS="$(EXT_LDFLAGS)" \
		-C $$dir || exit $?; \
        done

clean:
	@for dir in $(SRC_DIRS); do \
                make -C $$dir clean || exit $?; \
        done
	@for dir in $(LIB_DIRS); do \
                make -C $$dir clean || exit $?; \
        done

distclean:
	@for dir in $(SRC_DIRS); do \
                make -C $$dir distclean || exit $?; \
        done
	@for dir in $(LIB_DIRS); do \
                make -C $$dir distclean || exit $?; \
        done
	rm -rf $(DESTDIR)

help:
	@echo 'help'
	@echo  '  make for make all - Build all directory'
	@echo  '  clean             - Remove most generated files'
	@echo  '  distclean         - Rmeove all generated files'
	@echo  '  src               - Build application directory'
	@echo  '  lib               - Build library directory'
	@echo  ''
	@echo  '  make CROSS_COMPILE=arm-xxx '
	@echo  '       Set CROSS_COMPILE, default $(CROSS_COMPILE)'
	@echo  '  make DESTDIR='lib path''
	@echo  '       Set install absolute directory, default $(DESTDIR)'
	@echo  '  make ACFLAGS='add cflags''
	@echo  '       Build with added cflags'
	@echo  '  make ACPPFLAGS='add cppflags''
	@echo  '       Build with added cppflags'
	@echo  '  make ALDFLAGS='add ldflags''
	@echo  '       Build with added ldflags'
	@echo  '  make DEBUG=y '
	@echo  '       Build with debug mode, default no'

.PHONY: $(PHONY)
