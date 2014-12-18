#include "Vertex.h"




Vertex::Vertex( const Vertex & v )
{
	pos = v.pos;
	tex = v.tex;
	secondTex = v.secondTex;
	nor = v.nor;
	tan = v.tan;
}

Vertex::Vertex( Vector3 * _pos, Vector2 * _tex, Vector2 * _secondTex, Vector3 normal )
{
	pos = _pos;
	tex = _tex;
	secondTex = _secondTex;
	nor = normal;
}
