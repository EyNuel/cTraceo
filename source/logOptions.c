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
    
    //write the chosen output option to the log file:
    switch(settings->output.calcType){
        case CALC_TYPE__RAY_COORDS:
            LOG(    "Calculating ray coordinates [RCO]\n");
            break;
            
        case CALC_TYPE__ALL_RAY_INFO:
            LOG(    "Calculating all ray information [ARI]\n");
            break;
            
        case CALC_TYPE__EIGENRAYS_PROXIMITY:
            LOG(    "Calculating eigenrays by proximity method [EPR].\n");
            break;
            
        case CALC_TYPE__EIGENRAYS_REG_FALSI:
            LOG(    "Calculating  eigenrays by Regula Falsi Method [ERF].\n");
            break;
            
        case CALC_TYPE__AMP_DELAY_PROXIMITY:
            LOG(    "Calculating amplitudes and delays by Proximity Method [ADP].\n");
            break;
            
        case CALC_TYPE__AMP_DELAY_REG_FALSI:
            LOG(    "Calculating amplitudes and delays by Regula Falsi Method [ADR].\n");
            break;
            
        case CALC_TYPE__COH_ACOUS_PRESS:
            LOG(    "Calculating coherent acoustic pressure [CPR].\n");
            break;
            
        case CALC_TYPE__COH_TRANS_LOSS:
            LOG(    "Calculating coherent transmission loss [CTL].\n");
            break;
            
        case CALC_TYPE__PART_VEL:
            LOG(    "Calculating particle velocity [PVL].\n");
            break;
            
        case CALC_TYPE__COH_ACOUS_PRESS_PART_VEL:
            LOG(    "Calculating coherent acoustic pressure and particle velocity [PAV].\n");
            break;
            
        default:
            fatal("Unknown output option.\nAborting...");
            break;
    }
}
    
    
    
    
    
