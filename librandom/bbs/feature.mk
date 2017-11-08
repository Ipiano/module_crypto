# Define path to this lib
pwd_bbs := $(PWD_RANDOM)/bbs

CRYPTO_LIBS += cryptomath
include $(CRYPTO_ROOT)/include.mk

# Set up object files for this lib
#objs_bbs = bbs.o
#OBJS_CRYPTO += $(objs_bbs)
hdrs_bbs = bbs.h
HDRS_CRYPTO += $(hdrs_bbs)
INCLUDES += -I$(pwd_bbs)/headers

# Define recipes for objects in this lib
#$(objs_bbs): %.o: $(pwd_bbs)/src/%.cpp
#	$(CC) -c $(CFLAGS) $(INCLUDES) $^ -o $(OBJECTS_DIR)/$@

# Define recipes for headers so that they can be used as requirements
# without the full path being known elsewhere
$(hdrs_bbs): %.h: $(pwd_bbs)/headers/%.h