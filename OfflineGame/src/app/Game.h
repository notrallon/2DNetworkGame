#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include <gameobjects/GameObject.h>

class Game
{
public:
	Game();
	~Game();

	void						Run();

private:
	void						Init();
	void						Update();
	void						Draw();

	sf::RenderWindow*			m_Window;
	std::vector<GameObject*>	m_GameObjects;
};

