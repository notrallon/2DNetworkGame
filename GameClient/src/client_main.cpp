#include <sfml-libs.h>

#include "app/Game.h"

/*
	This is the game client.
*/


int main(int argc, char* argv[])
{
	if (argc >= 2)
	{
		// Treat arg 2 as IP-address
		std::string ip_address = argv[1];
	}

	Game game;
	game.Run();

	return 0;
}