# Define path to this lib
pwd_bbs := $(PWD_RANDOM)/bbs

CRYPTO_LIBS += cryptomath
include $(CRYPTO_ROOT)/include.mk

# Set up object files and headers
hdrs_bbs = $(patsubst %.h, $(pwd_bbs)/headers/%.h, bbs.h)
HDRS_RANDOM += $(hdrs_bbs)
INCLUDES += -I$(pwd_bbs)/headers