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

#ifndef ETPARALLELCOUNTINGSORT_H
#define ETPARALLELCOUNTINGSORT_H

#include <algorithm>
#include <iostream>
#include <omp.h>
#include "EtTimer.h"

//#define weightRange 442999
//#define weightRange 129999

template <class T>
class EtParallelCountingSort{

public:


protected:
	int weightRange;
	int numThreads;
	int* histo;
	int* chisto;

	EtParallelCountingSort( int maxWeight=255, int numThreads = 0 );

	~EtParallelCountingSort();

	int getSlice(int* slice, int n, int numThreads);

	virtual void createHisto(int ah, int fh, T*src, int* histo) = 0;

	void sumHisto(int numThreads, int* histoWithZero, int* histo);

	void commulateLast(int*last, int* chisto);

	void setPositions(int* histo, int* chisto);

	virtual void putElements(int ah, int fh, T* src, T* dest, int* histo) = 0;

	void countingSort(int n, T* src,  T* dest, int* histo, int* chisto);

};

//Size of histo is (numThreads+1)*(weightRange+1)

template <typename T>
void EtParallelCountingSort<T>::countingSort(int n, T* src,  T* dest, int* histo, int* chisto)
{	
	omp_set_num_threads( numThreads );

    int* slice = new int[numThreads + 2]();
	int numSlice = getSlice( slice, n, numThreads );

	//std::cout << "started with range: " << weightRange << "\n";

	#pragma omp parallel for 
	for(int i = 0; i < numThreads; ++i){
		this->createHisto( slice[i], slice[i+1], src, histo + (i+1)*weightRange );
	}
   
	//For all histo element (v in Range):  histo[i][v] += histo[i-1][v]
	sumHisto(numThreads, histo, histo + weightRange);

	//commulate histogram of whole image
	commulateLast( histo + numThreads*weightRange, chisto);

	//add chisto to each separate histo: histo[i][v] += chisto[i][v]
	for(int i = 0; i < numThreads; ++i){
	    setPositions( histo + i*weightRange, chisto);
	}

	//put element to their place
	#pragma omp parallel for 
	for(int i = 0; i < numThreads; ++i){
		this->putElements( slice[i], slice[i+1], src, dest, histo + i*weightRange );
	}

	delete[] slice;
}


template <typename T>
EtParallelCountingSort<T>::EtParallelCountingSort( int maxWeight, int numThreads ){
		if( numThreads == 0)
			numThreads = omp_get_max_threads();

		this->weightRange = maxWeight+1;
		this->numThreads = numThreads;
		this->histo = new int[weightRange*(numThreads+1)];
		this->chisto = new int[weightRange];
}

template <typename T>
EtParallelCountingSort<T>::~EtParallelCountingSort(){
		delete[] histo;
		delete[] chisto;
}


//mindig páros határt ad vissza
template <typename T>
int EtParallelCountingSort<T>::getSlice(int* slice, int n, int numThreads){
	int i;
	int nPerTh = n / numThreads;
	slice[0] = 0;
	for( i = 1; 1; ++i){
		slice[i] += i*nPerTh;
		if( slice[i] >= n )
			break;
	}
	slice[i] = n;
	return i;
}


//[0, range[  [range, 2*range[ ... [(numThreads-1)*range, numThreads*range[
//[0, range[ -- this is the extra part
template <typename T>
void EtParallelCountingSort<T>::sumHisto(int numThreads, int* histoWithZero, int* histo){
	int range = weightRange;
	int fh = range*numThreads;

    #pragma omp parallel for 
    for(int v = 0; v < weightRange; ++v){
	   histoWithZero[v] = 0;
	   for(int i = 0; i < fh; i += range ){
          histo[ v + i ] += histoWithZero[ v + i ];
	   }
	}
}

template <typename T>
void EtParallelCountingSort<T>::commulateLast(int*last, int* chisto){
	chisto[0] = 0;
	for(int i = 0 ; i < weightRange; ++i){
		chisto[i+1] = chisto[i] + last[i];
	}
}

//define start position for each image parts each value
template <typename T>
void EtParallelCountingSort<T>::setPositions(int* histo, int* chisto){
    for(int v = 0; v < weightRange; ++v){
        histo[ v ] += chisto[ v ];
	}	
}



//
//template<typename T>
//void EtParallelCountingSort<T>::createHisto(int ah, int fh, T*src, int* histo){
//	memset( histo, 0, sizeof(int)*weightRange ); 
//	for(int i = ah; i < fh; ++i)
//		++histo[src[i]];
//}


////put the element to their place
//template <typename T>
//void EtParallelCountingSort<T>::putElements(int ah, int fh, T* src, T* dest, int* histo){
//	int v;
//	for(int i = ah; i < fh; ++i){
//		v = src[i];
//		dest[ histo[v]++ ] = v;
//	}
//}

//template <typename T>
//void EtParallelCountingSort<T>::sort(T* src,  T* dest, int n){
//	countingSort<T>( n, src, dest, histo, chisto ); 
//}


#endif