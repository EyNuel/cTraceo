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
#include "matOut.h"
#include "writeMatfileHeader.c"


MATFile*	matOpen(				const char* filename, const char* openMode);

MATFile* matOpen(const char* filename, const char* openMode){
	/*
	 * Opens a matfile, tests for sucess and:
	 *   #if opening in 'write' mode, writes a matlab header to the file
	 *   #if opening in 'update' (append) mode, skips ahead to last position of file
	 *   #if opening in 'read' mode, throws an error [reading matfiles isn't planned]
	 */
	
	MATFile*	outfile = NULL;
	
	switch (openMode[0]){
		case 'w':
			//open file in mode "write+binary"
			outfile = fopen(filename, "wb");
			
			//verify file was opened successfully
			if(outfile == NULL) {
				fatal("Error while opening file.\n");
			}
			
			//write matlab header to file:
			writeMatfileHeader(outfile, MATLAB_HEADER_TEXT);
			break;
		case 'u':
			//open file in mode "append+binary"
			outfile = fopen(filename, "ab");
			
			//verify file was opened successfully
			if(outfile == NULL) {
				fatal("Error while opening file.\n");
			}
			break;
		default:
			fatal("Opening MATFiles in 'read' mode is not supported [nor planned].");
			break;
	}
	
	return outfile;
}
