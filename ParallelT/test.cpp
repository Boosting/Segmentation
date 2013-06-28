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

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include "image.h"
#include "imconv.h"
#include "misc.h"
#include "pnmfile.h"

#include "EtGcSegmentRgb.h"
#include "EtGcSegmentGray.h"
#include "EtTimer.h"


#include "EtParallelCountingSort.h"
#include "EtParallelCountingSortNode.h"
#include "EtCountingSortNode.h"

int main__(int argc, char* argv[])
{
	char *imgSizeName[]={"240x180","320x280","640x480","800x600","1024x768","1280x960","1280x1024","1600x1200","1600x1280","1920x1440","2048x1536","2400x1800","2560x1920","2560x2048","3072x2304","3200x2400","3200x2560","3840x3072","4000x3000","4096x3072","4800x3600","4800x3840","5120x3840","5120x4096","6400x4800","6400x5120","8000x6000","8000x6400"};
 	int imgSize[] = {43200, 89600, 307200, 480000, 786432, 1228800, 1310720, 1920000, 2048000, 2764800, 3145728, 4320000, 4915200, 5242880, 7077888, 7680000, 8192000, 11796480, 12000000, 12582912, 17280000, 18432000, 19660800, 20971520, 30720000, 32768000, 48000000, 51200000};

	//digital television formats
	int size[] = {176*144, 352*288, 720*480, 720*486, 720*540, 720*576, 768*576, 864*486, 960*720, 1024*576, 1280*720, 1366*768, 1280*1080, 1140*1024, 1440*1080, 1920*1080, 2880*2048};
	int size5[] = {64*64,128*128,256*256, 320*240, 640*480, 720*576, 960*640, 960*720, 1200*800, 1920*1080, 1900*1200, 2560*1440};
	int size2[] = {64*64,128*128,256*256,512*512,1024*1024,2048*2048,4096*4096, 8192*8192};
	int numT = sizeof(size)/sizeof(size[0]);
	const int trials = 50;

	EtTimer tmr;
	int weightRange = (int)sqrtf(256*256*3);
	int weightRangeSq2 = 256*256*3;

	EtCountingSortNode* cs = new EtCountingSortNode( weightRange, 1 );
	EtParallelCountingSortNode* pcs = new EtParallelCountingSortNode( weightRange, 2, 1000 );

	for( int i = 0; i < numT; ++i){
		int n = imgSize[i];
		EtGcSegment::GC_EDGE *a = new EtGcSegment::GC_EDGE[n];
		EtGcSegment::GC_EDGE *b = new EtGcSegment::GC_EDGE[n];

		double avgCs = 0, avgPcs = 0, avgPcms = 0, avgS = 0;

		for (int i2 = 0; i2 < trials; i2++) {
			for (int j = 0; j < n; j++) {
				a[j].a = i2;
				a[j].b = j;
				a[j].w = sqrtf((rand() % weightRangeSq2));
				//a[j].w = (rand() % weightRangeSq2);
			}

			memcpy(b, a, sizeof(EtGcSegment::GC_EDGE)*n);
			tmr.start();
			std::sort(b, b+n); 
			tmr.stop();
			avgS +=tmr.getElapsedTime();

			tmr.start();
			cs->sortFloor(a, b, n);
			tmr.stop();
			avgCs += tmr.getElapsedTime();

			tmr.start();
			pcs->sort(a, b, n);
			tmr.stop();
			avgPcs += tmr.getElapsedTime();

		}
		avgS /= trials;
		avgCs /= trials;
		avgPcs /= trials;
		
		printf("S: %d %s %lf ms\n", n, imgSizeName[i], avgS);
		printf("CS: %d %s %lf ms\n", n, imgSizeName[i], avgCs);
		printf("PCS: %d %s %lf ms\n", n, imgSizeName[i], avgPcs);

		//for (int i = 1; i < n; i++) {
		//	//printf("%5.2f", b[i-1].w);
		//	//if (b[i - 1].w >  b[i].w) {
		//	if (b[i - 1] >  b[i]){
		//		printf("Wrong at %d\n", i);
		//	}
		//}
		//getchar();


		delete[] a;
		delete[] b;	
	}
	//getchar();
	return 0;
}