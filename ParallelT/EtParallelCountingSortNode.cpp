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

#include "EtParallelCountingSortNode.h"
#include "EtParallelCountingSort.h"

EtParallelCountingSortNode::EtParallelCountingSortNode( int maxWeight, int numThreads, int weightCompense )
: EtParallelCountingSort((maxWeight+1)*weightCompense, numThreads) {
	   this->weightCompens = weightCompense;	
}


EtParallelCountingSortNode::~EtParallelCountingSortNode(){
   EtParallelCountingSort::~EtParallelCountingSort();  
}


void EtParallelCountingSortNode::sort(EtGcSegment::GC_EDGE* src,  EtGcSegment::GC_EDGE* dest, int n){
	EtParallelCountingSort<EtGcSegment::GC_EDGE>::countingSort( n, src, dest, histo, chisto ); 
}


void EtParallelCountingSortNode::createHisto(int ah, int fh, EtGcSegment::GC_EDGE*src, int* histo){
	memset( histo, 0, sizeof(int)*weightRange ); 

	for(int i = ah; i < fh; ++i)
		++histo[(int)(src[i].w*weightCompens)];
}

//put the element to their place
void EtParallelCountingSortNode::putElements(int ah, int fh, EtGcSegment::GC_EDGE* src, EtGcSegment::GC_EDGE* dest, int* histo){
	int v;
	for(int i = ah; i < fh; ++i){
		v = (int)(src[i].w * weightCompens);
		dest[ histo[v]++ ] = src[i];
	}
}

