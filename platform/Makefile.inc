ifeq (${PLATFORM},)
$(error need to specify $$PLATFORM!)
endif

COREDIR:=		$(shell pwd)/../../lib/libbmk_core
RUMPUSERDIR:=		$(shell pwd)/../../lib/libbmk_rumpuser
BASEDIR:=		$(shell pwd)/../../lib/librumprun_base
RUMPCOMPILER_RT:=	$(shell pwd)/../../lib/librumpkern_compiler_rt

COMMONDIR:=	$(abspath ../)

LDFLAGS_BAKE+=	-L${BASEDIR}/${PLATFORM} -L${COREDIR}/${PLATFORM}	\
		-L${RUMPUSERDIR}/${PLATFORM}

define BUILDLIB_target =
.PHONY: ${1}/${PLATFORM}/${2}
${1}/${PLATFORM}/${2}:
	( cd ${1} \
	    && ${RUMPMAKE} MAKEOBJDIR=${PLATFORM} obj \
	    && ${RUMPMAKE} MAKEOBJDIR=${PLATFORM} dependall )
endef

$(eval $(call BUILDLIB_target,${BASEDIR},librumprun_base.a))
$(eval $(call BUILDLIB_target,${COREDIR},libbmk_core.a))
$(eval $(call BUILDLIB_target,${RUMPUSERDIR},libbmk_rumpuser.a))
$(eval $(call BUILDLIB_target,${RUMPCOMPILER_RT},librumpkern_compiler_rt.a))

${COMMONDIR}/pseudolinkstubs.c: ${BASEDIR}/${PLATFORM}/librumprun_base.a
	sh ../makepseudolinkstubs.sh ${NM} ${RUMPSRC} $< $@

commonlibs: platformlibs userlibs
userlibs: ${BASEDIR}/${PLATFORM}/librumprun_base.a ${COMMONDIR}/pseudolinkstubs.o
platformlibs: ${COREDIR}/${PLATFORM}/libbmk_core.a ${RUMPUSERDIR}/${PLATFORM}/libbmk_rumpuser.a
compiler_rt: ${RUMPCOMPILER_RT}/${PLATFORM}/librumpkern_compiler_rt.a

.PHONY: buildtest
buildtest: ../../tests/hello/hello.c ${OBJ_DIR}/rumprun.o commonlibs app-tools
	${APP_TOOLS_CC} -g -o $@ $< -lrumprun_tester
	@echo Testing baking ...
	@export RUMPRUN_WARNING_STFU=please ; for board in \
	    $(shell RUMPRUN_WARNING_STFU=please ${APP_TOOLS_DIR}/rumpbake list\
		| grep ^${PLATFORM}); do \
			echo $${board} ; \
			${APP_TOOLS_DIR}/rumpbake $${board} $@.$${board} $@ \
			    || exit 1;\
	done
	@echo done

.PHONY: commonclean
commonclean:
	( cd ${BASEDIR} && ${RUMPMAKE} MAKEOBJDIR=${PLATFORM} cleandir )
	( cd ${COREDIR} && ${RUMPMAKE} MAKEOBJDIR=${PLATFORM} cleandir )
	( cd ${RUMPUSERDIR} && ${RUMPMAKE} MAKEOBJDIR=${PLATFORM} cleandir )
	( cd ${RUMPCOMPILER_RT} && \
		${RUMPMAKE} RUMPSRC=${RUMPSRC} MAKEOBJDIR=${PLATFORM} cleandir )
	rm -f ${COMMONDIR}/pseudolinkstubs.c ${COMMONDIR}/pseudolinkstubs.o

.PHONY: tests
tests: ${OBJ_DIR}/rumprun.o commonlibs app-tools
	../../tests/buildtests.sh ${PLATFORM}
	../../tests/runtests.sh ${PLATFORM_DEFAULT_TESTER}
