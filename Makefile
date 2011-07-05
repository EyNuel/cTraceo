## ================================================================
## Configuration
## Change the values in this section to match your system.
## ================================================================

## The base directory of your matlab installation:
MATLAB_DIR := /usr/local/matlabr14/
#MATLAB_DIR := /usr/local/MATLAB/R2010b/
#MATLAB_DIR := /usr/local/matlab2008a/

## Your Matlab Version:
## Allowable options are: R12, R14, R2007A, R2007B, R2008A, R2008B, R2010B
MATLAB_VERSION	:= R14

## The architecture for which you are compiling:
## Allowable options are: 64b, 32b
ARCH := 64b

## Choose a compiler:
CC 			:= clang
#CC 			:= gcc


## ================================================================
## Do not edit below this point unless you know what you are doing:
## ================================================================

## Compiler flags:
CFLAGS := 	-Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
			-Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
			-Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
			-Wstrict-prototypes -std=gnu99 \
			-I $(MATLAB_DIR)extern/include 

## Linker Flags:
LFLAGS := -lm -leng -lmat -lmex -lut -Wl,

## Generate path to matlab libraries according to configuration:
ifeq ($(ARCH),32b)
	LPATH := $(MATLAB_DIR)bin/glnx86
	CFLAGS := $(CFLAGS) -march=i686 -m32
endif
ifeq ($(ARCH),64b)
	LPATH := $(MATLAB_DIR)bin/glnxa64
endif

## Finalize linker flags:
LFLAGS := $(LFLAGS)-rpath,$(LPATH) -L $(LPATH)

## Define the compiler and linker comands to use:
LINK 		:= $(CC) $(LFLAGS) -o 
COMPLINK 	:= $(CC) $(CFLAGS) $(LFLAGS) -o $@

## A list of all non-source files that are part of the distribution.
AUXFILES := Makefile readme.txt

## A list of directories that belong to the project
PROJDIRS := . M-Files examples

## Recursively create a list of files that are inside the project
SRCFILES := $(shell find $(PROJDIRS) -mindepth 0 -maxdepth 1 -name "*.c")
HDRFILES := $(shell find $(PROJDIRS) -mindepth 0 -maxdepth 1 -name "*.h")
OBJFILES := $(patsubst %.c,%.o,$(SRCFILES))
MFILES   := $(shell find $(PROJDIRS) -mindepth 1 -maxdepth 1 -name "*.m")

## A list of all files that should end up in a distribuition tarball
ALLFILES := $(SRCFILES) $(HDRFILES) $(AUXFILES) $(MFILES)

## Disable checking for files with the folowing names:
.PHONY: all todo cTraceo.exe discuss 32b pg dist

## Build targets:
all:	dirs
		@$(CC) $(CFLAGS) $(LFLAGS) -D VERBOSE=0 -D MATLAB_VERSION=$(MATLAB_VERSION) -O3 -o bin/ctraceo cTraceo.c

pg:		dirs
		@gcc $(CFLAGS) $(LFLAGS) -D VERBOSE=0 -D MATLAB_VERSION=$(MATLAB_VERSION) -O3 -pg -o bin/ctraceo cTraceo.c

debug:	dirs
		@gcc $(CFLAGS) $(LFLAGS) -D VERBOSE=0 -D MATLAB_VERSION=$(MATLAB_VERSION) -O0 -g -o bin/ctraceo cTraceo.c
		
verbose:dirs
		@$(CC) $(CFLAGS) $(LFLAGS) -D VERBOSE=1 -D MATLAB_VERSION=$(MATLAB_VERSION) -O0 -g -o bin/ctraceo cTraceo.c

todo:	#list todos from all files
		@for file in $(ALLFILES); do fgrep -H -e TODO $$file; done; true

discuss:#list discussion points from all files
		@for file in $(ALLFILES); do fgrep -H -e DISCUSS $$file; done; true
		
dist:	#
		@tar -czf ./packages/cTraceo.tgz $(ALLFILES)
		
dirs:	#creates 'bin/' directory if it doesn't exist
		@if [ ! -d "bin" ]; then mkdir bin; fi
