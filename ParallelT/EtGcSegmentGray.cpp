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

#include "EtGcSegmentGray.h"
#include <iostream>
#include <omp.h>

#include "EtTimer.h"

//comparer function
#define diff(g, g2) ((g)>(g2)) ? ((g)-(g2)) : ((g2)-(g))


//constructor
EtGcSegmentGray::EtGcSegmentGray(int w, int h, bool doPreprocess, bool doPostprocess, int numDecimals) : 
       EtGcSegment(w, h, doPreprocess, doPostprocess, numDecimals) {

	this->img = new image<float>(w, h); 
	this->weightRange = 255*pow(10.0, numDecimals)+2; //last column only postprocess (num of count)
			
	this->histo = new int[weightRange];		

	clear();	
}


//segmentation
EtGcSegmentGray::~EtGcSegmentGray()
{
	delete img;
}


void EtGcSegmentGray::segment(image<unsigned char>*src, image<rgb>*dest, float sigma, float c, int minSize,  std::string sortingMethod){
	EtTimer tmr;

	//smooth each color channel  
	#pragma omp parallel for
	for (int y = 0; y < imgH; y++) {
		for (int x = 0; x < imgW; x++) {
			img->access[y][x] = src->access[y][x];
		}
	}

	if( doPreprocess ){
	    tmr.start();

		preprocess(sigma);
	    
		tmr.stop();
		std::cout << src->width() << " " << src->height() << " Preprocess " << tmr.getElapsedTime() << endl; 
	}

    tmr.start();

	buildGraph( );

	tmr.stop();
	std::cout << src->width() << " " << src->height() << " BuildGraph " << tmr.getElapsedTime() << endl; 

//	EtGcSegment::segment( src, dest, sigma, c, minSize, sortingMethod );
	EtGcSegment::segment( dest, c, minSize, sortingMethod );
}

void EtGcSegmentGray::preprocess(float sigma){
	smooth2(img, sigma, tmpImage);
}

//parallel graph build
void EtGcSegmentGray::buildGraph(){
	int w = imgW;
	int h = imgH;
	int wm1 = w-1;
	int hm1 = h-1;
	int offs = 0;

    #pragma omp parallel for
	for (int y = 1; y < hm1 ; ++y) {
		for (int x = 0; x < wm1 ; ++x) {
			int p = y*w + x;
			int d = ((y-1) * wm1 + x) * 4;
			edges[d].a = p;
			edges[d].b = p + 1;
			edges[d].w = diff(img->access[y][x],  img->access[y][x+1]);
			edges[d+1].a = p;
			edges[d+1].b = p + w;
			edges[d+1].w = diff(img->access[y][x],  img->access[y+1][x]);
			edges[d+2].a = p;
			edges[d+2].b = p + w + 1;
			edges[d+2].w = diff(img->access[y][x], img->access[y+1][x+1]);
			edges[d+3].a = p;
			edges[d+3].b = p - w + 1;
		    edges[d+3].w = diff(img->access[y][x],  img->access[y-1][x+1]);
		}
	}

	offs = 4*(w - 1)*(h - 2);

	#pragma omp parallel for
	for( int x = 0; x < wm1; ++x){
		int p = x;
		int d = offs + 3*x;
		edges[d].a = p;
		edges[d].b = p + 1;
		edges[d].w = diff(img->access[0][x],  img->access[0][x+1]);
		edges[d+1].a = p;
		edges[d+1].b = p + w;
		edges[d+1].w = diff(img->access[0][x], img->access[0+1][x]);
		edges[d+2].a = p;
		edges[d+2].b = p + w + 1;
		edges[d+2].w = diff(img->access[0][x], img->access[0+1][x+1]);
	}

	//lowest row
	offs += 3 * (w - 1);
	int yr = (h - 1)*w;
	#pragma omp parallel for
	for (int x = 0; x < wm1 ; ++x) {
		int p = yr + x;
		int d = offs + 2*x;
		edges[d].a = p;
		edges[d].b = p + 1;
		edges[d].w = diff(img->access[hm1][x],  img->access[hm1][x+1] );
		edges[d+1].a = p;
		edges[d+1].b = p - w + 1;
		edges[d+1].w = diff(img->access[hm1][x],  img->access[hm1-1][x+1] );
	}

	//last column
	offs += 2 * (w-1);
	int x = w-1;
	#pragma omp parallel for
	for (int y = 0; y < hm1; ++y) {
		int p = y*w + x;
		int d = offs + y;
		edges[d].a = p;
		edges[d].b = p + w;
		edges[d].w = diff(img->access[y][x],  img->access[y+1][x] );
	}

}


//#undef diff