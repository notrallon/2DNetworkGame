#include <sfml-libs.h>

#include "app/Game.h"

/*
	This is the game client.
*/


int main(int argc, char* argv[])
{
	Game game(argc, argv);
	game.Run();

	return 0;
}