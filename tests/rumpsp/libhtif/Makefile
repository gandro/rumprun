.include <bsd.own.mk>

LIB=	rumpdev_htif
IOCONF=	HTIF.ioconf

RUMP_SYM_NORENAME=bmk_

SRCS+=	htif.c htif_component.c rumpdev_htif.c
SRCS+=	htif_char.c htif_char_component.c
SRCS+=	htif_eth.c

RUMPTOP= ${TOPRUMP}

CPPFLAGS+=	-I${RUMPTOP}/librump/rumpkern
CPPFLAGS+=	-I${RUMPTOP}/librump/rumpdev
CPPFLAGS+=	-I${RUMPTOP}/librump/rumpvfs
CPPFLAGS+=	-I../../../platform/hw/arch/riscv/
CPPFLAGS+=	-I${.CURDIR}

# XXX
.undef RUMPKERN_ONLY

.include "${RUMPTOP}/Makefile.rump"
.include <bsd.lib.mk>
.include <bsd.klinks.mk>
