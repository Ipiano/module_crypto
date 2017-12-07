# Define path to this lib
PWD_CRYPTOMATH := $(CRYPTO_ROOT)/libcryptomath
$(info Including cryptomath algorithms at $(PWD_CRYPTOMATH))

# Set up object files and headers for this lib
OBJS_CRYPTOMATH += $(patsubst %.o, $(OBJECTS_DIR)/%.o, continuedfraction.o)
HDRS_CRYPTOMATH = $(patsubst %.h, $(PWD_CRYPTOMATH)/headers/%.h, \
					cryptomath.h continuedfractions.h math_factoring.h math_misc.h math_modulararith.h math_primality.h)

# Include headers
INCLUDES += -I$(PWD_CRYPTOMATH)/headers

# Define recipes for objects in this lib
$(OBJS_CRYPTOMATH):  $(OBJECTS_DIR)/%.o: $(PWD_CRYPTOMATH)/src/%.cpp $(HDRS_CRYPTOMATH)
	$(CC) -c $(CFLAGS) $(DEFINES) $(INCLUDES) $< -o $@

# Set up object files for this lib
LIB_OBJECTS += $(OBJS_CRYPTOMATH)
LIB_HEADERS += $(HDRS_CRYPTOMATH)
