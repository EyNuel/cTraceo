
#see: 	http://wiki.osdev.org/Makefile
#		http://www.xs4all.nl/~evbergen/nonrecursive-make.html

# Compiler and linker flags:
CFLAGS := 	-Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
			-Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
			-Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
			-Wconversion -Wstrict-prototypes -std=gnu99 \
			-I /usr/local/matlabr2008b/extern/include \
			-I /usr/local/matlab2008a/extern/include \
			-I /usr/local/matlabr14/extern/include \

# The following 3 lines are for compilation on siplab 64b machines:
LFLAGS := 	-L /usr/local/matlabr14/bin/glnxa64 \
			-lm -leng -lmat -lmex -lut \
			-Wl,-rpath,/usr/local/matlabr14/bin/glnxa64

# The following 3 lines are a case for compilation on a 32b machine:
#LFLAGS :=	-L /usr/local/matlabr2008b/bin/glnx86 \
#			-lm -leng -lmat -lmex -lut \
#			-Wl,-rpath,/usr/local/matlabr2008b/bin/glnx86 \

#another case 64b case:
#LFLAGS :=	-L /usr/local/matlab2008a/bin/glnxa64 \
#			-lm -leng -lmat -lmex -lut \
#			-Wl,-rpath /usr/local/matlab2008a/bin/glnx86 \


# Define the compiler and linker comands to use:
#CC 			:= clang
CC 			:= gcc

LINK 		:= $(CC) $(LFLAGS) -o 
COMPLINK 	:= $(CC) $(CFLAGS) $(LFLAGS) -o $@

# A list of all non-source files that are part of the distribution.
AUXFILES := Makefile

# A list of directories that belong to the project
PROJDIRS := . M-Files
	#functions includes internals

# Recursively create a list of files that are inside the project
SRCFILES := $(shell find $(PROJDIRS) -mindepth 0 -maxdepth 1 -name "*.c")
HDRFILES := $(shell find $(PROJDIRS) -mindepth 0 -maxdepth 1 -name "*.h")
OBJFILES := $(patsubst %.c,%.o,$(SRCFILES))
MFILES   := $(shell find $(PROJDIRS) -mindepth 1 -maxdepth 1 -name "*.m")

# Automatically create dependency files for every file in the project
#DEPFILES := $(patsubst %.c, %.d,$(SRCFILES))
#-include $(DEPFILES)

# A list of all files that should end up in a distribuition tarball
ALLFILES := $(SRCFILES) $(HDRFILES) $(AUXFILES) $(MFILES)

# Disable checking for files with the folowing names:
.PHONY: all todo cTraceo.exe discuss 32b pg dist

all:	#cTraceo.exe
		@$(CC) $(CFLAGS) $(LFLAGS) -O3 -o bin/cTraceo-64b.bin cTraceo.c


32b:	#cTraceo.exe
		@$(CC) $(CFLAGS) $(LFLAGS) -O3 -march=i686 -m32 -o bin/cTraceo-32b.bin cTraceo.c

pg:		#
		@gcc $(CFLAGS) $(LFLAGS) -O3 -pg -o bin/cTraceo-64b.bin cTraceo.c

debug:	#
		@$(CC) $(CFLAGS) $(LFLAGS)  -O0 -g -o bin/cTraceo-64b.bin cTraceo.c

todo:	#list todos from all files
		@for file in $(ALLFILES); do fgrep -H -e TODO $$file; done; true

discuss:	#list discussion points from all files
		@for file in $(ALLFILES); do fgrep -H -e DISCUSS $$file; done; true
		
dist:	#
		@tar -czf ./packages/cTraceo.tgz $(ALLFILES)
#@tar -czf ./packages/cTraceo.tgz calcAllRayInfo.c intBarycParab1D.c readIn.c Makefile intBarycParab2D.c reflectionCorr.c calcRayCoords.c rkf45.c boundaryInterpolation.c convertUnits.c intLinear1D.c solveDynamicEq.c boundaryReflectionCoeff.c csValues.c interpolation.h solveEikonalEq.c bracket.c dotProduct.c lineLineIntersec.c specularReflection.c cTraceo.c linearSpaced.c cValues1D.c globals.h thorpe.c cValues2D.c intBarycCubic1D.c rayBoundaryIntersection.c tools.c
		

