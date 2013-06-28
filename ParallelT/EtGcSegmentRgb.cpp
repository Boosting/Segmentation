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

#include "EtGcSegmentRgb.h"
#include <iostream>
#include <omp.h>

#include "EtTimer.h"

#define diffRGB(r, g, b, r2, g2, b2) sqrtf( ((r)-(r2))*((r)-(r2)) + ((g)-(g2))*((g)-(g2)) + ((b)-(b2))*((b)-(b2)) )

EtGcSegmentRgb::EtGcSegmentRgb(int w, int h, bool doPreprocess, bool doPostprocess, int numDecimals) : 
    EtGcSegment(w, h, doPreprocess, doPostprocess, numDecimals) {

	this->r = new image<float>(w, h); 
	this->g = new image<float>(w, h); 
	this->b = new image<float>(w, h); 
	this->weightRange = (int)( sqrtf(255*255*3)*pow(10.0, numDecimals)+1 )+1; //+1 - because of postprocess
			
	this->histo = new int[weightRange];		

	clear();	
}

EtGcSegmentRgb::~EtGcSegmentRgb()
{
	delete r;
	delete g;
	delete b;
}

void EtGcSegmentRgb::segment(image<rgb> *im, image<rgb>*dest, float sigma, float c, int minSize,  std::string sortingMethod){
	EtTimer tmr;

	//smooth each color channel  
	#pragma omp parallel for
	for (int y = 0; y < imgH; y++) {
		for (int x = 0; x < imgW; x++) {
			imRef(this->r, x, y) = imRef(im, x, y).r;
			imRef(this->g, x, y) = imRef(im, x, y).g;
			imRef(this->b, x, y) = imRef(im, x, y).b;
		}
	}

	if( doPreprocess ){
	    tmr.start();

		preprocess(sigma);
	    
		tmr.stop();
		std::cout << im->width() << " " << im->height() << " Preprocess " << tmr.getElapsedTime() << endl; 
	}

    tmr.start();

	buildGraph( );

	tmr.stop();
	std::cout << im->width() << " " << im->height() << " BuildGraph " << tmr.getElapsedTime() << endl; 

    //EtGcSegment::segment( im, dest, sigma, c, minSize, sortingMethod );
	EtGcSegment::segment( dest, c, minSize, sortingMethod );
}


//gauss smooth
void EtGcSegmentRgb::preprocess(float sigma){
	smooth2(r, sigma, tmpImage);
	smooth2(g, sigma, tmpImage);
	smooth2(b, sigma, tmpImage);
}

void EtGcSegmentRgb::buildGraph(){
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
			edges[d].w = diffRGB(r->access[y][x], g->access[y][x], b->access[y][x],  r->access[y][x+1], g->access[y][x+1], b->access[y][x+1]);
			edges[d+1].a = p;
			edges[d+1].b = p + w;
			edges[d+1].w = diffRGB(r->access[y][x], g->access[y][x], b->access[y][x],  r->access[y+1][x], g->access[y+1][x], b->access[y+1][x]);
			edges[d+2].a = p;
			edges[d+2].b = p + w + 1;
			edges[d+2].w = diffRGB(r->access[y][x], g->access[y][x], b->access[y][x], r->access[y+1][x+1], g->access[y+1][x+1], b->access[y+1][x+1]);
			edges[d+3].a = p;
			edges[d+3].b = p - w + 1;
		    edges[d+3].w = diffRGB(r->access[y][x], g->access[y][x], b->access[y][x],  r->access[y-1][x+1], g->access[y-1][x+1], b->access[y-1][x+1]);
		}
	}

	offs = 4*(w - 1)*(h - 2);

	#pragma omp parallel for
	for( int x = 0; x < wm1; ++x){
		int p = x;
		int d = offs + 3*x;
		edges[d].a = p;
		edges[d].b = p + 1;
		edges[d].w = diffRGB(r->access[0][x], g->access[0][x], b->access[0][x],  r->access[0][x+1], g->access[0][x+1], b->access[0][x+1]);
		edges[d+1].a = p;
		edges[d+1].b = p + w;
		edges[d+1].w = diffRGB(r->access[0][x], g->access[0][x], b->access[0][x],  r->access[0+1][x], g->access[1][x], b->access[1][x]);
		edges[d+2].a = p;
		edges[d+2].b = p + w + 1;
		edges[d+2].w = diffRGB(r->access[0][x], g->access[0][x], b->access[0][x],  r->access[0+1][x+1], g->access[1][x+1], b->access[1][x+1]);
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
		edges[d].w = diffRGB(r->access[hm1][x], g->access[hm1][x], b->access[hm1][x],  r->access[hm1][x+1], g->access[hm1][x+1], b->access[hm1][x+1]);
		edges[d+1].a = p;
		edges[d+1].b = p - w + 1;
		edges[d+1].w = diffRGB(r->access[hm1][x], g->access[hm1][x], b->access[hm1][x],  r->access[hm1-1][x+1], g->access[hm1-1][x+1], b->access[hm1-1][x+1]);
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
		edges[d].w = diffRGB(r->access[y][x], g->access[y][x], b->access[y][x],  r->access[y+1][x], g->access[y+1][x], b->access[y+1][x]);
	}

}


//#undef diffRGB