# Define path to this lib
PWD_CRYPTOMATH := $(CRYPTO_ROOT)/libcryptomath
$(info Including cryptomath algorithms at $(PWD_CRYPTOMATH))

# Set up object files for this lib
objs_cryptomath = continuedfraction.o
OBJS_CRYPTOMATH += $(objs_cryptomath)

hdrs_cryptomath = cryptomath.h continuedfractions.h math_factoring.h math_misc.h math_modulararith.h math_primality.h
HDRS_CRYPTOMATH += $(hdrs_cryptomath)
INCLUDES += -I$(PWD_CRYPTOMATH)/headers

# Define recipes for objects in this lib
$(objs_cryptomath): %.o: $(PWD_CRYPTOMATH)/src/%.cpp
	$(CC) -c $(CFLAGS) $(DEFINES) $(INCLUDES) $^ -o $(OBJECTS_DIR)/$@

# Define recipes for headers so that they can be used as requirements
# without the full path being known elsewhere
$(hdrs_cryptomath): %.h: $(PWD_CRYPTOMATH)/headers/%.h

# Set up object files for this lib
LIB_OBJECTS += $(OBJS_CRYPTOMATH)
INCLUDED_LIBS += cryptomath

# Define recipe for this build
cryptomath: $(OBJS_CRYPTOMATH) $(HDRS_CRYPTOMATH)