$(if $(LIBRANDOM_FEATURES),,\
	$(eval LIBRANDOM_FEATURES = bbs))

# Define path to this lib
PWD_RANDOM := $(CRYPTO_ROOT)/librandom
$(info Including random algorithms at $(PWD_RANDOM))

# Feature files fill OBJS_RANDOM, add to INCLUDES, and define recipies for specific objects
$(foreach feature,$(LIBRANDOM_FEATURES),$(eval include $(PWD_RANDOM)/$(feature)/feature.mk))

# Set up object files for this lib
LIB_OBJECTS += $(OBJS_RANDOM)
LIB_HEADERS += $(HDRS_RANDOM)