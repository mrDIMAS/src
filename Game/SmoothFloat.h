#pragma once

#include <math.h>
#include <limits.h>
#include "TextFileStream.h"

#undef min
#undef max 

// Плавный флоат, нужен для плавных изменений чисел во времени, задается минимальное и максимальное значение
// текущее значение обрезается по этому промежутку
class SmoothFloat
{
protected:
    float value;
    float min;
    float max;
    float chase;
public:
    explicit SmoothFloat( );
    explicit SmoothFloat( float newValue );
    explicit SmoothFloat( float newValue, float minValue, float maxValue );
    void Set( float newValue );
    void SetMin( float minValue );
    void SetMax( float maxValue );
    float GetMin( );
    float GetMax();
    bool IsReachEnd();
    bool IsReachBegin();
    void SetTarget( float chaseValue );
    float GetTarget( );    
    float ChaseTarget( float chaseSpeed = 0.01f );
    operator const float();
    void operator = ( float f ) {
        value = f;
    }
    void Serialize( TextFileStream & out );    
    void Deserialize( TextFileStream & in );
};

// not done yet
/*
class SmoothFloatInterp : public SmoothFloat
{
private:
    float interp;
    char currentIntpLaw;
public:

    float DoLinearInterpolation( float interpolationStep = 0.01f ) {
        currentIntpLaw = 1;
        value = min + ( max - min ) * interp;
        if( interp < 1.0f ){
            interp += interpolationStep;
        } else {        
            interp = 1.0f;
        }
        return value;
    }

    float DoLogarithmicInterpolation( float interpolationStep = 0.01f ) {
        currentIntpLaw = 2;
        value = min + ( max - min ) * log( 1.0f - interp );
        if( interp < 0.9f ){
            interp += interpolationStep;
        } else {        
            interp = 0.9f;
        }
        return value;
    }

    void Reset() {
        interp = 0.0f;
    }

    /*
    if( currentIntpLaw == 1 ) { //linear
    interp = ( value - max ) / ( min - max );
    } else if( currentIntpLaw == 2 ) { // logarithmic
    interp = 1.0f - exp( ( value - max ) / ( min - max ));
    }
    
};*/