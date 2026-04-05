#
# Makefile for QNX OTA Update Project
#

DEBUG = -g
CC = qcc
LD = qcc

TARGET = -Vgcc_ntox86_64

OQS_ROOT   = C:/liboqs/build-qnx-x86_64/install
OQS_INC    = $(OQS_ROOT)/include
OQS_LIBDIR = $(OQS_ROOT)/lib
OQS_LIB    = $(OQS_LIBDIR)/oqs.lib

SRC_DIR = src
BIN_DIR = bin

CFLAGS  += $(DEBUG) $(TARGET) -Wall -I$(OQS_INC)
LDFLAGS += $(DEBUG) $(TARGET)

# libraries
LIBS += $(OQS_LIB) -lcrypto

BINS = hash sign_rsa validate_rsa keygen_mldsa sign_mldsa validate_mldsa
# BINS += validate_mldsa

TARGETS = $(addprefix $(BIN_DIR)/,$(BINS))

all: $(TARGETS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/%: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) $(LIBS)

clean:
	rm -f $(BIN_DIR)/* *.o