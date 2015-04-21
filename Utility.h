#pragma once



void GetD3DMatrixFromBulletTransform ( const btTransform & trn, D3DMATRIX & outMatrix );
void ParseString( string str, map<string,string> & values);
int NearestPow2( int number );