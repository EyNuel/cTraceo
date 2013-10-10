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
#~ CC := clang

##Compiler commands for cross-compiling from linux ia64 to windows:
CCW32 := i686-w64-mingw32-gcc
CCW64 := x86_64-w64-mingw32-gcc

## Set Current Operating system:
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
MATLAB_DIR := /usr/local/matlabr14/
#MATLAB_DIR := /usr/local/MATLAB/R2010b/
#MATLAB_DIR := /usr/local/matlab2008a/

## Your Matlab Version (only relevant if USE_MATLAB == 1):
## Allowable options are: R12, R14, R2007A, R2007B, R2008A, R2008B, R2010B
MATLAB_VERSION	:= R14



## ================================================================
## Do not edit below this point unless you know what you are doing:
## ================================================================

LINUX   := 1
WINDOWS := 2

## Compiler flags:
CFLAGSBASE := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
			-Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
			-Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
			-Wstrict-prototypes -std=c99

ifeq ($(ARCH),32b)
	CFLAGS := $(CFLAGSBASE) -march=i686 -m32
endif
ifeq ($(ARCH),64b)
	CFLAGS := $(CFLAGSBASE) -march=nocona
endif

## Linker Flags:
LFLAGS := -lm

## Matlab output:
ifeq ($(USE_MATLAB),1)
	CFLAGS := $(CFLAGS) -I $(MATLAB_DIR)extern/include 
	LFLAGS := $(LFLAGS) -leng -lmat -lmex -lmx -lut -Wl,

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

## Create a variable containing all definitions to be passed to compiler
include source/version
DEFS := -D VERSION_LONG=$(VERSION_LONG)
DEFS := $(DEFS) -D VERSION_SHORT=$(VERSION_SHORT)
DEFS := $(DEFS) -D USE_MATLAB=$(USE_MATLAB)
DEFS := $(DEFS) -D OS=$(OS)

## Disable checking for files with the folowing names:
.PHONY: all todo cTraceo.exe discuss 32b pg dist doc

# ======================================================================
## Build targets:
all:	dirs
		@echo " "
		@echo "Building cTraceo $(VERSION_SHORT) with standard options -run 'make help' for more information."
		@echo " "
		@$(CC) $(CFLAGS) $(DEFS) -D VERBOSE=0 -O3 -o bin/ctraceo source/cTraceo.c $(LFLAGS)

win:	win32 win64

win32:	dirs
		@echo " "
		@echo "Building cTraceo $(VERSION_SHORT) for Windows x86."
		@echo " "
		@echo "---------------------------------"
		@$(CCW32) $(CFLAGSBASE) -march=i686 -m32 $(DEFS) -D VERBOSE=0 -D WINDOWS -D NDEBUG -O3 -o bin/ctraceo_$(VERSION_SHORT)_win_x86.exe source/cTraceo.c $(LFLAGS) -static
		@echo " "
		@echo "Please ignore possible 'warning: imaginary constants are a GCC extension [enabled by default]'. This is due to a bug in gcc-mingw which has been solved in version 4.8."
		@echo " "

win64:	dirs
		@echo " "
		@echo "Building cTraceo $(VERSION_SHORT) for Windows x86-64."
		@echo " "
		@echo "------------------------------------"
		@$(CCW64) $(CFLAGSBASE) -march=nocona $(DEFS) -D VERBOSE=0 -D WINDOWS -D NDEBUG -O3 -o bin/ctraceo_$(VERSION_SHORT)_win_x86-64.exe source/cTraceo.c $(LFLAGS) -static
		@echo " "
		@echo "Please ignore possible 'warning: imaginary constants are a GCC extension [enabled by default]'. This is due to a bug in gcc-mingw which has been solved in version 4.8."
		@echo " "

linux:	linux32 linux64

linux32:dirs
		@echo " "
		@echo "Building cTraceo $(VERSION_SHORT) for Linux i686."
		@echo " "
		@echo "--------------------------------"
		@$(CC) $(CFLAGSBASE) -march=i686 -m32 $(DEFS) -D VERBOSE=0 -D OS=LINUX -D NDEBUG -O3 -o bin/ctraceo_$(VERSION_SHORT)_linux_i686 source/cTraceo.c $(LFLAGS) -static

linux64:dirs
		@echo " "
		@echo "Building cTraceo $(VERSION_SHORT) for Linux x86-64."
		@echo " "
		@echo "----------------------------------"
		@$(CC) $(CFLAGSBASE) -march=nocona  $(DEFS) -D VERBOSE=0 -D OS=LINUX -D NDEBUG -O3 -o bin/ctraceo_$(VERSION_SHORT)_linux_x86-64 source/cTraceo.c $(LFLAGS) -static

pg:		dirs
		@$(CC) $(CFLAGS) $(DEFS) -D VERBOSE=0 -O3 -pg -o bin/ctraceo source/cTraceo.c $(LFLAGS)

debug:	dirs
		@$(CC) $(CFLAGS) $(DEFS) -D VERBOSE=0 -O0 -g -o bin/ctraceo source/cTraceo.c $(LFLAGS)
		
verbose:dirs
		@$(CC) $(CFLAGS) $(DEFS) -D VERBOSE=1 -O0 -g -o bin/ctraceo source/cTraceo.c $(LFLAGS)

todo:	#list todos from all files
		@for file in $(ALLFILES); do fgrep -H -e TODO $$file; done; true

discuss:#list discussion points from all files
		@for file in $(ALLFILES); do fgrep -H -e DISCUSS $$file; done; true
		
dist:	dirs fileList win linux
		@echo " "
		@echo "Making cTraceo $(VERSION_SHORT) distribution package."
		@echo "----------------------------"
		@if [ ! -d "packages" ]; then mkdir packages; fi
		@tar -czf ./packages/cTraceo_$(VERSION_SHORT).tgz $(ALLFILES)
		@echo "Done."
		
dirs:	#creates 'bin/' and 'doc/' directories if they don't exist
		@if [ ! -d "bin" ]; then mkdir bin; fi
		@if [ ! -d "doc" ]; then mkdir doc; fi
		
help:	#
		@echo " ============================================================================= "
		@echo "                    The cTraceo Acoustic Raytracing Model.                     "
		@echo "           The cTraceo Acoustic Raytracing Model, Version $(VERSION_LONG) "
		@echo "                                                                               "
		@echo " ----------------------------------------------------------------------------- "
		@echo " Website: https://github.com/EyNuel/cTraceo/wiki                               "
		@echo "                                                                               "
		@echo " License: The cTraceo Acoustic Raytracing Model is released under the Creative "
		@echo "          Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License    "
		@echo "          (http://creativecommons.org/licenses/by-nc-sa/3.0/ )                 "
		@echo "                                                                               "
		@echo " NOTE:    cTraceo is research code under active development.                   "
		@echo "          The code may contain bugs and updates are possible in the future.    "
		@echo "                                                                               "
		@echo " ----------------------------------------------------------------------------- "
		@echo " Written for project SENSOCEAN by:                                             "
		@echo "          Emanuel Ey                                                           "
		@echo "          emanuel.ey@gmail.com                                                 "
		@echo "          Copyright (C) 2011 - 2013                                            "
		@echo "          Signal Processing Laboratory                                         "
		@echo "          Universidade do Algarve                                              "
		@echo "                                                                               "
		@echo " cTraceo is the C port of the FORTRAN 77 TRACEO code written by:               "
		@echo "          Orlando Camargo Rodriguez:                                           "
		@echo "          Copyright (C) 2010                                                   "
		@echo "          Orlando Camargo Rodriguez                                            "
		@echo "          orodrig@ualg.pt                                                      "
		@echo "          Universidade do Algarve                                              "
		@echo "          Physics Department                                                   "
		@echo "          Signal Processing Laboratory                                         "
		@echo "                                                                               "
		@echo " ============================================================================= "
		@echo " Available make targets:                                                       "
		@echo "                                                                               "
		@echo "     all:      Compiles model with highest optimization level. [default]       "
		@echo "                                                                               "
		@echo "     pg:       Compiles model with highest optimization level, debugging       "
		@echo "               symbols and profiling information. For use with 'gprof';        "
		@echo "                                                                               "
		@echo "     debug:    Compiles model without optimizations and with debugging simbols;"
		@echo "                                                                               "
		@echo "     verbose:  Compiles the model without optimizations and in verbose mode.   "
		@echo "               Note that depending on the verbosity level defined in           "
		@echo "               'globals.h', the model may become _extremely_ slow.             "
		@echo "                                                                               "
		@echo "     doc:      Generates code documentation. Requires Doxygen to be installed. "
		@echo "                                                                               "
		@echo "     todo:     Prints a list of TODO's found in the source code.               "
		@echo "                                                                               "
		@echo "     dist:     Compiles and statically links all binaries for Windows/Linux    "
		@echo "               in 32 and 64bit flavors, and bundles them in a nice tarball     "
		@echo "               along with the source code, examples and Manual.                "
		@echo "                                                                               "
		@echo "     help:     Prints this help.                                               "
		@echo "                                                                               "
		@echo " ============================================================================= "
		
doc:	dirs
		doxygen Doxyfile

clean:	#clears doc directory
		@if [ -d "doc" ]; then rm -r doc; fi
		
fileList:	dirs #make a list of all files to include in dist package
## A list of all non-source files that are part of the distribution.
AUXFILES := Makefile cTraceo_User_Manual.pdf readme.txt license.txt changelog.txt examples/sletvik_transect.mat bin/ctraceo_$(VERSION_SHORT)_linux_i686 bin/ctraceo_$(VERSION_SHORT)_linux_x86-64 bin/ctraceo_$(VERSION_SHORT)_win_x86-64.exe bin/ctraceo_$(VERSION_SHORT)_win_x86.exe source/version

## A list of directories that belong to the project
PROJDIRS := M-Files examples source source/matOut doc bin

## Recursively create a list of files that are inside the project
SRCFILES := $(shell find $(PROJDIRS) -mindepth 0 -maxdepth 1 -name "*.c")
HDRFILES := $(shell find $(PROJDIRS) -mindepth 0 -maxdepth 1 -name "*.h")
OBJFILES := $(patsubst %.c,%.o,$(SRCFILES))
MFILES   := $(shell find $(PROJDIRS) -mindepth 1 -maxdepth 1 -name "*.m")
PDFFILES := $(shell find $(PROJDIRS) -mindepth 1 -maxdepth 1 -name "*.pdf")

## A list of all files that should end up in a distribution tarball
ALLFILES := $(SRCFILES) $(HDRFILES) $(AUXFILES) $(MFILES) $(PDFFILES)















