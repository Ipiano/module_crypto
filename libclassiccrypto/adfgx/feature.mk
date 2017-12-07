# Define path to this lib
pwd_adfgx := $(PWD_CLASSICCRYPTO)/adfgx

CRYPTO_LIBS += cryptomath
include $(CRYPTO_ROOT)/include.mk

# Set up object files for this lib
objs_adfgx = $(patsubst %.o, $(OBJECTS_DIR)/%.o, adfgxcipher.o)
OBJS_CLASSICCRYPTO += $(objs_adfgx)

hdrs_adfgx = $(patsubst %.h, $(pwd_adfgx)/headers/%.h, adfgxcipher.h)
HDRS_CLASSICCRYPTO += $(hdrs_adfgx)
INCLUDES += -I$(pwd_adfgx)/headers

# Define recipes for objects in this lib
$(objs_adfgx): $(OBJECTS_DIR)/%.o: $(pwd_adfgx)/src/%.cpp $(hdrs_adfgx)
	$(CC) -c $(CFLAGS) $(DEFINES) $(INCLUDES) $< -o $@