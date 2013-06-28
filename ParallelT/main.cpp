/*
Copyright (C) 2013 Sz. Szeghalmy 
(Sz. Szeghalmy, Gráf vágás alapú szegmentálás, Az elmélet és a gyakorlat találkozása a térinformatikában, Debrecen, 2013, ISBN: 978-963-318-334-2, pp. 429-436. (in hungarian))
http://geogis.detek.unideb.hu/Tkonferencia/2013/GEOINFO_kotet2013.pdf

This code based on Perdro Felzenszwalb source coude.
(Efficient Graph-Based Image Segmentation
Pedro F. Felzenszwalb and Daniel P. Huttenlocher
International Journal of Computer Vision, Volume 59, Number 2, September 2004
http://cs.brown.edu/~pff/segment/segment.zip)

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
#include "image.h"    //2006, Pedro Felzenszwalb
#include "imconv.h"   //2006, Pedro Felzenszwalb
#include "misc.h"     //2006, Pedro Felzenszwalb
#include "pnmfile.h"  //2006, Pedro Felzenszwalb

#include "EtGcSegmentRgb.h"
#include "EtGcSegmentGray.h"
#include "EtTimer.h"


int main(int argc, char **argv) {
	if (argc != 10) {
		fprintf(stderr, "usage: %s sigma k min input(ppm) output(ppm) doPreprocess(0/1) doPostprocess(0/1) sort(s/c/f) rgb(0/1)\n", argv[0]);
		fprintf(stderr, "   s - std::sort\n");
		fprintf(stderr, "   csf - parallel counting sort (round)\n");
		fprintf(stderr, "   csr - parallel counting sort (floor)\n\n");
		return 1;

		//example: 0.5 500 20 d:\testImgs\001_test.ppm d:\out001.ppm 1 1 csr 1 
	}

	EtTimer tmr;

	float sigma = (float)atof(argv[1]);
	float c = (float)atof(argv[2]);
	int minSize = atoi(argv[3]);
	bool preProcess = argv[6][0] == '1';
	bool postProcess = argv[7][0] == '1';
	bool rgbProcess = argv[9][0] == '1';

	image<rgb> *input = loadPPM(argv[4]);
	image<rgb> *res = new image<rgb>( input->width(), input->height());


	if( rgbProcess ){
		tmr.start();

		EtGcSegmentRgb* segmenter = new EtGcSegmentRgb(input->width(), input->height(), preProcess, postProcess, 3 );

		tmr.stop();
		std::cout << input->width() << " " << input->height() << " Inicialization " << tmr.getElapsedTime() << std::endl;

		tmr.start();

		segmenter->segment(input, res, sigma, c, minSize, argv[8]); 

		tmr.stop();
		std::cout << res->width() << " " << res->height() << " Full " << tmr.getElapsedTime() << std::endl;

		savePPM(res, argv[5]);
	}
	else{
		tmr.start();
		EtGcSegmentGray* segmenterG = new EtGcSegmentGray(input->width(), input->height(), preProcess, postProcess, 3 );
		image<uchar>* gray = imageRGBtoGRAY(input);

		tmr.stop();
		std::cout << input->width() << " " << input->height() << " Inicialization " << tmr.getElapsedTime() << std::endl;

		tmr.start();

		segmenterG->segment(gray, res, sigma, c, minSize, argv[8]); 

		tmr.stop();
		std::cout << res->width() << " " << res->height() << " Full " << tmr.getElapsedTime() << std::endl;

		savePPM(res, argv[5]);

	}

	return 0;
}

