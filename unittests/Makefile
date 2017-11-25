# General variables
CC = g++
CFLAGS += --std=c++11
CXXFLAGS = CFLAGS

TARGET = unittests_modulecrypto

PROJECT_ROOT = $(PWD)/..
CRYPTO_ROOT = $(PROJECT_ROOT)
CRYPTO_LIBS = random cryptomath classiccrypto des
LIBRANDOM_FEATURES = bbs
LIBCLASSICCRYPTO_FEATURES = affine frequency vigenere

BUILD_TYPE = release
BUILD_DIR = $(PROJECT_ROOT)/build/$(BUILD_TYPE)
OBJECTS_DIR = $(BUILD_DIR)/objects
LIBS_DIR = $(BUILD_DIR)/lib
DEST_DIR = $(PWD)/$(BUILD_TYPE)

LIBS += -lm -lpthread -L$(LIBS_DIR)

# Build debug mode by default
# Change all output directories
debug: CFLAGS += -g
debug: DEFINES += -DDEBUG
debug: BUILD_TYPE = debug
debug: $(TARGET)

release: CFLAGS += -O2
release: $(TARGET)

debug_gmp: DEFINES += -DCRYPTOMATH_GMP
debug_gmp: LIBS += -lgmpxx -lgmp
debug_gmp: debug

release_gmp: DEFINES += -DCRYPTOMATH_GMP
release_gmp: LIBS += -lgmpxx -lgmp
release_gmp: release

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

clean_debug: BUILD_TYPE = debug
clean_debug: clean
clean_release: clean
clean:
	@-rm -f $(OBJECTS_DIR)/*.o
	@-rm -f $(LIBS_DIR)/*.a
	@-rm -f $(DEST_DIR)/$(TARGET)

tests_cryptomath = test_extgcd.o test_inversemod.o test_mod.o test_continuedfraction.o\
				   test_factor2s.o test_factor.o test_primitiveroots.o test_isprime.o test_gcd.o
$(tests_cryptomath): %.o: tests/cryptomath/%.cpp
	$(CC) -c $(CFLAGS) $(DEFINES) $(INCLUDES) $^ -o $(OBJECTS_DIR)/$@

tests_classiccrypto = test_affine.o test_frequencycount.o test_vigenere.o
$(tests_classiccrypto): %.o: tests/classiccrypto/%.cpp
	$(CC) -c $(CFLAGS) $(DEFINES) $(INCLUDES) $^ -o $(OBJECTS_DIR)/$@

tests_des = test_des64.o test_des4.o
$(tests_des): %.o: tests/des/%.cpp
	$(CC) -c $(CFLAGS) $(DEFINES) $(INCLUDES) $^ -o $(OBJECTS_DIR)/$@

tests_random = test_bbs.o
$(tests_random): %.o: tests/random/%.cpp
	$(CC) -c $(CFLAGS) $(DEFINES) $(INCLUDES) $^ -o $(OBJECTS_DIR)/$@

$(info $(INCLUDED_LIBS))

#objs_main = main.o $(tests_cryptomath) $(tests_classiccrypto) $(tests_random) $(tests_des)
objs_main = main.o $(tests_classiccrypto)

# Substitute objects location onto object files from internal libs
$(TARGET): build_objects = $(patsubst %.o, $(OBJECTS_DIR)/%.o, $(objs_main) $(LIB_OBJECTS))
$(TARGET): mkdirs $(objs_main) $(INCLUDED_LIBS)
	$(CC) $(build_objects) $(LIBS) -o $(DEST_DIR)/$@

main.o: main.cpp
	$(CC) -c $(CFLAGS) $(DEFINES) $(INCLUDES) main.cpp -o $(OBJECTS_DIR)/main.o
