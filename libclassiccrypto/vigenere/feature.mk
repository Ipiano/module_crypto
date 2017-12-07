# Define path to this lib
pwd_vigenere := $(PWD_CLASSICCRYPTO)/vigenere

CRYPTO_LIBS += cryptomath
include $(CRYPTO_ROOT)/include.mk

# Set up object files for this lib
objs_vigenere = $(patsubst %.o, $(OBJECTS_DIR)/%.o, vigenerecipher.o)
OBJS_CLASSICCRYPTO += $(objs_vigenere)

hdrs_vigenere = $(patsubst %.h, $(pwd_vigenere)/headers/%.h, vigenerecipher.h)
HDRS_CLASSICCRYPTO += $(hdrs_vigenere)
INCLUDES += -I$(pwd_vigenere)/headers

# Define recipes for objects in this lib
$(objs_vigenere): $(OBJECTS_DIR)/%.o: $(pwd_vigenere)/src/%.cpp $(hdrs_vigenere)
	$(CC) -c $(CFLAGS) $(DEFINES) $(INCLUDES) $< -o $@