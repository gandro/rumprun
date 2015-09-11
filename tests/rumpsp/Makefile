include ../../global.mk
-include ../../platform/hw/config.mk

ifdef BUILDRUMP_TOOLFLAGS
include ${BUILDRUMP_TOOLFLAGS}
endif

CFLAGS+=	${BUILDRUMP_TOOL_CFLAGS}

LDFLAGS:= -L$(abspath ../../rumprun/lib)
LDFLAGS+= -L$(abspath ../../lib/libbmk_core/hw/)
LDFLAGS+= -L$(abspath ../../lib/libbmk_rumpuser/hw/)
LDFLAGS+= -L$(abspath ../../lib/libcompiler_rt/hw)

LDFLAGS+= -L$(abspath libhtif)

CPPFLAGS+= -I../../include -I../../rumprun/include -I../../platform/hw/include
CPPFLAGS+= -nostdlib

LDSCRIPT=$(abspath ../../platform/hw/arch/${MACHINE}/kern.ldscript)

ifeq (${MACHINE},amd64)
LDFLAGS+= -z max-page-size=0x1000
endif

ifeq (${MACHINE},i386)
LDFLAGS+= -m elf_i386
endif

SP_OBJS=rumpuser_sp.o rumpsp.o

OBJS= main.o $(SP_OBJS) $(abspath ../../platform/hw/rumprun.o)

.PHONY: clean run libhtif

all: main.elf

libhtif:
	(cd libhtif \
		&& ${RUMPMAKE} obj \
		&& ${RUMPMAKE} dependall )

main.elf: ${OBJS} libhtif
	${LD} ${LDFLAGS} -T${LDSCRIPT} 					\
	${OBJS} 							\
	-lrumpkern_sysproxy						\
	--whole-archive -lrumpdev_htif -lrumpdev --no-whole-archive	\
	--whole-archive -lrumpvfs -lrump --no-whole-archive		\
	-lbmk_core -lbmk_rumpuser					\
	-lcompiler_rt 							\
	-o $@

run: main.elf
	spike +char=rumpsp.sock main.elf

clean:
	rm -rf main.o main.elf $(SP_OBJS)
	(cd libhtif && ${RUMPMAKE} cleandir )
