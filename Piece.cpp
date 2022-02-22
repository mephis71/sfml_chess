#include "Piece.h"

Piece::Piece(Color c, Type t, int p, bool m)
{
	color = c;
	type = t;
	pos = p;
	moved = m;
}


Piece::~Piece()
{

}

Color Piece::getCol()
{
	return color;
}

Type Piece::getType()
{
	return type;
}

int Piece::getPos()
{
	return pos;
}

bool Piece::isMoved()
{
	return moved;
}
