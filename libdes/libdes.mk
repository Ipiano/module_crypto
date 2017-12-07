# Define path to this lib
PWD_DES := $(CRYPTO_ROOT)/libdes
$(info Including DES algorithms at $(PWD_DES))

# List all requirements for library
OBJS_DES = $(patsubst %.o, $(OBJECTS_DIR)/%.o, des64.o des4.o)
HDRS_DES = $(patsubst %.h, $(PWD_DES)/headers/%.h, des64.h des4.h)

# Include headers
INCLUDES += -I$(PWD_DES)/headers 

# Define recipes for objects in this lib
# Dependent upon headers
$(OBJS_DES): $(OBJECTS_DIR)/%.o: $(PWD_DES)/src/%.cpp $(HDRS_DES)
	$(CC) -c $(CFLAGS) $(DEFINES) $(INCLUDES) $< -o $@

# Add headers and objects to full groups
LIB_OBJECTS += $(OBJS_DES)
LIB_HEADERS += $(HDRS_DES)