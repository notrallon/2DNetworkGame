#include <sfml-libs.h>

#include "app/GameServer.h"

int main(int argc, char* argv[])
{
	GameServer server(argc, argv);

	server.RunServer();

	return 0;
}