#
# Makefile for QNX OTA Update Project
#

DEBUG = -g
CC = qcc
LD = qcc

TARGET = -Vgcc_ntoaarch64le

OQS_ROOT   = C:/liboqs/build-qnx-aarch64/install
OQS_INC    = $(OQS_ROOT)/include
OQS_LIBDIR = $(OQS_ROOT)/lib

SRC_DIR = src
BIN_DIR = bin

CFLAGS  += $(DEBUG) $(TARGET) -Wall -I$(OQS_INC)
LDFLAGS += $(DEBUG) $(TARGET) -L$(OQS_LIBDIR)

LIBS += -loqs -lcrypto

BINS = hash sign_rsa validate_rsa keygen_mldsa sign_mldsa validate_mldsa
TARGETS = $(addprefix $(BIN_DIR)/,$(BINS))

all: $(TARGETS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/%: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) $(LIBS)

clean:
	rm -f $(BIN_DIR)/* *.o