$(if $(librandom_included),,\
	$(eval librandom_included = true)\
	$(eval include $(CRYPTO_ROOT)/librandom/librandom.mk)\
)