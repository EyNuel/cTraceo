################################################################
##                                                            ##
##           The cTraceo Acoustic Raytracing Model            ##
##                                                            ##
################################################################
##                                                            ##
##  Written by:                                               ##
##          Emanuel Ey                                        ##
##          emanuel.ey@gmail.com                              ##
##          Universidade do Algarve                           ##
##          Signal Processing Laboratory                      ##
##                                                            ##
##  Based on previous work by:                  ##
##          Orlando Camargo Rodriguez                         ##
##          Copyright (C) 2010                                ##
##          orodrig@ualg.pt                                   ##
##          Universidade do Algarve                           ##
##          Physics Department                                ##
##          Signal Processing Laboratory                      ##
##                                                            ##
################################################################


################################################################
## Features
TODO

################################################################
## Dependencies

# Matlab
  This software generates Matlab .mat-files as output, and thus
  requires a version of Matlab(R) to be installed.
  Compilation without a working copy of Matlab(R) is not
  supported.

# Compiler Support
  cTraceo requires a C Compiler compatible with the ISO C99
  Standard. This is mostly due to the need for complex
  (Imaginary) Math.
  
  The GNU C Compiler
  GCC fully supports the ISO C99 standard as of version 4.5.
  The 4.3.x and 4.4.x versions partially implement the C99
  standard and although warnings may be generated, the code is
  compiled successfully.
  Successfully tested with versions 4.4.4 and 4.4.5 on Linux.
  
  Clang - LLVM C Language Frontend
  The Clang compiler is the compiler used for development of the
  cTraceo Model.
  Successfully tested with versions 2.8 and 2.9 on Linux.
  
  MS Visual Studio
  No version of Microsoft's Visual Studio C Compiler supports 
  the ISO C99 Standard, so compilation should not be possible.
  Not tested.



################################################################
## Makefile Configuration
   To compile this model the user will have to adapt the included
   makefile to match the target system.
   Although the options in the Makefile are mostly self-explanatory,
   a brief overview will be presented here:
   
# The "MATLAB_DIR" variable
  This variable should contain the base directory of the 
  system's Matlab(R) installation.
  This is needed for linking with the libraries which are 
  required for writing .mat files.

# The "MATLAB_VERSION" variable
  As different versions of Matlab require different variable
  types for calling library functions, this option is required 
  for selecting the appropriate types.
  
# The "ARCH" variable
  This is used to define whether to compile for 32 bit or 64 bit
  systems.
  
# The "CC" variable
  Defines which command to invoke for compilation.


################################################################
## Compilation
   After the options descibed in "Makefile Configuration" have
   been defined, just run:
   
   $> make
   
   This will compile the model and place a binary named "ctraceo"
   in the "bin/" subdirectory.
   
   
################################################################
## Matlab Configuration & Example Cases
   Several example cases are contained in the "examples/"
   subdirectory. These examples depend on matlab functions
   contained in the "M-Files/" subdirectory and as such this
   folder should be added to the matlab path.
   
   Within the provided example files the model is invoked by
   calling the command "ctraceo", and thus the "bin/" folder 
   containing the compiled cTraceo binary should be on the
   system's PATH.



