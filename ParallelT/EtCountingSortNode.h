/*
Copyright (C) 2013 Sz. Szeghalmy

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/

#ifndef ETCOUNTINGSORTNODE_H
#define ETCOUNTINGSORTNODE_H

#include "EtGcSegment.h"

class EtCountingSortNode
{
public:
	EtCountingSortNode(int range = 256, int compens = 1);
	~EtCountingSortNode(void);

	void sortFloor( EtGcSegment::GC_EDGE*src, EtGcSegment::GC_EDGE*dest, int n );
	void sortRound( EtGcSegment::GC_EDGE*src, EtGcSegment::GC_EDGE*dest, int n );

private:
	int* histo;

	int weightRange;
	int compensate;
};


#endif
