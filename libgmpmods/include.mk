$(if $(libgmpmods_included),,\
	$(eval libgmpmods_included = true)\
	$(eval include $(CRYPTO_ROOT)/libgmpmods/libgmpmods.mk)\
)