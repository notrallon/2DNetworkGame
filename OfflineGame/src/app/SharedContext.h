#pragma once

#include "Game.h"

struct SharedContext
{
	SharedContext() : game(nullptr) {}

	Game* game;
	sf::RenderWindow* window;
};