# Define path to this lib
pwd_affine := $(PWD_CLASSICCRYPTO)/affine

CRYPTO_LIBS += cryptomath
include $(CRYPTO_ROOT)/include.mk

# Set up object files for this lib
#objs_affine = affinecipher.o
#OBJS_CLASSICCRYPTO += $(objs_affine)
hdrs_affine = affinecipher.h
HDRS_CLASSICCRYPTO += $(hdrs_affine)
INCLUDES += -I$(pwd_affine)/headers

# Define recipes for objects in this lib
#$(objs_affine): %.o: $(pwd_affine)/src/%.cpp
#	$(CC) -c $(CFLAGS) $(INCLUDES) $^ -o $(OBJECTS_DIR)/$@

# Define recipes for headers so that they can be used as requirements
# without the full path being known elsewhere
$(hdrs_affine): %.h: $(pwd_affine)/headers/%.h