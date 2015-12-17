/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2016 Stepanov Dmitriy aka mrDIMAS              *
*                                                                              *
* This file is part of Ruthenium Engine.                                      *
*                                                                              *
* Ruthenium Engine is free software: you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as published by  *
* the Free Software Foundation, either version 3 of the License, or            *
* (at your option) any later version.                                          *
*                                                                              *
* Ruthenium Engine is distributed in the hope that it will be useful,         *
* but WITHOUT ANY WARRANTY; without even the implied warranty of               *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                *
* GNU Lesser General Public License for more details.                          *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with Ruthenium Engine.  If not, see <http://www.gnu.org/licenses/>.   *
*                                                                              *
*******************************************************************************/

#include "Precompiled.h"

#include "Timer.h"

vector<Timer*> Timer::msTimerList;

double Timer::GetElapsedTimeInMicroSeconds() {
    return GetTimeInMicroSeconds() - mLastTime;
}

double Timer::GetElapsedTimeInMilliSeconds() {
    return GetTimeInMilliSeconds() - mLastTime / 1000.0;
}

double Timer::GetElapsedTimeInSeconds() {
    return GetTimeInSeconds() - mLastTime / 1000000.0;
}

double Timer::GetTimeInMicroSeconds() {
    LARGE_INTEGER time;
    QueryPerformanceCounter ( &time );
    return ( double ) ( time.QuadPart * 1000000.0 ) / ( double ) ( mFreq.QuadPart );
}

double Timer::GetTimeInMilliSeconds() {
    LARGE_INTEGER time;
    QueryPerformanceCounter ( &time );
    return ( double ) ( time.QuadPart * 1000.0 ) / ( double ) ( mFreq.QuadPart );
}

double Timer::GetTimeInSeconds() {
    LARGE_INTEGER time;
    QueryPerformanceCounter ( &time );
    return ( double ) ( time.QuadPart ) / ( double ) ( mFreq.QuadPart );
}

void Timer::Restart() {
    mLastTime = GetTimeInMicroSeconds();
}

Timer::Timer() {
    msTimerList.push_back( this );
    QueryPerformanceFrequency ( &mFreq );
    Restart();
}

Timer::~Timer()
{
	msTimerList.erase( find( msTimerList.begin(), msTimerList.end(), this ));
}

ruTimer * ruTimer::Create( ) {
	return new Timer;
}

ruTimer::~ruTimer() {

}