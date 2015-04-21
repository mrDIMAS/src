#include "Precompiled.h"

#include "Utility.h"

void GetD3DMatrixFromBulletTransform ( const btTransform & trn, D3DMATRIX & outMatrix ) {
    btVector3 R = trn.getBasis().getColumn ( 0 );
    btVector3 U = trn.getBasis().getColumn ( 1 );
    btVector3 L = trn.getBasis().getColumn ( 2 );
    btVector3 P = trn.getOrigin();
    outMatrix._11 = R.x();
    outMatrix._12 = R.y();
    outMatrix._13 = R.z();
    outMatrix._14 = 0.0f;
    outMatrix._21 = U.x();
    outMatrix._22 = U.y();
    outMatrix._23 = U.z();
    outMatrix._24 = 0.0f;
    outMatrix._31 = L.x();
    outMatrix._32 = L.y();
    outMatrix._33 = L.z();
    outMatrix._34 = 0.0f;
    outMatrix._41 = P.x();
    outMatrix._42 = P.y();
    outMatrix._43 = P.z();
    outMatrix._44 = 1.0f;
}

float frandom( float low, float high ) {
    return low + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(high-low)));
}

void ParseString ( string str, map<string,string> & values ) {
	if ( str.size() <= 1 ) {
		return;
	}

	values.clear();

	bool equalFound = false;
	bool quoteLF = false, quoteRF = false;
	unsigned int n = 0;

	string varName, var;

	while ( true ) {
		char symbol = str.at ( n );

		if ( symbol == ';' ) {
			quoteLF    = false;
			quoteRF    = false;
			equalFound  = false;

			varName.clear();
			var.clear();
		};

		if ( isalpha ( ( unsigned char ) symbol ) || isdigit ( ( unsigned char ) symbol ) || symbol == '_' ) {
			if ( !equalFound ) {
				varName.push_back ( symbol );
			}
		} else {
			if ( symbol == '=' ) {
				equalFound = true;
			}

			if ( symbol == '"' ) {
				if ( quoteLF == false ) {
					quoteLF = true;
				} else {
					quoteRF = true;
				}
			}
		};

		if ( quoteLF ) {
			if ( quoteRF ) {
				values[ varName ] = var;
			} else {
				if ( symbol != '"' ) {
					var.push_back ( symbol );
				}
			};
		};

		n++;

		if ( n >= str.size() ) {
			break;
		}
	};
}



int NearestPow2( int number )
{
	int power = 1;
	while( power < number ) {
		power *= 2;
		if( power * 2 > number )
			break;
	}
	return power;
}