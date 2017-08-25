# Define path to this lib
pwd_crypto := $(CRYPTO_ROOT)/libcrypto
$(info Including crypto algorithms at $(pwd_crypto))

# Set up object files for this lib
objs_crypto = algotest.o
LIB_OBJECTS += $(objs_crypto)
INCLUDED_LIBS += crypto
INCLUDES += -I$(pwd_crypto)/cryptoalgorithmsbase/headers

include $(CRYPTO_ROOT)/libgmpmods/include.mk

# Define recipe for this build
crypto: $(objs_crypto)

# Define recipes for objects in this lib
$(objs_crypto): %.o: $(pwd_crypto)/cryptoalgorithmsbase/src/%.cpp
	$(CC) -c $(CFLAGS) $(INCLUDES) $^ -o $(OBJECTS_DIR)/$@