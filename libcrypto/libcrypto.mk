$(if $(CRYPTO_FEATURES),,\
	$(eval CRYPTO_FEATURES = frequency ceasar affine))

# Define path to this lib
PWD_CRYPTO := $(CRYPTO_ROOT)/libcrypto
$(info Including crypto algorithms at $(PWD_CRYPTO))

# Feature files fill OBJS_CRYPTO, add to INCLUDES, and define recipies for specific objects
$(foreach feature,$(CRYPTO_FEATURES),$(eval include $(PWD_CRYPTO)/$(feature)/feature.mk))

# Set up object files for this lib
LIB_OBJECTS += $(OBJS_CRYPTO)
INCLUDED_LIBS += crypto

# Define recipe for this build
crypto: $(OBJS_CRYPTO) $(HDRS_CRYPTO)