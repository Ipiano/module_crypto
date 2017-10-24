$(if $(libclassiccrypto_included),,\
	$(eval libclassiccrypto_included = true)\
	$(eval include $(CRYPTO_ROOT)/libclassiccrypto/libclassiccrypto.mk)\
)