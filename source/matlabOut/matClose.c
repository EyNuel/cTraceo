#pragma once
#include <stdio.h>
#include "matlabOut.h"

void matClose(MATFile* file);

void matClose(MATFile* file){
	fclose(file);
}
