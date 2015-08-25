LOCALDIR:=.
BASEDIR:=.
SYSTEM:=$(shell uname)
MODULES:=
DEBUG_TARGETS:=
RELEASE_TARGETS:=

include $(LOCALDIR)/module.def

# Each module will add its source file to the following
# variables.
DEBUG_OBJS:=
RELEASE_OBJS:=

DEBUG_ARCHIVES:=
RELEASE_ARCHIVES:=

DEBUG_LIBS:=
RELEASE_LIBS:=

include $(patsubst %, $(BASEDIR)/%/module.mk, $(MODULES))

###############################################################################
# Targets
###############################################################################

default: 
	@echo
	@echo "Please use one of the following and available parameters:"
	@echo " * 'debug' to generate the libraries and applications with debug symbols and debug log capability."
	@echo " * 'release' to generate the libraries and applications without debug symbols (stripped) and no debug log capability."
	@echo " * 'clean' to clean all the objects, libraries, archives and generated binaries."
	@echo " * 'cleanall' to clean all generated files (as above) plus the dependency files."
	@echo " * 'install' to install the files into $(PREFIX) to be delivered together with GisusDLL library."
	@echo

debug: $(DEBUG_OBJS) $(DEBUG_ARCHIVES) $(DEBUG_LIBS) $(DEBUG_TARGETS)

release: $(RELEASE_OBJS) $(RELEASE_ARCHIVES) $(RELEASE_LIBS) $(RELEASE_TARGETS)

clean:
	@echo "Cleaning ..."
	@for m in $(MODULES) ; do find $(BASEDIR)/$$m/obj -type f -exec rm -f {} \; -print ; done
	@if [ -d lib ]; then find lib -type f -exec rm -f {} \; -print ; fi
	@if [ -d bin ]; then find bin -type f -exec rm -f {} \; -print ; fi
	@find . -mindepth 1 \( -name \*~ -o -name core\.\* \) -exec rm -Rf {} \; -print || true

cleanall:
	@echo "Cleaning all ..."
	@for m in $(MODULES) ; do find $(BASEDIR)/$$m/obj -type f -exec rm -f {} \; -print ; done
	@if [ -d lib ]; then find lib -type f -exec rm -f {} \; -print ; fi
	@if [ -d bin ]; then find bin -type f -exec rm -f {} \; -print ; fi
	@find . -mindepth 1 \( -name \*~ -o -name core\.\* \) -exec rm -Rf {} \; -print || true
	@for d in */dep; do find $$d -type f -exec rm -Rf {} \; -print; done 

install: 
	@if [ ! -d $(PREFIX)inc ]; then mkdir -p $(PREFIX)inc; fi
	@if [ ! -d $(PREFIX)src ]; then mkdir -p $(PREFIX)src; fi
	@for i in common/inc/constants.h common/inc/mydll.h common/inc/mytypes.h gisusdll/inc/gisusdll.h mythread/inc/mythread.hxx mythread/inc/mymutex.hxx mythread/inc/mytimer.hxx; do echo "Copying $$i into $(PREFIX)inc ..."; cp -f $$i $(PREFIX)inc ; done
	@for i in suapp/src/suapp.cpp; do echo "Copying $$i into $(PREFIX)src ..."; cp -f $$i $(PREFIX)src ; done


###############################################################################
# Rules
###############################################################################

.PRECIOUS: $(MYCRYPTO_BASEDIR)/obj/debug/%.s $(MYCRYPTO_BASEDIR)/obj/release/%.s 


# Includes the rules to build objects from inserted modules.
include $(patsubst %, $(BASEDIR)/%/rules.mk, $(MODULES))

# Includes the C/C++ header dependencies for each object (automatically generated)
#
# NOTE: The following supposes that the corresponding rules.mk file contains
# sufficient information to build the .d file.
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),cleanall)
ifneq ($(MAKECMDGOALS),doc)
ifneq ($(MAKECMDGOALS),delivery)
ifneq ($(MAKECMDGOALS),)
     include $(DEPS)
endif
endif
endif
endif
endif
