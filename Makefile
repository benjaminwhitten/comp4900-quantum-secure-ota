#
# Makefile for QNX OTA Update Project
#

DEBUG = -g
CC = qcc
LD = qcc

TARGET = -Vgcc_ntox86_64

OQS_ROOT = C:/liboqs/build-qnx-x86_64/install
OQS_INC = $(OQS_ROOT)/include
OQS_LIBDIR= $(OQS_ROOT)/lib
OQS_LIB= $(OQS_LIBDIR)/oqs.lib

CFLAGS += $(DEBUG) $(TARGET) -Wall -I$(OQS_INC)

LDFLAGS += $(DEBUG) $(TARGET)

#libraries
LIBS += $(OQS_LIB) -lcrypto

BINS = hash sign_rsa validate_rsa keygen_mldsa sign_mldsa # validate_mldsa

all: $(BINS)

hash: hash.c
	$(CC) $(CFLAGS) hash.c -o hash $(LDFLAGS) $(LIBS)

sign_rsa: sign_rsa.c
	$(CC) $(CFLAGS) sign_rsa.c -o sign_rsa $(LDFLAGS) $(LIBS)

validate_rsa: validate_rsa.c
	$(CC) $(CFLAGS) validate_rsa.c -o validate_rsa $(LDFLAGS) $(LIBS)
	
keygen_mldsa: keygen_mldsa.c
	$(CC) $(CFLAGS) keygen_mldsa.c -o keygen_mldsa $(LDFLAGS) $(LIBS)

sign_mldsa: sign_mldsa.c
	$(CC) $(CFLAGS) sign_mldsa.c -o sign_mldsa $(LDFLAGS) $(LIBS)

# validate_mldsa: validate_mldsa.c
# $(CC) $(CFLAGS) validate_mldsa.c -o validate_mldsa $(LDFLAGS) $(LIBS)

clean:
	rm -f *.o $(BINS)
