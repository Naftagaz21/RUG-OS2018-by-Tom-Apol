CC = gcc
CFLAGS = -Wall -Werror -std=c99 -pedantic
DEBUGFLAGS = -g
SOURCEDIR := src

ALL_SOURCES := $(shell find $(SOURCEDIR) -name '*.c')

debug:    $(ALL_SOURCES)
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(ALL_SOURCES) -o $@

iwish:
	$(CC) $(CFLAGS) $(ALL_SOURCES) -o $@
