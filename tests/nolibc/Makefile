include ../../global.mk
include ../../platform/hw/config.mk

LDFLAGS:= -L$(abspath ../../rumprun/lib)
LDFLAGS+= -L$(abspath ../../lib/libbmk_core/hw/)
LDFLAGS+= -L$(abspath ../../lib/libbmk_rumpuser/hw/)
LDFLAGS+= -L$(abspath ../../lib/librumpkern_compiler_rt/)

CPPFLAGS+= -I../../include -I../../rumprun/include -I../../platform/hw/include
CPPFLAGS+= -nostdlib

LDSCRIPT=$(abspath ../../platform/hw/arch/${MACHINE}/kern.ldscript)

ifeq (${MACHINE},amd64)
LDFLAGS+= -z max-page-size=0x1000
endif

OBJS= main.o $(abspath ../../platform/hw/rumprun.o)

.PHONY: clean

main.elf: ${OBJS}
	${LD} ${LDFLAGS} -T${LDSCRIPT} 					\
	${OBJS} 							\
	-lrumpkern_compiler_rt 						\
	--whole-archive -lrumpvfs -lrump --no-whole-archive		\
	-lbmk_core -lbmk_rumpuser					\
	-o $@

clean:
	rm -rf main.o main.elf
