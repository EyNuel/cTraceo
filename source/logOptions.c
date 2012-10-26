/****************************************************************************************
 * logOptions.c                                                                         *
 * Writes program options to log file.                                                  *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ *
 * Website:                                                                             *
 *          https://github.com/EyNuel/cTraceo/wiki                                      *
 *                                                                                      *
 * License: This file is part of the cTraceo Raytracing Model and is released under the *
 *          Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License  *
 *          http://creativecommons.org/licenses/by-nc-sa/3.0/                           *
 *                                                                                      *
 * NOTE:    cTraceo is research code under active development.                          *
 *          The code may contain bugs and updates are possible in the future.           *
 *                                                                                      *
 * Written for project SENSOCEAN by:                                                    *
 *          Emanuel Ey                                                                  *
 *          emanuel.ey@gmail.com                                                        *
 *          Copyright (C) 2011, 2012                                                    *
 *          Signal Processing Laboratory                                                *
 *          Universidade do Algarve                                                     *
 *                                                                                      *
 * cTraceo is the C port of the FORTRAN 77 TRACEO code written by:                      *
 *          Orlando Camargo Rodriguez:                                                  *
 *          Copyright (C) 2010                                                          *
 *          Orlando Camargo Rodriguez                                                   *
 *          orodrig@ualg.pt                                                             *
 *          Universidade do Algarve                                                     *
 *          Physics Department                                                          *
 *          Signal Processing Laboratory                                                *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ *
 *  Inputs (when using "explicit" version):                                             *
 *          settings:   Pointer to the settings structure.                              *
 *                                                                                      *
 *  Outputs:                                                                            *
 *          None (writes to log file on disk).                                          *
 *                                                                                      *
 *  Return Value:                                                                       *
 *          None                                                                        *
 *                                                                                      *
 ****************************************************************************************/

void logOptions(settings_t* settings);

void logOptions(settings_t* settings){
    
    if(settings->options.saveSSP == true){
        LOG("Option '--ssp' enabled; saving sound speed profile to 'ssp.mat'\n");
    }
    
    if(settings->options.killBackscatteredRays == true){
        LOG("Option '--killBackscatteredRays' enabled; terminating any backscattered rays.\n");
    }
    
    if(settings->options.writeHeader == false){
        LOG("Option '--noHeader' enabled; not displaying model's header on stdout.\n");
    }
    
    if(settings->options.outputFileName != NULL){
        LOG("Option '--outputFileName' enabled; writing results to %s\n", settings->options.outputFileName);
    }
}
    
    
    
    
    
