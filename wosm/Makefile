#
# Makefile for wapi programs ©2022 - Patrick-Gilles Maillot
#
# This is to build a simple c program using static C library wapi (WING API)
#
TARGET = wosm
#
WAPI =../wapi
CC=gcc
CFLAGS=-I$(WAPI) -L$(WAPI)

LIBS=-lwapi

_DEPS = 
DEPS = $(patsubst %,$(WAPI)/%,$(_DEPS))


$(TARGET): $(TARGET).c $(DEPS)
	$(CC) -o $@ $< $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o *~ core $(TARGET)
