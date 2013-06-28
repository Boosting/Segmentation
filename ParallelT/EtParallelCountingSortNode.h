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

#ifndef ETPARALLELCOUNTINGSORTNODE_H
#define ETPARALLELCOUNTINGSORTNODE_H

#include <stdlib.h>

#include "EtGcSegment.h"
#include "EtParallelCountingSort.h"


//#define weightRange 442999
//#define weightRange 129999

class EtParallelCountingSortNode : public EtParallelCountingSort<EtGcSegment::GC_EDGE>{
//
public:
	EtParallelCountingSortNode( int maxWeight=255, int numThreads = 0, int weightCompens = 1 );

	~EtParallelCountingSortNode();

	void sort(EtGcSegment::GC_EDGE* src, EtGcSegment::GC_EDGE*dest, int n);

private:
	int weightCompens;

	void createHisto(int ah, int fh, EtGcSegment::GC_EDGE*src, int* histo);

	void putElements(int ah, int fh, EtGcSegment::GC_EDGE* src, EtGcSegment::GC_EDGE* dest, int* histo);

	void countingSort(int n, EtGcSegment::GC_EDGE* src,  EtGcSegment::GC_EDGE* dest, int* histo, int* chisto);
};

#endif