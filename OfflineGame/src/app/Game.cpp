#include "Game.h"

#include <gameobjects/Player.h>


Game::Game() : m_Window(nullptr)
{
	Init();
}


Game::~Game()
{
	for (auto it : m_GameObjects)
	{
		delete it;
		it = nullptr;
	}
	m_GameObjects.clear();

	delete m_Window;
	m_Window = nullptr;
}

void Game::Run()
{
	while (m_Window->isOpen())
	{
		sf::Event evnt;
		while (m_Window->pollEvent(evnt))
		{
			if (evnt.type == sf::Event::Closed)
			{
				m_Window->close();
			}
		}

		// Gamelogic
		Update();

		// Draw
		Draw();
	}
}

void Game::Init()
{
	m_Window = new sf::RenderWindow(sf::VideoMode(800, 600), "Offline Shooter Game");

	m_GameObjects.push_back(new Player());
}

void Game::Update()
{
	static sf::Clock clock;
	sf::Time time;
	float dt;

	time = clock.restart();
	dt = time.asSeconds();

	for (auto it : m_GameObjects)
	{
		it->Update(dt);
	}

}

void Game::Draw()
{
	m_Window->clear(sf::Color::Magenta);

	for (auto it : m_GameObjects)
	{
		it->Draw(*m_Window);
	}

	m_Window->display();
}
