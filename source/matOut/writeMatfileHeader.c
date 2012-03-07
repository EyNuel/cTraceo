/*
 *  Copyright 2011 Emanuel Ey <emanuel.ey@gmail.com>
 * 
 *  This file is part of matOut.
 *
 *  MatOut is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  MatOut is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with matOut.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#pragma once
#include <stdio.h>
#include "stdint.h"

FILE* writeMatfileHeader(       FILE* outfile, const char descriptiveText[124]);

FILE* writeMatfileHeader(FILE* outfile, const char descriptiveText[124]){
    size_t      discard;
    /*
     * write 124 bytes of description
     */
    discard = fwrite(descriptiveText, 1, 124, outfile);  (void)discard;
    

    /*
     * write 2 Bytes "Version"
     */
    uint16_t    tempUInt16 = 0x0100;
    discard = fwrite(&tempUInt16, sizeof(uint16_t), 1, outfile);  (void)discard;
    //printf(":%u:\n", tempUInt16);
    

    /*
     * write 2 bytes "Endianness indicator": the characters "M"
     * and "I" concatenated and written as 16 bits.
     */
    tempUInt16 =  'M';
    tempUInt16 <<= 8;
    tempUInt16 |= 'I';
    discard = fwrite(&tempUInt16, sizeof(uint16_t), 1, outfile);  (void)discard;
    
    return outfile;
}
