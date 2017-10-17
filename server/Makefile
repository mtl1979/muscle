# compilation flags used under any OS or compiler (may be appended to, below)
CXXFLAGS   += -I.. -I../regex/regex -I../zlib/zlib -DMUSCLE_SINGLE_THREAD_ONLY
CXXFLAGS   += -DMUSCLE_ENABLE_ZLIB_ENCODING
#CXXFLAGS  += -DMUSCLE_AVOID_IPV6
#CXXFLAGS  += -DMUSCLE_INCLUDE_SOURCE_LOCATION_IN_LOGTIME
#CXXFLAGS  += -DMUSCLE_USE_POLL
#CXXFLAGS  += -DMUSCLE_USE_KQUEUE
#CXXFLAGS  += -DMUSCLE_USE_EPOLL
#CXXFLAGS  += -DMUSCLE_ENABLE_SSL
#CXXFLAGS  += -DMUSCLE_AVOID_CPLUSPLUS11
CXXFLAGS   += -std=c++11 $(CFLAGS) $(DEFINES)

# Uncomment these to enable clang++'s static analyzer (for more thorough error checking)
#CXXFLAGS += --analyze -Xanalyzer -analyzer-output=text
#CFLAGS   += --analyze -Xanalyzer -analyzer-output=text

# compilation flags that are specific to the gcc compiler (hard-coded)
GCCFLAGS    = -fno-exceptions -DMUSCLE_NO_EXCEPTIONS -W -Wall -Wno-multichar

# flags to include when compiling the optimized version (with 'make')
CCOPTFLAGS  = -O3

# flags to include when linking (set per operating system, below)
LFLAGS      =

# libraries to include when linking (set per operating system, below)
LIBS        =

# names of the executables to compile
EXECUTABLES = muscled admin

# object files to include in all executables
OBJFILES = Message.o AbstractMessageIOGateway.o MessageIOGateway.o String.o AbstractReflectSession.o SignalMultiplexer.o SignalHandlerSession.o DumbReflectSession.o StorageReflectSession.o DataNode.o ReflectServer.o SocketMultiplexer.o StringMatcher.o muscled.o MiscUtilityFunctions.o NetworkUtilityFunctions.o SysLog.o PulseNode.o PathMatcher.o FilterSessionFactory.o RateLimitSessionIOPolicy.o MemoryAllocator.o GlobalMemoryAllocator.o SetupSystem.o ServerComponent.o ZLibCodec.o ByteBuffer.o QueryFilter.o Directory.o FilePathInfo.o regcomp.o regerror.o regexec.o regfree.o
ZLIBOBJS = adler32.o deflate.o trees.o zutil.o inflate.o inftrees.o inffast.o crc32.o compress.o gzclose.o gzread.o gzwrite.o gzlib.o

# Where to find .cpp files
VPATH = ../message ../besupport ../dataio ../iogateway ../reflector ../regex ../util ../syslog ../system ../zlib ../zlib/zlib ../regex/regex

# if muscled is crashing, try setting this to no; some older compilers can't handle memory tracking!
MEMORY_TRACKING_SUPPORTED = yes

# if the OS type variable is unset, try to set it using the uname shell command
ifeq ($(OSTYPE),)
  OSTYPE = $(strip $(shell uname))
endif

ifeq ($(OSTYPE),Darwin)
   CXXFLAGS += -stdlib=libc++
   LIBS += -framework SystemConfiguration -framework CoreFoundation
endif

# IRIX may report itself as IRIX or as IRIX64.  They are both the same to us.
ifeq ($(OSTYPE),IRIX64)
  OSTYPE = IRIX
endif

ifeq ($(OSTYPE),Haiku)
   LIBS = -lbe -lnetwork -lroot
   CXXFLAGS  += -DMUSCLE_AVOID_IPV6
endif

ifeq ($(OSTYPE),beos)
   ifeq ($(BE_HOST_CPU),ppc)
      CXX = mwcc
      MEMORY_TRACKING_SUPPORTED = no  # mwcc can't handle it correctly :^(
   else # not ppc
      CXXFLAGS += $(GCCFLAGS) $(CCOPTFLAGS)
      LIBS = -lbe -lnet -lroot
      ifeq ($(shell ls 2>/dev/null -1 /boot/develop/headers/be/bone/bone_api.h), /boot/develop/headers/be/bone/bone_api.h)
         CXXFLAGS += -I/boot/develop/headers/be/bone -DBONE
         LIBS = -nodefaultlibs -lbind -lsocket -lbe -lroot -L/boot/beos/system/lib
      endif
   endif
else # not beos
    ifneq (,$(findstring g++,$(CXX)))
	CXXFLAGS += $(GCCFLAGS) $(CCOPTFLAGS)
    else
	CXXFLAGS += $(CCOPTFLAGS)
    endif
endif

ifeq ($(OSTYPE),freebsd4.0)
   CXXFLAGS += -I/usr/include/machine
endif

ifeq ($(OSTYPE),DragonFly)
   # void
endif

ifeq ($(OSTYPE),IRIX)
   CXXFLAGS += -DSGI -DMIPS
   ifneq (,$(findstring g++,$(CXX))) # if we are using SGI's CC compiler, we gotta change a few things
      CXX = CC
      CCFLAGS = -g2 -n32 -LANG:std -woff 1110,1174,1552,1460,3303
      LFLAGS  = -g2 -n32
      CXXFLAGS += -DNEW_H_NOT_AVAILABLE
      MEMORY_TRACKING_SUPPORTED = no # SGI CC can't handle it correctly :^(
   endif
endif

ifeq ($(OSTYPE), SunOS)
    CXXFLAGS += -DSUN
    ifneq ($(CXX),$(findstring g++,$(CXX))) # if we are using the SunPro compilers, we have to change some things
	CXX = CC
    endif
    LIBS = -lsocket -lnsl
endif

ifneq (,$(findstring MINGW32,$(OSTYPE)))
    OSTYPE=MINGW
endif

ifneq (,$(findstring MINGW64,$(OSTYPE)))
    OSTYPE=MINGW
endif

ifeq ($(OSTYPE),MINGW)
    CC = gcc
    ARCH = $(shell gcc -dumpmachine | sed 's/-.*//g')
    ifeq ($(ARCH),x86_64)
        CXXFLAGS += -m64 -D__MINGW64__ -I/mingw64/x86_64-w64-mingw32/include
        LFLAGS = -L/mingw64/x86_64-w64-mingw32/lib -L/lib
        LIBS = -lws2_32 -lwinmm -liphlpapi -lmingwex -lmsvcrt
    else
        CXXFLAGS += -m32 -D__MINGW32__ -DMUSCLE_AVOID_IPV6 -D_WIN32_WINNT=0x0501 -I/mingw32/i686-w64-mingw32/include
        LFLAGS = -L/mingw32/i686-w64-mingw32/lib -L/lib
        LIBS = -lws2_32 -lwinmm -liphlpapi -lmingwex -lmsvcrt
    endif
endif

ifeq ($(strip $(MEMORY_TRACKING_SUPPORTED)),yes)
   CXXFLAGS += -DMUSCLE_ENABLE_MEMORY_TRACKING
endif

ifneq (,$(findstring MUSCLE_ENABLE_SSL,$(CXXFLAGS))) # Add OpenSSL-specific files only if OpenSSL is enabled
	OBJFILES += SSLSocketDataIO.o SSLSocketAdapterGateway.o
	LIBS += -lssl -lcrypto
endif

all : $(EXECUTABLES)

debug : CCOPTFLAGS = -g
debug : all

muscled : muscled.o libmuscle.a
	$(CXX) $(LFLAGS) -o $@ $^ libmuscle.a $(LIBS)

admin : admin.o libmuscle.a
	$(CXX) $(LFLAGS) -o $@ $^ libmuscle.a $(LIBS)

libmuscle.a : $(OBJFILES) $(ZLIBOBJS)
	$(AR) rcs $@ $^

clean :
	rm -f *.o *.xSYM $(EXECUTABLES) libmuscle.a

