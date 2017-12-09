# General variables
CC = g++
CFLAGS += --std=c++11
CXXFLAGS = CFLAGS

TARGET = unittests_modulecrypto

PROJECT_ROOT = $(PWD)/..
CRYPTO_ROOT = $(PROJECT_ROOT)
CRYPTO_LIBS = random cryptomath classiccrypto des

BUILD_TYPE ?= debug
BUILD_DIR = $(PROJECT_ROOT)/build/$(BUILD_TYPE)
OBJECTS_DIR = $(BUILD_DIR)/objects
LIBS_DIR = $(BUILD_DIR)/lib
DEST_DIR = $(PWD)/$(BUILD_TYPE)

LIBS += -lm -lpthread -L$(LIBS_DIR)

all: $(if $(findstring debug, $(BUILD_TYPE)),\
		$(info Debug Build) \
			$(eval CFLAGS += -g) \
			$(eval DEFINES += -DDEBUG), \
		$(info Release Build) \
			$(eval CFLAGS += -O2))
all: $(TARGET)

gmp: DEFINES += -DCRYPTOMATH_GMP
gmp: LIBS += -lgmpxx -lgmp
gmp: all

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
	@-rm -f $(OBJECTS_DIR)/*.o 2>/dev/null || true
	@-rm -f $(LIBS_DIR)/*.a 2>/dev/null || true
	@-rm -f $(DEST_DIR)/$(TARGET) 2>/dev/null || true

tests_cryptomath = $(patsubst %.o, $(OBJECTS_DIR)/%.o,\
					 test_extgcd.o test_inversemod.o test_mod.o test_continuedfraction.o\
				     test_factor2s.o test_factor.o test_primitiveroots.o test_isprime.o test_gcd.o\
					 test_sundaram.o test_randomprime.o)
$(tests_cryptomath): $(OBJECTS_DIR)/%.o: tests/cryptomath/%.cpp $(HDRS_CRYPTOMATH)
	$(CC) -c $(CFLAGS) $(DEFINES) $(INCLUDES) $< -o $@

tests_classiccrypto = $(patsubst %.o, $(OBJECTS_DIR)/%.o,\
						test_affine.o test_frequencycount.o test_vigenere.o test_adfgx.o)
$(tests_classiccrypto): $(OBJECTS_DIR)/%.o: tests/classiccrypto/%.cpp $(HDRS_CLASSICCRYPTO)
	$(CC) -c $(CFLAGS) $(DEFINES) $(INCLUDES) $< -o $@

tests_des = $(patsubst %.o, $(OBJECTS_DIR)/%.o, \
			  test_des64.o test_des4.o)
$(tests_des): $(OBJECTS_DIR)/%.o: tests/des/%.cpp $(HDRS_DES)
	$(CC) -c $(CFLAGS) $(DEFINES) $(INCLUDES) $< -o $@

tests_random = $(patsubst %.o, $(OBJECTS_DIR)/%.o, \
			     test_bbs.o)
$(tests_random): $(OBJECTS_DIR)/%.o: tests/random/%.cpp $(HDRS_RANDOM)
	$(CC) -c $(CFLAGS) $(DEFINES) $(INCLUDES) $< -o $@

$(info $(INCLUDED_LIBS))

tests = $(tests_cryptomath) $(tests_classiccrypto) $(tests_random) $(tests_des)
objs_main = $(patsubst %.o, $(OBJECTS_DIR)/%.o, main.o)

build_objects = $(objs_main) $(tests) $(LIB_OBJECTS)

# Substitute objects location onto object files from internal libs
$(TARGET): $(build_objects) | mkdirs
	$(CC) $(build_objects) $(LIBS) -o $(DEST_DIR)/$@

$(objs_main): $(OBJECTS_DIR)/%.o: %.cpp
	$(CC) -c $(CFLAGS) $(DEFINES) $(INCLUDES) $< -o $@
