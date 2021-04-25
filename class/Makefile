#Some Makefile for CLASS.
#Julien Lesgourgues, 28.11.2011
#Nils Schöneberg, Matteo Lucca, 27.02.2019

########################################################
###### LINES TO ADAPT TO YOUR PLATEFORM ################
########################################################

# path where CLASS is installed
CLASS_PATH = /home/qvgd89/class_n/class_public

# your C compiler:
CC       = gcc

# your optimization flag
#OPTFLAG = -O3
OPTFLAG = -Ofast -ffast-math #-march=native
#OPTFLAG = -fast

# your openmp flag (comment for compiling without openmp)
OMPFLAG   = -fopenmp
#OMPFLAG   = -mp -mp=nonuma -mp=allcores -g
#OMPFLAG   = -openmp

# all other compilation flags
CCFLAG = -g -fPIC
LDFLAG = -g -fPIC

# leave blank to compile without HyRec, or put path to HyRec directory
# (with no slash at the end: e.g. "external/RecfastCLASS")
HYREC = external/HyRec2020
RECFAST = external/RecfastCLASS
HEATING = external/heating

########################################################
###### IN PRINCIPLE THE REST SHOULD BE LEFT UNCHANGED ##
########################################################

# pass current working directory to the code
CCFLAG += -D__CLASSDIR__='"$(MDIR)"'

# where to find include files *.h
INCLUDES = -I../include
HEADERFILES = $(wildcard ./include/*.h)

# automatically add external programs if needed. First, initialize to blank.
EXTERNAL =

vpath %.c $(RECFAST)
#CCFLAG += -DRECFAST
INCLUDES += -I../$(RECFAST)
EXTERNAL += wrap_recfast.o
HEADERFILES += $(wildcard ./$(RECFAST)/*.h)

vpath %.c $(HEATING)
#CCFLAG += -DHEATING
INCLUDES += -I../$(HEATING)
EXTERNAL += injection.o noninjection.o
HEADERFILES += $(wildcard ./$(HEATING)/*.h)

# update flags for including HyRec
ifneq ($(HYREC),)
vpath %.c $(HYREC)
CCFLAG += -DHYREC
#LDFLAGS += -DHYREC
INCLUDES += -I../$(HYREC)
EXTERNAL += hyrectools.o helium.o hydrogen.o history.o wrap_hyrec.o energy_injection.o
HEADERFILES += $(wildcard ./$(HYREC)/*.h)
endif

%.o:  %.c .base $(HEADERFILES)
	cd $(WRKDIR);$(CC) $(OPTFLAG) $(OMPFLAG) $(CCFLAG) $(INCLUDES) -c ../$< -o $*.o

TOOLS = growTable.o dei_rkck.o sparse.o evolver_rkck.o  evolver_ndf15.o arrays.o parser.o quadrature.o hyperspherical.o common.o trigonometric_integrals.o

SOURCE = input.o background.o thermodynamics.o perturbations.o primordial.o fourier.o transfer.o harmonic.o lensing.o distortions.o

INPUT = input.o

PRECISION = precision.o

BACKGROUND = background.o

THERMO = thermodynamics.o

PERTURBATIONS = perturbations.o

TRANSFER = transfer.o

PRIMORDIAL = primordial.o

HARMONIC = harmonic.o

FOURIER = fourier.o

LENSING = lensing.o

DISTORTIONS = distortions.o

OUTPUT = output.o

CLASS = class.o

C_TOOLS =  $(addprefix tools/, $(addsuffix .c,$(basename $(TOOLS))))
C_SOURCE = $(addprefix source/, $(addsuffix .c,$(basename $(SOURCE) $(OUTPUT))))
C_TEST = $(addprefix test/, $(addsuffix .c,$(basename $(TEST_DEGENERACY) $(TEST_LOOPS) $(TEST_TRANSFER) $(TEST_FOURIER) $(TEST_PERTURBATIONS) $(TEST_THERMODYNAMICS))))
C_MAIN = $(addprefix main/, $(addsuffix .c,$(basename $(CLASS))))
C_ALL = $(C_MAIN) $(C_TOOLS) $(C_SOURCE)
H_ALL = $(addprefix include/, common.h svnversion.h $(addsuffix .h, $(basename $(notdir $(C_ALL)))))
PRE_ALL = cl_ref.pre clt_permille.pre
INI_ALL = explanatory.ini lcdm.ini
MISC_FILES = Makefile CPU psd_FD_single.dat myselection.dat myevolution.dat README bbn/sBBN.dat external_Pk/* cpp
PYTHON_FILES = python/classy.pyx python/setup.py python/cclassy.pxd python/test_class.py

all: libclass.so

libclass.so:
	$(CC) $(OPTFLAG) $(OMPFLAG) $(LDFLAG) -shared -o libclass.so $(addprefix $(CLASS_PATH)/build/, $(TOOLS) $(SOURCE) $(EXTERNAL)) -lm
