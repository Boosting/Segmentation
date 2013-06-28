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

#include "EtGcSegment.h"

#include <iostream>
#include <stack>
#include <omp.h>

#include "EtTimer.h"


static void threadsTest(){
	int numThreads = omp_get_max_threads(); 
	omp_set_num_threads(numThreads);
	
	#pragma omp parallel for 
	for (int j=0; j<10; ++j)
	{
		int threadNum;
		threadNum = omp_get_thread_num();
		printf("This is thread %d\n", threadNum);
	}	
}

EtGcSegment::EtGcSegment(int w, int h, bool doPreprocess, bool doPostprocess, int numDecimals)
{
	this->imgW = w;
	this->imgH = h;
	this->doPreprocess = doPreprocess;
	this->doPostprocess = doPostprocess;
	this->numDecimals = max( numDecimals, 3);


	//temp storage for filtering
	if( doPreprocess ){
		this->tmpImage = new image<float>(h, w);
	}

	//for build graph
	this->numEdges = w*h*4 -3*w - 3*h+2;		
	this->edges = new GC_EDGE[numEdges];		
			
	//for sort
	this->tmpEdges = new GC_EDGE[numEdges];
		

	//kezdetben minden pont kulon szegmens
	this->thresholds = new float[w*h];
	this->segments = new GC_SEGMENT[w*h];

	this->numSegments = w*h;

	clear();
}


EtGcSegment::~EtGcSegment()
{
	delete[] histo;
	delete[] thresholds;
	delete[] segments;
	delete[] edges;
	delete[] tmpEdges;

	if( doPreprocess ){
		delete tmpImage;
	}
}


//0.5 500 20 d:\Tamop2_4c\TechnicalReport\progik\map.ppm paralellm.ppm 0 1 csr 0
void EtGcSegment::segment(image<rgb>*dest, float c, int minSize,  std::string sortingMethod){
	EtTimer tmr;
	// sort edges by weight

    if( !sortingMethod.compare( "s" ) ){
		tmr.start();		
		std::sort( edges, edges + this->numEdges);		
		tmr.stop();
		std::cout << imgW << " " << imgH << " std::sort " << tmr.getElapsedTime() << endl; 		
	}
    else if( !sortingMethod.compare( "csr" ) ){
		tmr.start();		
		countingSort();	
		tmr.stop();
		std::cout << imgW << " " << imgH << " CountingSort_round " << tmr.getElapsedTime() << endl; 		
	}
    else{ //if( !sortingMethod.compare( "csf" ) )
		tmr.start();		
		countingSortFloor();
		tmr.stop();
		std::cout << imgW << " " << imgH << " CountingSort_floor " << tmr.getElapsedTime() << endl; 		
	}


    tmr.start();

	segmentGraph( c );

	tmr.stop();
	std::cout << imgW << " " << imgH << " Disjoint " << tmr.getElapsedTime() << endl; 


	if( doPostprocess ){
		tmr.start();

		postProcessOriginal( minSize );
		
		tmr.stop();
		std::cout << imgW << " " << imgH << " PostProcess " << tmr.getElapsedTime() << endl; 
	}
	

	drawSegments( dest );
}

void EtGcSegment::drawSegments(image<rgb>* res){
	rgb c;
	for (int y = 0; y < imgH; y++) {
		for (int x = 0; x < imgW; x++) {
			int comp = find(y * imgW + x);	
			c.r = (comp<<1) & 255;
			c.g = (255-comp*17) & 255;
			c.b = (comp<<5) & 255;
			res->access[y][x] = c;
		}
	} 
}


static void swapArrayPointer(EtGcSegment::GC_EDGE*& a, EtGcSegment::GC_EDGE*&b){
	EtGcSegment::GC_EDGE* tmp;
	tmp = a;
	a = b;
	b = tmp;
}

void EtGcSegment::countingSort(){
	int s = (int)pow(10.0, numDecimals);

	//create histogram
	for(int i = 0; i < this->numEdges; ++i)
		++histo[(int)(edges[i].w*s+0.5)];

	//first location (commulate histo sifted one)
	for(int i = 1 ; i < this->weightRange; ++i){
		histo[i] += histo[i-1];
	}

	int v;
	for(int i = 0; i < this->numEdges; ++i){
		v = (int)(edges[i].w*s+0.5);
		tmpEdges[ --histo[v] ] = edges[i];
	}

	swapArrayPointer( edges, tmpEdges);
}

void EtGcSegment::countingSortFloor(){
	int s = (int)pow(10.0, numDecimals);

	//create histogram
	for(int i = 0; i < this->numEdges; ++i)
		++histo[(int)(edges[i].w*s)];

	//first location (commulate histo sifted one)
	for(int i = 1 ; i < this->weightRange; ++i){
		histo[i] += histo[i-1];
	}

	int v;
	for(int i = 0; i < this->numEdges; ++i){
		v = (int)(edges[i].w*s);
		tmpEdges[ --histo[v] ] = edges[i];
	}

	swapArrayPointer( edges, tmpEdges);
}

void EtGcSegment::clear(){
	numSegments = imgW*imgH;
	for (int i = 0; i < numSegments; ++i) {
		segments[i].rank = 0;
		segments[i].background = 0;	
		segments[i].size = 1;
		segments[i].componentID = i;
	}
}


//Find label. (from F&H)
inline int EtGcSegment::find(int i) {
	int tmp = i;

	while (tmp != segments[tmp].componentID)
		tmp = segments[tmp].componentID;

	segments[i].componentID = tmp;

	return tmp;
}

//Join components.(from F&H)
inline void EtGcSegment::join(int x, int y) {
	if (segments[x].rank > segments[y].rank) {
		segments[y].componentID = x;
		segments[x].size += segments[y].size;
	} else {
		segments[x].componentID = y;
		segments[y].size += segments[x].size;
		if (segments[x].rank == segments[y].rank)
			segments[y].rank++;
	}
	--numSegments;
}



// Segment a graph (from F&H)
// Create the disjoint-set forest representing the segmentation. 
// c: constant for treshold function.
void EtGcSegment::segmentGraph(float c) { 

	int numNodes = this->imgW * this->imgH;

	for (int i = 0; i < numNodes; ++i)
		thresholds[i] = c;

	GC_EDGE *pedge = this->edges;
	for (int i = 0; i < numEdges; ++i) {		
		int a = find(pedge->a);
		int b = find(pedge->b);

		if (a != b && pedge->w <= thresholds[a] && pedge->w <= thresholds[b]) {
			join(a, b);
			a = find(a);
			thresholds[a] = pedge->w +  c / segments[a].size;
		}
		++pedge;
	}
}


//join too small segments: deterministic, only work with counting sort 
//(because this use the precomputed commulative histo)
void EtGcSegment::postProcess(int minSize){	
	//0 - not merge, 1 - merge
	int numEdgesm1 = numEdges-1;
	int a, b, ah, fh = 0;
	for (int i = 0; i < weightRange; ++i) {
		fh = ah;
		ah = histo[i];
		//#pragma omp parallel for
		for(int j = ah; j < fh ; ++j){
			a = find(edges[j].a);
			b = find(edges[j].b);
			if ( (a != b) && ((segments[a].size < minSize) || (segments[b].size < minSize))){
				tmpEdges[j].a = 1;
			}
			else{
				tmpEdges[j].a = -1;
			}
		}
		//merges if need
		for(int j = ah; j < fh; ++j){
			if( tmpEdges[j].a > 0){
				a = find(edges[j].a);
				b = find(edges[j].b);
				if( a != b )
					join( a, b );
			}
		}
		
	}
}


//join too small segments: original (F&H), result may be different depend on sorting method (stable or unstable)
void EtGcSegment::postProcessOriginal(int minSize){
	// post process small components
	for (int i = 0; i < numEdges; i++) {
		int a = find(edges[i].a);
		int b = find(edges[i].b);
		if ( (a != b) && ((segments[a].size < minSize) || (segments[b].size < minSize)))
			join(a, b);
	}
}