LDFLAGS:= -L$(abspath ../../rumprun/lib)
LDFLAGS+= -L$(abspath ../../lib/libbmk_core/hw/)
LDFLAGS+= -L$(abspath ../../lib/libbmk_rumpuser/hw/)

CFLAGS=	-I../../include -I../../rumprun/include -I../../platform/hw/include -nostdlib

LDSCRIPT=$(abspath ../../platform/hw/arch/riscv/kern.ldscript)

OBJS=	main.o $(abspath ../../platform/hw/rumprun.o)

minion.elf: ${OBJS}
	${CC} ${LDFLAGS} -T${LDSCRIPT} ${CFLAGS} 			\
	${OBJS} 							\
	-lrumpkern_sysproxy -lrump 					\
	-lbmk_core -lbmk_rumpuser					\
	-o $@
