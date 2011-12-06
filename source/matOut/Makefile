## ================================================================
##					 General Configuration
## ----------------------------------------------------------------
## Change the values in this section to match your system.
## Most options available in this makefile are self-explanatory;
## nevertheless, if in doubt, consult the readme, the manual or
## contact the author.
## ================================================================

## Choose a compiler command:
CC  := gcc
#CC := clang


## ================================================================
## Do not edit below this point unless you know what you are doing:
## ================================================================

## Compiler flags:
CFLAGS :=   -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
			-Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
			-Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
			-Wstrict-prototypes -std=c99

## Linker Flags:
LFLAGS := -lm


## Define the compiler and linker comands to use:
LINK		:= $(CC) $(LFLAGS) -o 
COMPLINK	:= $(CC) $(CFLAGS) $(LFLAGS) -o $@

## A list of all non-source files that are part of the distribution.
AUXFILES := Makefile readme.txt

## A list of directories that belong to the project
PROJDIRS := . 

## Recursively create a list of files that are inside the project
SRCFILES := $(shell find $(PROJDIRS) -mindepth 0 -maxdepth 1 -name "*.c")
HDRFILES := $(shell find $(PROJDIRS) -mindepth 0 -maxdepth 1 -name "*.h")

## A list of all files that should end up in a distribution tarball
ALLFILES := $(SRCFILES) $(HDRFILES) $(AUXFILES)

## Disable checking for files with the folowing names:
.PHONY: all todo pg dist

## Build targets:
all:	dirs
		@$(CC) $(CFLAGS) $(LFLAGS) -D VERBOSE=0 -O3 -o bin/tests tests.c

pg:	 dirs
		@$(CC) $(CFLAGS) $(LFLAGS) -D VERBOSE=0 -O3 -pg -o bin/tests tests.c

debug:  dirs
		@$(CC) $(CFLAGS) $(LFLAGS) -D VERBOSE=0 -O0 -g -o bin/tests tests.c
		
verbose:dirs
		@$(CC) $(CFLAGS) $(LFLAGS) -D VERBOSE=1 -O0 -g -o bin/tests tests.c

todo:   #list todos from all files
		@for file in $(ALLFILES); do fgrep -H -e TODO $$file; done; true

dist:   #
		@if [ ! -d "packages" ]; then mkdir packages; fi
		@tar -czf ./packages/matlabOut.tgz $(ALLFILES)
		
dirs:   #creates 'bin/' directory if it doesn't exist
		@if [ ! -d "bin" ]; then mkdir bin; fi
		
