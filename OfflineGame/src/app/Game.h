#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include <gameobjects/GameObject.h>

struct SharedContext;

class Game
{
public:
	Game();
	~Game();

	void						Run();

	void						AddObject(GameObject* object);

private:
	void						Init();
	void						Update();
	void						Draw();

	sf::RenderWindow*			m_Window;
	std::vector<GameObject*>	m_GameObjects;

	SharedContext*				m_Context;
};

