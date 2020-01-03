CC=gcc
LD=gcc
OPT_LEVEL=
#OPT_LEVEL=-Ofast -mavx -fomit-frame-pointer -fPIC -mtune=native
CC_OPTS=-g -std=c99 -fPIC -Wfatal-errors
LD_OPTS=-g $(OPT_LEVEL) -flto -lc
INCLUDES=
HEADERS:=$(wildcard *.h) Makefile

.SUFFIXES:

TARGETS=pagerank
all : $(TARGETS)

%.o: %.c $(HEADERS)
	$(CC) $(CC_OPTS) $(INCLUDES) -c $< -o $@

pagerank: pagerank.o
	$(LD) $^ -o $@ $(LD_OPTS)

clean:
	rm -rf $(TARGETS) *.o
