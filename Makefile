#Compiler options
GCC = gcc

#Libraries
INI_PARSER = parser/minIni.o
STD_LIBRARIES = -lm
HDF5_LIBRARIES = -lhdf5
CLASS_LIBRARIES = -lclass

CLASS_PATH = /home/qvgd89/class_m/class_public

HDF5_INCLUDES += -I/usr/lib/x86_64-linux-gnu/hdf5/serial/include
HDF5_LIBRARIES += -L/usr/lib/x86_64-linux-gnu/hdf5/serial -I/usr/include/hdf5/serial
CLASS_INCLUDES += -I$(CLASS_PATH)/include
CLASS_LIBRARIES += -L$(CLASS_PATH) -Wl,-rpath=$(CLASS_PATH)

#Putting it together
INCLUDES = $(HDF5_INCLUDES) $(CLASS_INCLUDES)
LIBRARIES = $(INI_PARSER) $(STD_LIBRARIES) $(HDF5_LIBRARIES) $(CLASS_LIBRARIES)
CFLAGS = -Wall

OBJECTS = lib/*.o

all:
	make minIni
	$(GCC) src/input.c -c -o lib/input.o $(INCLUDES) $(CFLAGS)
	$(GCC) src/output.c -c -o lib/output.o $(INCLUDES) $(CFLAGS)
	$(GCC) src/class_titles.c -c -o lib/class_titles.o $(INCLUDES) $(CFLAGS)
	$(GCC) src/class_transfer.c -c -o lib/class_transfer.o $(INCLUDES) $(CFLAGS)
	$(GCC) src/derivatives.c -c -o lib/derivatives.o $(INCLUDES) $(CFLAGS)
	$(GCC) src/classex.c -o classex $(INCLUDES) $(OBJECTS) $(LIBRARIES) $(CFLAGS)

minIni:
	cd parser && make

check:
	cd tests && make
