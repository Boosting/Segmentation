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

#ifndef ETTIMER_H
#define ETTIMER_H

#include "windows.h"


class EtTimer
{
public:
	EtTimer(void);
	~EtTimer(void);

	void start();
	void stop();
	double getElapsedTime();

private:
	LARGE_INTEGER frequency;        // ticks per second
	LARGE_INTEGER startTick, stopTick;    
	double elapsedTime;

};

#endif

