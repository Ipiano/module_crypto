# Define path to this lib
pwd_vigenere := $(PWD_CLASSICCRYPTO)/vigenere

CRYPTO_LIBS += cryptomath
include $(CRYPTO_ROOT)/include.mk

# Set up object files for this lib
objs_vigenere = vigenerecipher.o
OBJS_CLASSICCRYPTO += $(objs_vigenere)

hdrs_vigenere = vigenerecipher.h
HDRS_CLASSICCRYPTO += $(hdrs_vigenere)
INCLUDES += -I$(pwd_vigenere)/headers

# Define recipes for objects in this lib
$(objs_vigenere): %.o: $(pwd_vigenere)/src/%.cpp
	$(CC) -c $(CFLAGS) $(DEFINES) $(INCLUDES) $^ -o $(OBJECTS_DIR)/$@

# Define recipes for headers so that they can be used as requirements
# without the full path being known elsewhere
$(hdrs_vigenere): %.h: $(pwd_vigenere)/headers/%.h