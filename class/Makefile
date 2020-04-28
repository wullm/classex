#Some Makefile for CLASS.
#Julien Lesgourgues, 28.11.2011

########################################################
###### LINES TO ADAPT TO YOUR PLATEFORM ################
########################################################

# path where CLASS is installed
CLASS_PATH = /home/qvgd89/class_m/class_public

# your C compiler:
CC       = gcc

# your optimization flag
OPTFLAG = -O4 -ffast-math -march=native
#OPTFLAG = -Ofast -ffast-math #-march=native
#OPTFLAG = -fast

# your openmp flag (comment for compiling without openmp)
OMPFLAG   = -fopenmp
#OMPFLAG   = -mp -mp=nonuma -mp=allcores -g
#OMPFLAG   = -openmp

# all other compilation flags
LDFLAG = -g -fPIC

# leave blank to compile without HyRec, or put path to HyRec directory
# (with no slash at the end: e.g. hyrec or ../hyrec)
HYREC = hyrec

########################################################
###### IN PRINCIPLE THE REST SHOULD BE LEFT UNCHANGED ##
########################################################

# automatically add external programs if needed. First, initialize to blank.
EXTERNAL =

# eventually update flags for including HyRec
ifneq ($(HYREC),)
vpath %.c $(HYREC)
EXTERNAL += hyrectools.o helium.o hydrogen.o history.o
endif

TOOLS = growTable.o dei_rkck.o sparse.o evolver_rkck.o  evolver_ndf15.o arrays.o parser.o quadrature.o hyperspherical.o common.o trigonometric_integrals.o

SOURCE = input.o background.o thermodynamics.o perturbations.o primordial.o nonlinear.o transfer.o spectra.o lensing.o

all: libclass.so

libclass.so:
	$(CC) $(OPTFLAG) $(OMPFLAG) $(LDFLAG) -shared -o libclass.so $(addprefix $(CLASS_PATH)/build/, $(TOOLS) $(SOURCE) $(EXTERNAL)) -lm
