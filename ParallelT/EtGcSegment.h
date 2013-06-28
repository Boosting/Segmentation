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

#ifndef ET_GCSEGMENT_H
#define ET_GCSEGMENT_H

#include <cstdlib>
#include "image.h"
#include "misc.h"
#include "filter.h"

using namespace std;


class EtGcSegment{
public:

	typedef struct gcSegment{
		int rank;
		int componentID;
		int size;
		int background;
	} GC_SEGMENT; 


	typedef struct gcEdge{
		float w;
		int a, b;
		bool operator < (const gcEdge &b){return w < b.w;};
		bool operator > (const gcEdge &b){return w > b.w;};
	} GC_EDGE;



protected:
	//Constructor.
	//w: width of image.
	//h: height of image.
	//numDecimals: using integer and first numDecimals decimals to sort.
	//doPreProcess: true - do gauss smooth
	//doPostProcess: merge small segments
	EtGcSegment(int w=1, int h=1,  bool doPreProcess = true, bool doPostProcess = true, int numDecimals = 3 );

	~EtGcSegment();

	void segment( image<rgb>*dest, float c, int minSize, std::string sortingMethod );



	//Image for preprocess.
	image<float> *tmpImage;

	bool rgbImage;

	//Range of weight of edges.
	int weightRange;

	//Considering integer part and numDecimals decimals for sort.
	int numDecimals;

	//Size of image.
	int imgW, imgH;

	//Flag of preprocess.
	bool doPreprocess;

	//Flag of postprocess.
	bool doPostprocess; 

	//Define the join threshold between elements.
	float* thresholds;

	//Storage for graph edges.
	GC_EDGE *edges;

	//Sorted edges.
	GC_EDGE *tmpEdges;

	//for counting sort.
	int* histo; 

	//Segments of image.
	GC_SEGMENT *segments;

	int numEdges;

	int numSegments;


	//Sets default state the edges, universe, segments. (Call before restart the new segmentation process.)
	void clear();
	
	//Finds label of element (original: 2006 Pedro Felzenszwalb)
	int find(int x); 

	//Join too segments. (original: 2006 Pedro Felzenszwalb)
	void join(int x, int y);

	//Segmentation process. (original: 2006 Pedro Felzenszwalb)
	void segmentGraph(float c);

	//Sorts edges by counting sort. round( weight, numDecimals)
	void countingSort();

	//Sorts edges by counting sort. floor( weight, numDecimals)
	void countingSortFloor();

	//Sorts edges by parallel introspective sort.
	void paralellQsort( int ah, int fh);

	//Postprocess (deterministic)
	void postProcess(int minSize);

	//Postprocess (original: 2006 Pedro Felzenszwalb)
	void postProcessOriginal(int minSize);

	//virtual void preProcess(image<float>* r, image<float>* g, image<float>* b, float sigma);

	//Creates result of segmentation.
	void drawSegments(image<rgb>* res);
};

#endif


