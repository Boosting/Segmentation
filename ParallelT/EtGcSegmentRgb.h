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

#ifndef ET_GCSEGMENTRGB_H
#define ET_GCSEGMENTRGB_H

#include "EtGcSegment.h"

class EtGcSegmentRgb : public EtGcSegment{

public:

	//Constructor.
	//w: width of image
	//h: height of image
	//numDecimals: using integer and first numDecimals decimals to sort
	EtGcSegmentRgb(int w=1, int h=1, bool doPreProcess = true, bool doPostProcess = true, int numDecimals = 3 );

	~EtGcSegmentRgb();
	
	virtual void segment(image<rgb> *im, image<rgb>*dest, float sigma, float c, int minSize, std::string sort);	

private:
	//RGB image channels.
	image<float>*r, *g, *b;

	//gaussian smooth
	void preprocess(float sigma);

	//Creates edges of graph.
    void buildGraph();

	
};

#endif


