#pragma once
#include <SFML/Graphics.hpp>
enum Color
{
	White = 0,
	Black = 1
};
enum Type
{
	Pawn = 0,
	Knight = 1,
	Bishop = 2,
	Rook = 3,
	King = 4,
	Queen = 5
};

class Piece : public sf::Sprite
{
private:
	Type type;
	Color color;
	int pos;
	bool moved;
	
public:
	Piece(Color c, Type t, int p, bool m);
	~Piece();
	Color getCol();
	Type getType();
	int getPos();
	bool isMoved();
	
};