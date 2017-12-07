# Define path to this lib
pwd_frequency := $(PWD_CLASSICCRYPTO)/frequency

# Set up object files for this lib
hdrs_frequency = $(patsubst %.h, $(pwd_frequency)/headers/%.h, freq_count.h)
HDRS_CLASSICCRYPTO += $(hdrs_frequency)
INCLUDES += -I$(pwd_frequency)/headers