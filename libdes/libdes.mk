# Define path to this lib
PWD_DES := $(CRYPTO_ROOT)/libdes
$(info Including DES algorithms at $(PWD_DES))

# Set up object files for this lib
objs_des = des64.o des4.o
OBJS_DES += $(objs_des)

hdrs_des = des64.h des4.h
HDRS_DES += $(hdrs_des)
INCLUDES += -I$(PWD_DES)/headers

# Define recipes for objects in this lib
$(objs_des): %.o: $(PWD_DES)/src/%.cpp
	$(CC) -c $(CFLAGS) $(DEFINES) $(INCLUDES) $^ -o $(OBJECTS_DIR)/$@

# Define recipes for headers so that they can be used as requirements
# without the full path being known elsewhere
$(hdrs_des): %.h: $(PWD_DES)/headers/%.h

# Set up object files for this lib
LIB_OBJECTS += $(OBJS_DES)
INCLUDED_LIBS += des

# Define recipe for this build
des: $(OBJS_DES) $(HDRS_DES)