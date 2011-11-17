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
##  Based on previous work by:                                ##
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

# Matlab (Optional)
  This software generates Matlab .mat-files as output, and
  provides 2 methods for writing these files.
  1 - Linking with Matlab(R) libraries during compilation.
  2 - Using a set of internal functions.
  Option 2 is now the recommended method and is enabled by default.
  Although it is still possible to fall back to option 1 this requires
  some extra configuration, is only supported on Linux and requires
  a copy of Matlab(R) to be available.
  

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

# The "CC" variable
  Defines which command to invoke for compilation.
  
# The "USE_MATLAB" variable
  When compiling cTraceo on Linux it is possible to choose between
  using Matlab or the internal functions to write the result files.
  If in doubt, leave at "0".
   
# The "MATLAB_DIR" variable
  This variable should contain the base directory of the 
  system's Matlab(R) installation. This is needed when linking with 
  with Matlab for writing .mat files, and is only possible on Linux.
  

# The "MATLAB_VERSION" variable
  As different versions of Matlab require different variable
  types for calling library functions, this option is required 
  for selecting the appropriate types.
  
# The "ARCH" variable
  This is used to define whether to compile for 32 bit or 64 bit
  systems.



################################################################
## Compilation

 # Linux
   After the options descibed in "Makefile Configuration" have
   been defined, just run:
    
   $> make
   
   This will compile the model and place a binary named "ctraceo"
   in the "bin/" subdirectory.
   
 # Windows
   To compile cTraceo on windows, several steps are required:
   
	* Download and install Cygwin from www.cygwin.com.
	  Cygwin provides basic linux build tools (like 'make') for windows.
      When installing Cygwin packages, select the following packages:
        "devel > gcc4"
        "devel > make"
      Several other packages will be installed for dependencies.
    
    * Add "c:/cygwin/bin" to your system's PATH environment variable.
    
    * Open Cygwin. A command line will appear.
      This command line provides an interface similar to Linux, and
      allows for easy compilation of the model.
      In Cygwin, navigate to location which contains cTraceo's source
      code; this will look similar to:
		$> cd /cygdrive/c/Users/username/Documents/models/cTraceo
	
	* Run:
	    $> make
	  If all goes well, this will produce an executable called
	  "ctraceo.exe" in the "bin/" subdirectory.
    
   
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



