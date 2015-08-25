MYNET:=mynet

MYNET_BASEDIR:=$(BASEDIR)/$(MYNET)

MYNET_CC:=g++
MYNET_LD:=g++

MYNET_MODULES_DEPENDENCIES:=myfile
MYNET_INCFLAGS:=-I$(MYNET_BASEDIR)/inc -I$(BASEDIR)/common/inc $(patsubst %, -I$(BASEDIR)/%/inc, $(MYNET_MODULES_DEPENDENCIES))

ifeq ($(SYSTEM), MINGW32_NT-5.1)
MYNET_DEBUG_CFLAGS:=-g -Wall -W  -Woverloaded-virtual -D_REENTRANT -DDEBUG $(MYNET_INCFLAGS)
MYNET_DEBUG_LDFLAG:=
MYNET_DEBUG_LIBS:=

MYNET_RELEASE_CFLAGS:=-Wall -W -Woverloaded-virtual -D_REENTRANT -DRELEASE $(MYNET_INCFLAGS)
MYNET_RELEASE_LDFLAG:=
MYNET_RELEASE_LIBS:=

endif

ifeq ($(SYSTEM), Linux)
MYNET_DEBUG_CFLAGS:=-g -Wall -W -Woverloaded-virtual -D_REENTRANT -DDEBUG $(MYNET_INCFLAGS)
MYNET_DEBUG_LDFLAG:=
MYNET_DEBUG_LIBS:=-Wl,-Rlib/debug -Llib/debug $(patsubst %, -l%, $(MYNET_MODULES_DEPENDENCIES))

MYNET_RELEASE_CFLAGS:=-Wall -W -Woverloaded-virtual -D_REENTRANT -DRELEASE $(MYNET_INCFLAGS)
MYNET_RELEASE_LDFLAG:=
MYNET_RELEASE_LIBS:=-Wl,-Rlib/release -Llib/release $(patsubst %, -l%, $(MYNET_MODULES_DEPENDENCIES))

endif

#MYNET_SRC:=$(shell ls 2>&1 *.cpp)
#MYNET_SRC:=socketexception.cpp sslserversocket.cpp sslsocketimpl.cpp tcpserversocket.cpp tcpsocketimpl.cpp sslclientsocket.cpp sslsocket.cpp tcpclientsocket.cpp tcpsocket.cpp
MYNET_SRC:=socketexception.cpp tcpserversocket.cpp tcpsocketimpl.cpp tcpclientsocket.cpp tcpsocket.cpp


TMP_MYNET_OBJ:= $(patsubst %.cpp,$(MYNET_BASEDIR)/src/%.o,$(filter %.cpp,$(MYNET_SRC)))
TMP_MYNET_DEP:= $(patsubst %.cpp,$(MYNET_BASEDIR)/src/%.d,$(filter %.cpp,$(MYNET_SRC)))

SRC+=$(MYNET_SRC)

MYNET_DEBUG_OBJS=$(shell echo $(TMP_MYNET_OBJ) | sed -e 's@src@obj/debug@g' )
MYNET_RELEASE_OBJS+=$(shell echo $(TMP_MYNET_OBJ) | sed -e 's@src@obj/release@g' )

DEBUG_OBJS+=$(MYNET_DEBUG_OBJS)
RELEASE_OBJS+=$(MYNET_RELEASE_OBJS)

# Dependencies are release independant.
DEPS+=$(shell echo $(TMP_MYNET_DEP) | sed -e 's@src@dep@g' )

DEBUG_MYNET_ARCHIVE:=$(BASEDIR)/lib/debug/$(MYNET).a
RELEASE_MYNET_ARCHIVE:=$(BASEDIR)/lib/release/$(MYNET).a

DEBUG_MYNET_LIB:=$(BASEDIR)/lib/debug/lib$(MYNET).so
RELEASE_MYNET_LIB:=$(BASEDIR)/lib/release/lib$(MYNET).so

DEBUG_ARCHIVES+=$(DEBUG_MYNET_ARCHIVE)
RELEASE_ARCHIVES+=$(RELEASE_MYNET_ARCHIVE)

DEBUG_LIBS+=$(DEBUG_MYNET_LIB)
RELEASE_LIBS+=$(RELEASE_MYNET_LIB)
