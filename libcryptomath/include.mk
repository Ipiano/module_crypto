$(if $(libcryptomath_included),,\
	$(eval libcryptomath_included = true)\
	$(eval include $(CRYPTO_ROOT)/libcryptomath/libcryptomath.mk)\
)