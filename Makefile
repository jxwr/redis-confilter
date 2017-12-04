# find the OS
uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')

# Compile flags for linux / osx
ifeq ($(uname_S),Linux)
	SHOBJ_CFLAGS ?= -Wall -W -O3 -fno-common -g -ggdb -std=c99
	SHOBJ_LDFLAGS ?= -shared
else
	SHOBJ_CFLAGS ?= -Wall -W -O3 -dynamic -fno-common -g -ggdb -std=c99
	SHOBJ_LDFLAGS ?= -bundle -undefined dynamic_lookup  -macosx_version_min 10.12
endif

.SUFFIXES: .c .so .xo .o

all:
	@echo ""
	@echo "Make avx     -- Faster if you have a modern CPU."
	@echo "Make generic -- Works everywhere."
	@echo ""
	@echo "The avx code uses AVX2, it requires Haswell (Q2 2013) or better."
	@echo ""

generic: confilter.so

avx:
	make confilter.so CFLAGS=-DUSE_AVX AVX="-mavx2 -mfma"

.c.xo: redismodule.h
	$(CC) -I. $(CFLAGS) $(SHOBJ_CFLAGS) $(AVX) -fPIC -c $< -o $@

confilter.so: confilter.xo
	$(LD) -o $@ $< $(SHOBJ_LDFLAGS) $(LIBS) -lc

clean:
	rm -rf *.xo *.so
