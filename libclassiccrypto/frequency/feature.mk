# Define path to this lib
pwd_frequency := $(PWD_CLASSICCRYPTO)/frequency

# Set up object files for this lib
#objs_frequency = 
#OBJS_CLASSICCRYPTO += $(objs_frequency)
hdrs_frequency = freq_count.h
HDRS_CLASSICCRYPTO += $(hdrs_frequency)
INCLUDES += -I$(pwd_frequency)/headers

# Define recipes for objects in this lib
#$(objs_frequency): %.o: $(pwd_frequency)/src/%.cpp
#	$(CC) -c $(CFLAGS) $(INCLUDES) $^ -o $(OBJECTS_DIR)/$@

# Define recipes for headers so that they can be used as requirements
# without the full path being known elsewhere
$(hdrs_frequency): %.h: $(pwd_frequency)/headers/%.h