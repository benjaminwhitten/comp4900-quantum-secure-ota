
#
#	Makefile for QNX OTA Update Project
#

DEBUG = -g
CC = qcc
LD = qcc


TARGET = -Vgcc_ntox86_64
#TARGET = -Vgcc_ntox86
#TARGET = -Vgcc_ntoarmv7le
#TARGET = -Vgcc_ntoaarch64le


CFLAGS += $(DEBUG) $(TARGET) -Wall -lcrypto
LDFLAGS+= $(DEBUG) $(TARGET)
BINS = hash
all: $(BINS)

clean:
	rm -f *.o $(BINS);
#	cd solutions; make clean
