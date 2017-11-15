#include "Game.h"

#include <gameobjects/Player.h>
#include <app/SharedContext.h>


Game::Game() : m_Window(nullptr), m_Context(nullptr)
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
	m_Context->game = this;
	m_Context->window = m_Window;

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

void Game::AddObject(GameObject* object)
{
	m_GameObjects.push_back(object);
}

void Game::Init()
{
	m_Window = new sf::RenderWindow(sf::VideoMode(800, 600), "Offline Shooter Game");
	m_Context = new SharedContext();

	m_GameObjects.push_back(new Player(m_Context));

}

void Game::Update()
{
	static sf::Clock clock;
	sf::Time time;
	float dt;

	time = clock.restart();
	dt = time.asSeconds();

	size_t size = m_GameObjects.size();

	for (size_t i = 0; i < size; i++)
	{
		m_GameObjects[i]->Update(dt);
	}

}

void Game::Draw()
{
	m_Window->clear(sf::Color::Magenta);

	size_t size = m_GameObjects.size();

	for (size_t i = 0; i < size; i++)
	{
		m_GameObjects[i]->Draw(*m_Window);
	}

	m_Window->display();
}
