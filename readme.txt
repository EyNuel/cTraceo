################################################################
##                                                            ##
##           The cTraceo Acoustic Raytracing Model            ##
##                                                            ##
################################################################
## Website:                                                   ##
##          https://github.com/EyNuel/cTraceo/wiki            ##
##                                                            ##
## License: This file is part of the cTraceo Raytracing Model ##
##          and is released under the Creative Commons        ##
##          Attribution-NonCommercial-ShareAlike 3.0 Unported ##
##          License (CC BY-NC-SA 3.0)                         ## 
##          http://creativecommons.org/licenses/by-nc-sa/3.0/ ##
##                                                            ##
## NOTE:    cTraceo is research code under active development.##
##          The code may contain bugs and updates are possible##
##          in the future.                                    ##
##                                                            ##  
## Written for project SENSOCEAN by:                          ##  
##          Emanuel Ey                                        ##  
##          emanuel.ey@gmail.com                              ##  
##          Copyright (C) 2011, 2012                          ##  
##          Signal Processing Laboratory                      ##  
##          Universidade do Algarve                           ##  
##                                                            ##  
## cTraceo is the C port of the FORTRAN 77 TRACEO code,       ##
## written by:                                                ##
##          Orlando Camargo Rodriguez:                        ##  
##          Copyright (C) 2010                                ##  
##          Orlando Camargo Rodriguez                         ##  
##          orodrig@ualg.pt                                   ##  
##          Universidade do Algarve                           ##  
##          Physics Department                                ##  
##          Signal Processing Laboratory                      ##  
##                                                            ##
################################################################


################################################################
## Contents:
 * Features
 * Software Requirements
 * Makefile Configuration
 * Compilation & Installation
 * Matlab Configuration & Examples
 * License Notes.

################################################################
## Features:

 # Range dependent altimetry/bathymetry including range dependent
  elastic properties (Compressional and shear wave speeds,
  density, as well as compressional and shear attenuation);
  
 # Range dependent sound speed fields;
 
 # Support for user-defined smooth objects within waveguide
   (like rocks, marine mammals, submarines);
   
 # Full suppport for backscattering (from altimetry/bathymetry
   or objects);
   
 # Allows free positioning of acoustic source and receivers
   (altimetry/bathymetry as well as objects and receivers may be
   positioned behind the source for modelling complex waveguides);
   
 # Support for horizontal, vertical, rectangular and arbitrarily
   shaped receiver arrays (allows modelling unusual array shapes,
   multiple arrays as well as array distortions);
   
 # Output options: ray trajectories and amplitudes, eigenray
   search (with user-definable proximity threshhold), arrival
   patterns, acoustic pressure, coherent transmission loss, as
   well as particle velocity components (for modelling vector
   sensors).
   
 # Outputs are written to .mat files (does not require Matlab(R)
   for compilation/usage)

################################################################
## Software Requirements

# Matlab (Optional)
  This software generates Matlab .mat-files as output, and
  provides 2 methods for writing these files.
  1 - Linking with Matlab(R) libraries during compilation.
  2 - Using a set of internal functions.
  Option 2 is now the recommended method and is enabled by default.
  Although it is still possible to fall back to option 1 this
  requires some extra configuration, is only supported on Linux
  and requires a Matlab(R) License to be available.
  

# Compiler Support
  cTraceo requires a C Compiler compatible with the ISO C99
  Standard. This is mostly due to the need for complex
  (Imaginary) Math.
  
 * The GNU C Compiler
   GCC fully supports the ISO C99 standard as of version 4.5.
   The 4.3.x and 4.4.x versions partially implement the C99
   standard and although warnings related to imaginary math may
   be generated, the code is compiled successfully.
   Successfully tested with versions 4.4.4 and 4.4.5 on Linux as
   well as with version 4.5.3 on Windows (using Cygwin)
  
 * Clang - LLVM C Language Frontend
   The Clang compiler is the compiler used for development of the
   cTraceo Model.
   Successfully tested with versions 2.8 and 2.9 on Linux.
  
 * MS Visual Studio
   No version of Microsoft's Visual Studio C Compiler supports 
   the ISO C99 Standard, so compilation should not be possible.
   Not tested.



################################################################
## Makefile Configuration
   To compile this model the user may have to adapt the included
   makefile to match the target system.
   Although the options in the Makefile are mostly self-
   -explanatory, a brief overview will be presented here:

# The "CC" variable
  Defines which command to invoke for compilation.
  
# The "OS" Variable:
  Defines for which operating system to compile.

# The "ARCH" variable
  This is used to define whether to compile for 32 bit or 64 bit
  systems.

# The "USE_MATLAB" variable
  When compiling cTraceo on Linux it is possible to choose between
  using Matlab or the internal functions to write the result files.
  If in doubt, leave at "0".
   
# The "MATLAB_DIR" variable
  This variable should contain the base directory of the 
  system's Matlab(R) installation. This is needed when linking with 
  Matlab for writing .mat files, and is only possible on Linux.
  

# The "MATLAB_VERSION" variable
  As different versions of Matlab require different variable
  types for calling library functions, this option is required 
  for selecting the appropriate types.

################################################################
## Compilation and Installation

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
      "ctraceo.exe" in the "bin/" subdirectory which can now be
      used like any other windows executable.
    
   
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


################################################################
## License Notes
   The cTraceo Raytracing Model is released under the Creative
   Commons Attribution-NonCommercial-ShareAlike 3.0 Unported
   (CC BY-NC-SA 3.0) 
   http://creativecommons.org/licenses/by-nc-sa/3.0/

Below, a short, "human readable" version of the software license
is shown. Read the attached "license.txt" file for the full text
of the license.
----------------------------------------------------------------

In Short:
You are free:
    to Share — to copy, distribute and transmit the work
    to Remix — to adapt the work 

Under the following conditions:
    Attribution   — You must attribute The cTraceo Raytracing
                    Model to SiPLAB (with link to
                    http://www.siplab.fct.ualg.pt/)
    Noncommercial — You may not use this work for commercial 
                    purposes. 
    Share Alike   — If you alter, transform, or build upon this
                    work, you may distribute the resulting work
                    only under the same or similar license to
                    this one. 

With the understanding that:
    Waiver        — Any of the above conditions can be waived if
                    you get permission from the copyright holder.
    Public Domain — Where the work or any of its elements is in
                    the public domain under applicable law, that
                    status is in no way affected by the license.
    Other Rights  — In no way are any of the following rights
                    affected by the license:
                    # Your fair dealing or fair use rights, or
                      other applicable copyright exceptions and
                      limitations;
                    # The author's moral rights;
                    # Rights other persons may have either in
                      the work itself or in how the work is used,
                      such as publicity or privacy rights.
    Notice        — For any reuse or distribution, you must make
                    clear to others the license terms of this
                    work. The best way to do this is with a link to:
                    http://creativecommons.org/licenses/by-nc-sa/3.0/
