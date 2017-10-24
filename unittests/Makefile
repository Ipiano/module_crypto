# General variables
CC = g++
CFLAGS += --std=c++11 -O1

PROJECT_ROOT = $(PWD)/..
CRYPTO_ROOT = $(PROJECT_ROOT)
CRYPTO_LIBS = random classiccrypto cryptomath
LIBRANDOM_FEATURES = bbs
LIBCLASSICCRYPTO_FEATURES = affine frequency

BUILD_TYPE = release
BUILD_DIR = $(PROJECT_ROOT)/build/$(BUILD_TYPE)
OBJECTS_DIR = $(BUILD_DIR)/objects
LIBS_DIR = $(BUILD_DIR)/lib
DEST_DIR = $(PWD)/$(BUILD_TYPE)

LIBS += -lm -lpthread -lgmpxx -lgmp -L$(LIBS_DIR)

# Build debug mode by default
# Change all output directories
debug: CFLAGS += -g -DDEBUG
debug: BUILD_TYPE = debug
debug: release

release: unittests_modulecrypto

# Include necessary headers and either sources or libraries
include $(CRYPTO_ROOT)/include.mk

# Newline in terminal output
$(info   )

.PHONY: clean mkdirs

mkdirs:
	@-mkdir -p $(BUILD_DIR)
	@-mkdir -p $(OBJECTS_DIR)
	@-mkdir -p $(LIBS_DIR)
	@-mkdir -p $(DEST_DIR)

clean:
	rm $(PWD)/*.o
	rm $(OBJECTS_DIR)/*.o

tests_cryptomath = test_extgcd.o test_gcd.o test_inversemod.o test_mod.o
$(tests_cryptomath): %.o: tests/cryptomath/%.cpp
	$(CC) -c $(CFLAGS) $(INCLUDES) $^ -o $(OBJECTS_DIR)/$@

objs_main = main.o $(tests_cryptomath)

# Substitute objects location onto object files from internal libs
unittests_modulecrypto: build_objects = $(patsubst %.o, $(OBJECTS_DIR)/%.o, $(objs_main) $(LIB_OBJECTS))
unittests_modulecrypto: mkdirs $(objs_main) $(INCLUDED_LIBS)
	$(CC) $(build_objects) $(LIBS) -o $(DEST_DIR)/$@

main.o: main.cpp
	$(CC) -c $(CFLAGS) $(INCLUDES) main.cpp -o $(OBJECTS_DIR)/main.o