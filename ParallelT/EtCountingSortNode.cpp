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

#include "EtCountingSortNode.h"
#include "EtGcSegment.h"

EtCountingSortNode::EtCountingSortNode(int range, int compens)
{
	this->compensate = compens;
	this->weightRange = (range+1)*compensate;
	this->histo = new int[weightRange];
}


EtCountingSortNode::~EtCountingSortNode(void)
{
	delete[] histo;
}


void EtCountingSortNode::sortRound( EtGcSegment::GC_EDGE*src, EtGcSegment::GC_EDGE*dest, int n ){
    memset(histo, 0, sizeof(int)*this->weightRange);
	//create histogram
	for(int i = 0; i < n; ++i)
		++histo[(int)(src[i].w*compensate+0.5)];

	//first location (commulate histo sifted one)
	for(int i = 1 ; i < this->weightRange; ++i){
		histo[i] += histo[i-1];
	}

	int v;
	for(int i = 0; i < n; ++i){
		v = (int)(src[i].w*compensate+0.5);
		dest[ --histo[v] ] = src[i];
	}
}

void EtCountingSortNode::sortFloor( EtGcSegment::GC_EDGE*src, EtGcSegment::GC_EDGE*dest, int n ){
    memset(histo, 0, sizeof(int)*this->weightRange);
	//create histogram
	for(int i = 0; i < n; ++i)
		++histo[(int)(src[i].w*compensate)];

	//first location (commulate histo sifted one)
	for(int i = 1 ; i < this->weightRange; ++i){
		histo[i] += histo[i-1];
	}

	int v;
	for(int i = 0; i < n; ++i){
		v = (int)(src[i].w*compensate);
		dest[ --histo[v] ] = src[i];
	}
}
