# Define path to this lib
pwd_affine := $(PWD_CRYPTO)/affine

# Set up object files for this lib
objs_affine = affinecipher.o
OBJS_CRYPTO += $(objs_affine)
#hdrs_frequency = freq_count.h
#HDRS_CRYPTO += $(hdrs_frequency)
INCLUDES += -I$(pwd_affine)/headers

# Define recipes for objects in this lib
$(objs_affine): %.o: $(pwd_affine)/src/%.cpp
	$(CC) -c $(CFLAGS) $(INCLUDES) $^ -o $(OBJECTS_DIR)/$@

# Define recipes for headers so that they can be used as requirements
# without the full path being known elsewhere
#$(hdrs_frequency): %.h: $(pwd_affine)/headers/%.h