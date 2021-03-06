MYFILE:=myfile

MYFILE_BASEDIR:=$(BASEDIR)/$(MYFILE)

MYFILE_CC:=g++
MYFILE_LD:=g++

MYFILE_MODULES_DEPENDENCIES:=mythread

MYFILE_INCFLAGS:=-I$(MYFILE_BASEDIR)/inc -I$(BASEDIR)/common/inc -I$(MYFILE_BASEDIR)/obj $(patsubst %, -I$(BASEDIR)/%/inc, $(MYFILE_MODULES_DEPENDENCIES))

ifeq ($(SYSTEM), MINGW32_NT-5.1)
MYFILE_DEBUG_CFLAGS:=-g -Wall -W -pedantic -Woverloaded-virtual -D_REENTRANT -DDEBUG $(MYFILE_INCFLAGS)
MYFILE_DEBUG_LDFLAG:=
MYFILE_DEBUG_LIBS:=

MYFILE_RELEASE_CFLAGS:=-Wall -W -pedantic -Woverloaded-virtual -D_REENTRANT -DRELEASE $(MYFILE_INCFLAGS)
MYFILE_RELEASE_LDFLAG:=
MYFILE_RELEASE_LIBS:=

MYFILE_SCRAMBLED_CFLAGS:=-Wall -W -pedantic -Woverloaded-virtual -D_REENTRANT -DRELEASE -DSCRAMBLE_SYMBOLS $(MYFILE_INCFLAGS)
MYFILE_SCRAMBLED_LDFLAG:=
MYFILE_SCRAMBLED_LIBS:=
endif

ifeq ($(SYSTEM), Linux)
MYFILE_DEBUG_CFLAGS:=-g -Wall -W -pedantic -Woverloaded-virtual -D_REENTRANT -DDEBUG $(MYFILE_INCFLAGS)
MYFILE_DEBUG_LDFLAG:=
MYFILE_DEBUG_LIBS:=

MYFILE_RELEASE_CFLAGS:=-Wall -W -pedantic -Woverloaded-virtual -D_REENTRANT -DRELEASE $(MYFILE_INCFLAGS)
MYFILE_RELEASE_LDFLAG:=
MYFILE_RELEASE_LIBS:=

MYFILE_SCRAMBLED_CFLAGS:=-Wall -W -pedantic -Woverloaded-virtual -D_REENTRANT -DRELEASE -DSCRAMBLE_SYMBOLS $(MYFILE_INCFLAGS)
MYFILE_SCRAMBLED_LDFLAG:=
MYFILE_SCRAMBLED_LIBS:=
endif

MYFILE_SRC:=$(shell ls 2>&1 $(MYFILE_BASEDIR)/src/*.cpp) $(shell ls 2>&1 $(MYFILE_BASEDIR)/src/*.y)

TMP_MYFILE_OBJ:= $(patsubst %.cpp,%.o,$(filter %.cpp,$(MYFILE_SRC))) $(patsubst %.y,%.o,$(filter %.y,$(MYFILE_SRC)))
TMP_MYFILE_DEP:= $(patsubst %.cpp,%.d,$(filter %.cpp,$(MYFILE_SRC))) $(patsubst %.y,%.d,$(filter %.y,$(MYFILE_SRC)))

SRC+=$(MYFILE_SRC)

MYFILE_DEBUG_OBJS=$(shell echo $(TMP_MYFILE_OBJ) | sed -e 's@src@obj/debug@g' )
MYFILE_RELEASE_OBJS+=$(shell echo $(TMP_MYFILE_OBJ) | sed -e 's@src@obj/release@g' )
MYFILE_SCRAMBLED_OBJS+=$(shell echo $(TMP_MYFILE_OBJ) | sed -e 's@src@obj/scrambled@g' )

DEBUG_OBJS+=$(MYFILE_DEBUG_OBJS)
RELEASE_OBJS+=$(MYFILE_RELEASE_OBJS)
SCRAMBLED_OBJS+=$(MYFILE_SCRAMBLED_OBJS)

# Dependencies are release independant.
DEPS+=$(shell echo $(TMP_MYFILE_DEP) | sed -e 's@src@dep@g' )

DEBUG_MYFILE_ARCHIVE:=$(BASEDIR)/lib/debug/$(MYFILE).a
RELEASE_MYFILE_ARCHIVE:=$(BASEDIR)/lib/release/$(MYFILE).a
SCRAMBLED_MYFILE_ARCHIVE:=$(BASEDIR)/lib/scrambled/$(MYFILE).a

DEBUG_ARCHIVES+=$(DEBUG_MYFILE_ARCHIVE)
RELEASE_ARCHIVES+=$(RELEASE_MYFILE_ARCHIVE)
SCRAMBLED_ARCHIVES+=$(SCRAMBLED_MYFILE_ARCHIVE)
