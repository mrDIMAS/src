#include "Timer.h"

double Timer::GetElapsedTimeInMicroSeconds()
{
  return GetTimeInMicroSeconds() - lastTime;
}

double Timer::GetElapsedTimeInMilliSeconds()
{
  return GetTimeInMilliSeconds() - lastTime / 1000.0;
}

double Timer::GetElapsedTimeInSeconds()
{
  return GetTimeInSeconds() - lastTime / 1000000.0;
}

double Timer::GetTimeInMicroSeconds()
{
  LARGE_INTEGER time;
  QueryPerformanceCounter ( &time );
  return ( double ) ( time.QuadPart * 1000000.0 ) / ( double ) ( freq.QuadPart );
}

double Timer::GetTimeInMilliSeconds()
{
  LARGE_INTEGER time;
  QueryPerformanceCounter ( &time );
  return ( double ) ( time.QuadPart * 1000.0 ) / ( double ) ( freq.QuadPart );
}

double Timer::GetTimeInSeconds()
{
  LARGE_INTEGER time;
  QueryPerformanceCounter ( &time );
  return ( double ) ( time.QuadPart ) / ( double ) ( freq.QuadPart );
}

void Timer::RestartTimer()
{
  lastTime = GetTimeInMicroSeconds();
}

Timer::Timer()
{
  QueryPerformanceFrequency ( &freq );
  RestartTimer();
}

//////////////////////////////////////////////////////////////////////////
// API
//////////////////////////////////////////////////////////////////////////


API TimerHandle CreateTimer( )
{
  return reinterpret_cast< TimerHandle >( new Timer );
}

API void RestartTimer( TimerHandle timer )
{
  (reinterpret_cast< Timer* >( timer ))->RestartTimer();
}

API double GetElapsedTimeInSeconds( TimerHandle timer )
{
  return (reinterpret_cast< Timer* >( timer ))->GetElapsedTimeInSeconds();
}

API double GetElapsedTimeInMilliSeconds( TimerHandle timer )
{
  return (reinterpret_cast< Timer* >( timer ))->GetElapsedTimeInMilliSeconds();
}

API double GetElapsedTimeInMicroSeconds( TimerHandle timer )
{
  return (reinterpret_cast< Timer* >( timer ))->GetElapsedTimeInMicroSeconds();
}
