# Define path to this lib
pwd_gmpmods := $(CRYPTO_ROOT)/libgmpmods
$(info Including gmp additions at $(pwd_gmpmods))

# Set up object files for this lib
objs_gmpmods = gmpmodtest.o
LIB_OBJECTS += $(objs_gmpmods)
INCLUDED_LIBS += gmpmods
INCLUDES += -I$(pwd_gmpmods)/headers

# Define recipe for this build
gmpmods: $(objs_gmpmods)

# Define recipes for objects in this lib
$(objs_gmpmods): %.o: $(pwd_gmpmods)/src/%.cpp
	$(CC) -c $(CFLAGS) $(INCLUDES) $^ -o $(OBJECTS_DIR)/$@