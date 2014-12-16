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


RUAPI ruTimerHandle ruCreateTimer( ) {
    return reinterpret_cast< ruTimerHandle >( new Timer );
}

RUAPI void ruRestartTimer( ruTimerHandle timer ) {
    (reinterpret_cast< Timer* >( timer ))->RestartTimer();
}

RUAPI double ruGetElapsedTimeInSeconds( ruTimerHandle timer ) {
    return (reinterpret_cast< Timer* >( timer ))->GetElapsedTimeInSeconds();
}

RUAPI double ruGetElapsedTimeInMilliSeconds( ruTimerHandle timer ) {
    return (reinterpret_cast< Timer* >( timer ))->GetElapsedTimeInMilliSeconds();
}

RUAPI double ruGetElapsedTimeInMicroSeconds( ruTimerHandle timer ) {
    return (reinterpret_cast< Timer* >( timer ))->GetElapsedTimeInMicroSeconds();
}

RUAPI double ruGetTimeInSeconds( ruTimerHandle timer ) {
    return (reinterpret_cast< Timer* >( timer ))->GetTimeInSeconds();
}

RUAPI double ruGetTimeInMilliSeconds( ruTimerHandle timer ) {
    return (reinterpret_cast< Timer* >( timer ))->GetTimeInMilliSeconds();
}

RUAPI double ruGetTimeInMicroSeconds( ruTimerHandle timer ) {
    return (reinterpret_cast< Timer* >( timer ))->GetTimeInMicroSeconds();
}