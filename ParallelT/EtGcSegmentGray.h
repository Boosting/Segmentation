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

#ifndef ET_GCSEGMENTGRAY_H
#define ET_GCSEGMENTGRAY_H

#include "EtGcSegment.h"

class EtGcSegmentGray : public EtGcSegment{

public:

	//Constructor.
	//w: width of image
	//h: height of image
	//numDecimals: using integer and first numDecimals decimals to sort
	EtGcSegmentGray(int w=1, int h=1, bool doPreProcess = true, bool doPostProcess = true, int numDecimals = 3 );

	~EtGcSegmentGray();
	
	virtual void segment(image<unsigned char> *im, image<rgb>*dest, float sigma, float c, int minSize, std::string sort);	

private:
	//Grayscale image.
	image<float>*img;

	//Gaussian smooth.
	void preprocess(float sigma);

	//Creates edges of graph.
    void buildGraph();

};

#endif


