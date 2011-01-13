
#see: 	http://wiki.osdev.org/Makefile
#		http://www.xs4all.nl/~evbergen/nonrecursive-make.html

# Compiler and linker flags:
CFLAGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
			-Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
			-Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
			-Wconversion -Wstrict-prototypes \
			-I /usr/local/matlabr14/extern/include \
			-D_GNU_SOURCE
# LFLAGS := 	
# -L /usr/local/matlabr14/bin/glnxa64 -leng -lmat -lmex -lut -Wl,-rpath,/usr/local/matlabr14/bin/glnxa64

# Define the compiler and linker comands to use:
CC 			:= clang
LINK 		:= $(CC) $(LFLAGS) -o 
COMPLINK 	:= $(CC) $(CFLAGS) $(LFLAGS) -o $@

# A list of all non-source files that are part of the distribution.
AUXFILES := Makefile munk.in

# A list of directories that belong to the project
PROJDIRS := .
	#functions includes internals

# Recursively create a list of files that are inside the project
SRCFILES := $(shell find $(PROJDIRS) -mindepth 0 -maxdepth 3 -name "*.c")
HDRFILES := $(shell find $(PROJDIRS) -mindepth 0 -maxdepth 3 -name "*.h")
OBJFILES := $(patsubst %.c,%.o,$(SRCFILES))

# Automatically create dependency files for every file in the project
DEPFILES := $(patsubst %.c, %.d,$(SRCFILES))
-include $(DEPFILES)

# A list of all files that should end up in a distribuition tarball
ALLFILES := $(SRCFILES) $(HDRFILES) $(AUXFILES)


# Disable checking for files with the folowing names:
.PHONY: all todo cTraceo.exe discuss
			
# Rules are listed top-level first, with this format:
# target:	dependecy
#			command

all:	#cTraceo.exe
		@$(CC) $(CFLAGS) -o cTraceo.exe ctraceo.c
#		@rm *.o
#		@echo ".c files: " $(SRCFILES)
#		@echo ".o files: " $(OBJFILES)
		
cTraceo.exe:	#$(OBJFILES)
				#$(CC) -o cTraceo.exe ctraceo.c

#		@$(CC) $(OBJFILES) -o cTraceo.exe				
todo:	#list todos from all files
		@for file in $(ALLFILES); do fgrep -H -e TODO $$file; done; true

discuss:	#list discussion points from all files
		@for file in $(ALLFILES); do fgrep -H -e DISCUSS $$file; done; true
		
#%.o:	%.c Makefile	#Automagically create depencies
#		@$(CC) $(CFLAGS) -DNDEBUG -MMD -MP -MT "$*.d $*.o" -g -std=c99 -c $< -o $@ #-I./includes -I./internals

#%:		%.o
#		$(LINK)

#%:		%.c
#		$(COMPLINK)






		
