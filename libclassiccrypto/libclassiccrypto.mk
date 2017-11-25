$(if $(LIBCLASSICCRYPTO_FEATURES),,\
	$(eval LIBCLASSICCRYPTO_FEATURES = frequency affine vigenere))

# Define path to this lib
PWD_CLASSICCRYPTO := $(CRYPTO_ROOT)/libclassiccrypto
$(info Including classic crypto algorithms at $(PWD_CLASSICCRYPTO))

# Feature files fill OBJS_CLASSICCRYPTO, add to INCLUDES, and define recipies for specific objects
$(foreach feature,$(LIBCLASSICCRYPTO_FEATURES),$(eval include $(PWD_CLASSICCRYPTO)/$(feature)/feature.mk))

# Set up object files for this lib
LIB_OBJECTS += $(OBJS_CLASSICCRYPTO)
INCLUDED_LIBS += classiccrypto

# Define recipe for this build
classiccrypto: $(OBJS_CLASSICCRYPTO) $(HDRS_CLASSICCRYPTO)