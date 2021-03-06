/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2017 Stepanov Dmitriy aka mrDIMAS              *
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

#pragma once

class Timer : public ITimer {
private:
	LARGE_INTEGER mFreq;
	double mLastTime;
public:
	Timer( );
	~Timer( );
	virtual void Restart( ) override final;
	virtual double GetTimeInSeconds( ) override final;
	virtual double GetTimeInMilliSeconds( ) override final;
	virtual double GetTimeInMicroSeconds( ) override final;
	virtual double GetElapsedTimeInSeconds( ) override final;
	virtual double GetElapsedTimeInMilliSeconds( ) override final;
	virtual double GetElapsedTimeInMicroSeconds( ) override final;
};