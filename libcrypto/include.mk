$(if $(libcrypto_included),,\
	$(eval libcrypto_included = true)\
	$(eval include $(CRYPTO_ROOT)/libcrypto/libcrypto.mk)\
)