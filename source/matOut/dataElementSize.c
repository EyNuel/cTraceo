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
#include <stdint.h>

uint32_t	dataElementSize(size_t dataItemSize, uint32_t nDataItems);

uint32_t	dataElementSize(size_t dataItemSize, uint32_t nDataItems){
	/*
	 * calculates the size of an unwritten data element.
	 * This is needed because the header for an array needs to
	 * contain the number of bytes that compose the array.
	 * NOTE: includes the size of any required padding bytes.
	 */
	uint32_t	nBytes;
	uint32_t	paddingBytes = 0;
	
	nBytes	= dataItemSize * nDataItems;
	if (nBytes % 8 > 0){
		paddingBytes= 8 - nBytes % 8;	//This could probably be neatly rewritten with the ternary operator
	}
	
	return(8 +nBytes +paddingBytes);
}
