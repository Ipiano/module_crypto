# Define path to this lib
pwd_affine := $(PWD_CLASSICCRYPTO)/affine

CRYPTO_LIBS += cryptomath
include $(CRYPTO_ROOT)/include.mk

# Set up object files for this lib
objs_affine = $(patsubst %.o, $(OBJECTS_DIR)/%.o, affinecipher.o)
OBJS_CLASSICCRYPTO += $(objs_affine)

hdrs_affine = $(patsubst %.h, $(pwd_affine)/headers/%.h, affinecipher.h)
HDRS_CLASSICCRYPTO += $(hdrs_affine)
INCLUDES += -I$(pwd_affine)/headers

# Define recipes for objects in this lib
$(objs_affine): $(OBJECTS_DIR)/%.o: $(pwd_affine)/src/%.cpp $(hdrs_affine)
	$(CC) -c $(CFLAGS) $(DEFINES) $(INCLUDES) $< -o $@