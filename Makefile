#Compiler options
GCC = gcc

#Libraries
INI_PARSER = parser/minIni.o
STD_LIBRARIES = -lm
HDF5_LIBRARIES = -lhdf5
CLASS_LIBRARIES = -lclass

CLASS_PATH = /home/qvgd89/class_n/class_public

HDF5_INCLUDES += -I/usr/lib/x86_64-linux-gnu/hdf5/serial/include
HDF5_LIBRARIES += -L/usr/lib/x86_64-linux-gnu/hdf5/serial -I/usr/include/hdf5/serial
CLASS_INCLUDES += -I$(CLASS_PATH)/include -I$(CLASS_PATH)/external/HyRec2020 -I$(CLASS_PATH)/external/RecfastCLASS -I$(CLASS_PATH)/external/heating
CLASS_LIBRARIES += -L$(PWD)/class -Wl,-rpath=$(PWD)/class

#Putting it together
INCLUDES = $(HDF5_INCLUDES) $(CLASS_INCLUDES)
LIBRARIES = $(INI_PARSER) $(STD_LIBRARIES) $(HDF5_LIBRARIES) $(CLASS_LIBRARIES)
CFLAGS = -Wall -Wshadow=global -Ofast -march=native

OBJECTS = lib/*.o

all:
	make minIni
	make classlib
	$(GCC) src/input.c -c -o lib/input.o $(INCLUDES) $(CFLAGS)
	$(GCC) src/output.c -c -o lib/output.o $(INCLUDES) $(CFLAGS)
	$(GCC) src/class_titles.c -c -o lib/class_titles.o $(INCLUDES) $(CFLAGS)
	$(GCC) src/class_transfer.c -c -o lib/class_transfer.o $(INCLUDES) $(CFLAGS)
	$(GCC) src/derivatives.c -c -o lib/derivatives.o $(INCLUDES) $(CFLAGS)
	$(GCC) src/classex.c -o classex $(INCLUDES) $(OBJECTS) $(LIBRARIES) $(CFLAGS)

minIni:
	cd parser && make

classlib:
	cd class && make

check:
	cd tests && make

clean:
	rm lib/*.o
	rm parser/*.o
	rm class/*.so
