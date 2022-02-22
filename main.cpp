#include "Game.h"

//todo rook moving pattern
int main()
{
	Game game;

	while (game.isRunning())
	{
		game.update();
		game.render();
	}

	return 0;

}