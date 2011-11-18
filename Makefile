## ================================================================
##                     General Configuration
## ----------------------------------------------------------------
## Change the values in this section to match your system.
## Most options available in this makefile are self-explanatory;
## nevertheless, if in doubt, consult the readme, the manual or
## contact the authors.
## ================================================================

## Choose a compiler command:
CC  := gcc
#CC := clang

## Set Operating system:
## Allowable values are: WINDOWS, LINUX
OS  := LINUX

## The architecture for which you are compiling:
## Allowable options are: 64b, 32b
ARCH := 64b


## ================================================================
##                      Matfile Configuration
## ----------------------------------------------------------------
## This section is only applicable to Linux systems, and editing it
## is optional.
## The cTraceo model writes results in the form of Matlab's .mat
## files, and provides 2 methods for creating theese files. 
## On Linux, it is possible to choose between linking with the
## libraries provided by MATLAB(R), or using the internal functions
## for writing the matfiles.
## 
## ================================================================

## Set to 1 to link with matlab instead of using the internal functions
## to write .mat files containing the results:
USE_MATLAB := 0


## The base directory of your matlab installation (only relevant
## if USE_MATLAB == 1):
#MATLAB_DIR := /usr/local/matlabr14/
#MATLAB_DIR := /usr/local/MATLAB/R2010b/
MATLAB_DIR := /usr/local/matlab2008a/

## Your Matlab Version (only relevant if USE_MATLAB == 1):
## Allowable options are: R12, R14, R2007A, R2007B, R2008A, R2008B, R2010B
MATLAB_VERSION	:= R2008B



## ================================================================
## Do not edit below this point unless you know what you are doing:
## ================================================================

LINUX   := 1
WINDOWS := 2

## Compiler flags:
CFLAGS := 	-Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
			-Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
			-Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
			-Wstrict-prototypes -std=gnu99

ifeq ($(ARCH),32b)
	CFLAGS := $(CFLAGS) -march=i686 -m32
endif

## Linker Flags:
LFLAGS := -lm

## Matlab output:
ifeq ($(USE_MATLAB),1)
	CFLAGS := $(CFLAGS) -I $(MATLAB_DIR)extern/include 
	LFLAGS := $(LFLAGS) -leng -lmat -lmex -lut -Wl,

	## Generate path to matlab libraries according to configuration:
	ifeq ($(ARCH),32b)
		LPATH := $(MATLAB_DIR)bin/glnx86
	endif
	ifeq ($(ARCH),64b)
		LPATH := $(MATLAB_DIR)bin/glnxa64
	endif
	
	## Finalize linker flags:
	LFLAGS := $(LFLAGS)-rpath,$(LPATH) -L $(LPATH)
	
endif


## Define the compiler and linker comands to use:
LINK 		:= $(CC) $(LFLAGS) -o 
COMPLINK 	:= $(CC) $(CFLAGS) $(LFLAGS) -o $@

## A list of all non-source files that are part of the distribution.
AUXFILES := Makefile readme.txt examples/sletvik_transect.mat

## A list of directories that belong to the project
PROJDIRS := . M-Files examples matlabOut

## Recursively create a list of files that are inside the project
SRCFILES := $(shell find $(PROJDIRS) -mindepth 0 -maxdepth 1 -name "*.c")
HDRFILES := $(shell find $(PROJDIRS) -mindepth 0 -maxdepth 1 -name "*.h")
OBJFILES := $(patsubst %.c,%.o,$(SRCFILES))
MFILES   := $(shell find $(PROJDIRS) -mindepth 1 -maxdepth 1 -name "*.m")
PDFFILES := $(shell find $(PROJDIRS) -mindepth 1 -maxdepth 1 -name "*.pdf")

## A list of all files that should end up in a distribution tarball
ALLFILES := $(SRCFILES) $(HDRFILES) $(AUXFILES) $(MFILES) $(PDFFILES)

## Disable checking for files with the folowing names:
.PHONY: all todo cTraceo.exe discuss 32b pg dist

## Build targets:
all:	dirs
		@$(CC) $(CFLAGS) $(LFLAGS) -D VERBOSE=0 -D USE_MATLAB=$(USE_MATLAB) -D OS=$(OS) -D MATLAB_VERSION=$(MATLAB_VERSION) -O3 -o bin/ctraceo cTraceo.c

pg:		dirs
		@$(CC) $(CFLAGS) $(LFLAGS) -D VERBOSE=0 -D USE_MATLAB=$(USE_MATLAB) -D OS=$(OS) -D MATLAB_VERSION=$(MATLAB_VERSION) -O3 -pg -o bin/ctraceo cTraceo.c

debug:	dirs
		@$(CC) $(CFLAGS) $(LFLAGS) -D VERBOSE=0 -D USE_MATLAB=$(USE_MATLAB) -D OS=$(OS) -D MATLAB_VERSION=$(MATLAB_VERSION) -O0 -g -o bin/ctraceo cTraceo.c
		
verbose:dirs
		@$(CC) $(CFLAGS) $(LFLAGS) -D VERBOSE=1 -D USE_MATLAB=$(USE_MATLAB) -D OS=$(OS) -D MATLAB_VERSION=$(MATLAB_VERSION) -O0 -g -o bin/ctraceo cTraceo.c

todo:	#list todos from all files
		@for file in $(ALLFILES); do fgrep -H -e TODO $$file; done; true

discuss:#list discussion points from all files
		@for file in $(ALLFILES); do fgrep -H -e DISCUSS $$file; done; true
		
dist:	#
		@if [ ! -d "packages" ]; then mkdir packages; fi
		@tar -czf ./packages/cTraceo.tgz $(ALLFILES)
		
dirs:	#creates 'bin/' directory if it doesn't exist
		@if [ ! -d "bin" ]; then mkdir bin; fi
		
