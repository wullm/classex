#Compiler options
GCC = gcc

#Libraries
INI_PARSER = ../parser/minIni.o
STD_LIBRARIES = -lm
HDF5_LIBRARIES = -lhdf5
CLASS_LIBRARIES = -lclass

CLASS_PATH = /home/qvgd89/class_n/class_public

HDF5_INCLUDES += -I/usr/lib/x86_64-linux-gnu/hdf5/serial/include
HDF5_LIBRARIES += -L/usr/lib/x86_64-linux-gnu/hdf5/serial -I/usr/include/hdf5/serial
CLASS_INCLUDES += -I$(CLASS_PATH)/include
CLASS_LIBRARIES += -L$(CLASS_PATH) -Wl,-rpath=$(CLASS_PATH)

#Putting it together
INCLUDES = $(HDF5_INCLUDES) $(CLASS_INCLUDES)
LIBRARIES = $(INI_PARSER) $(STD_LIBRARIES) $(HDF5_LIBRARIES) $(CLASS_LIBRARIES)
CFLAGS = -Wall

OBJECTS = ../lib/*.o

all:
	@#$(GCC) test_minIni.c -o test_minIni $(INI_PARSER)
	@#@./test_minIni

	$(GCC) test_input.c -o test_input $(OBJECTS) $(LIBRARIES) $(CFLAGS) $(INCLUDES)
	@./test_input

	$(GCC) test_class.c -o test_class $(OBJECTS) $(LIBRARIES) $(CFLAGS) $(INCLUDES)
	@./test_class

	$(GCC) test_titles.c -o test_titles $(OBJECTS) $(LIBRARIES) $(CFLAGS) $(INCLUDES)
	@./test_titles

	$(GCC) test_transfer.c -o test_transfer $(OBJECTS) $(LIBRARIES) $(CFLAGS) $(INCLUDES)
	@./test_transfer

	$(GCC) test_hdf5.c -o test_hdf5 $(OBJECTS) $(LIBRARIES) $(CFLAGS) $(INCLUDES)
	rm -f test.hdf5
	@./test_hdf5
	@rm test.hdf5
	@rm -f output/minimal.*

	$(GCC) test_output.c -o test_output $(OBJECTS) $(LIBRARIES) $(CFLAGS) $(INCLUDES)
	rm -f test_perturb.hdf5
	@./test_output
	@rm test_perturb.hdf5
