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

vector<Timer*> Timer::timers;

double Timer::GetElapsedTimeInMicroSeconds() {
    return GetTimeInMicroSeconds() - lastTime;
}

double Timer::GetElapsedTimeInMilliSeconds() {
    return GetTimeInMilliSeconds() - lastTime / 1000.0;
}

double Timer::GetElapsedTimeInSeconds() {
    return GetTimeInSeconds() - lastTime / 1000000.0;
}

double Timer::GetTimeInMicroSeconds() {
    LARGE_INTEGER time;
    QueryPerformanceCounter ( &time );
    return ( double ) ( time.QuadPart * 1000000.0 ) / ( double ) ( freq.QuadPart );
}

double Timer::GetTimeInMilliSeconds() {
    LARGE_INTEGER time;
    QueryPerformanceCounter ( &time );
    return ( double ) ( time.QuadPart * 1000.0 ) / ( double ) ( freq.QuadPart );
}

double Timer::GetTimeInSeconds() {
    LARGE_INTEGER time;
    QueryPerformanceCounter ( &time );
    return ( double ) ( time.QuadPart ) / ( double ) ( freq.QuadPart );
}

void Timer::RestartTimer() {
    lastTime = GetTimeInMicroSeconds();
}

Timer::Timer() {
    timers.push_back( this );
    QueryPerformanceFrequency ( &freq );
    RestartTimer();
}

//////////////////////////////////////////////////////////////////////////
// API
//////////////////////////////////////////////////////////////////////////


ruTimerHandle ruCreateTimer( ) {
    return reinterpret_cast< ruTimerHandle >( new Timer );
}

void ruRestartTimer( ruTimerHandle timer ) {
    (reinterpret_cast< Timer* >( timer ))->RestartTimer();
}

double ruGetElapsedTimeInSeconds( ruTimerHandle timer ) {
    return (reinterpret_cast< Timer* >( timer ))->GetElapsedTimeInSeconds();
}

double ruGetElapsedTimeInMilliSeconds( ruTimerHandle timer ) {
    return (reinterpret_cast< Timer* >( timer ))->GetElapsedTimeInMilliSeconds();
}

double ruGetElapsedTimeInMicroSeconds( ruTimerHandle timer ) {
    return (reinterpret_cast< Timer* >( timer ))->GetElapsedTimeInMicroSeconds();
}

double ruGetTimeInSeconds( ruTimerHandle timer ) {
    return (reinterpret_cast< Timer* >( timer ))->GetTimeInSeconds();
}

double ruGetTimeInMilliSeconds( ruTimerHandle timer ) {
    return (reinterpret_cast< Timer* >( timer ))->GetTimeInMilliSeconds();
}

double ruGetTimeInMicroSeconds( ruTimerHandle timer ) {
    return (reinterpret_cast< Timer* >( timer ))->GetTimeInMicroSeconds();
}