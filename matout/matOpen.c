#pragma once
#include <stdio.h>
#include "matlabOut.h"
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
				exit(EXIT_FAILURE);		//this is redundant but avoids "control may reach end of non-void function" warning
			}
			
			//write matlab header to file:
			writeMatfileHeader(outfile, MATLAB_HEADER_TEXT);
			break;
		case 'u':
			fatal("Opening MATFiles in 'update' mode is not yet supported.");	//TODO
			break;
		default:
			fatal("Opening MATFiles in 'read' mode is not supported [nor planned].");
			break;
	}
	
	return outfile;
}
