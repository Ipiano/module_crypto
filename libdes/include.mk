$(if $(libdes_included),,\
	$(eval libdes_included = true)\
	$(eval include $(CRYPTO_ROOT)/libdes/libdes.mk)\
)