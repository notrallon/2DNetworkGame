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

	delete m_Context;
	m_Context = nullptr;

	delete m_Window;
	m_Window = nullptr;
}

void Game::Run()
{
	m_Context->game = this;
	m_Context->window = m_Window;

	static sf::Clock clock;
	sf::Time time;
	float elapsed = 0;
	float tickrate = 1 / 60;

	while (m_Window->isOpen())
	{
		time = clock.restart();
		float dt = time.asSeconds();
		elapsed += dt;

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

		// Send Updates
		if (elapsed > tickrate)
		{
			sf::Packet packet;

			packet >> m_Player->GetPlayerInfo();

			m_Socket.send(packet, "10.96.108.73", 55002);
		}

		// Draw
		Draw();

		// Recieve packets
		// Add packets to gameobjects if they don't exist
		// Update all gameobjects
	}
}

void Game::AddObject(GameObject* object)
{
	m_GameObjects.push_back(object);
}

void Game::Init()
{
	m_Socket.bind(sf::Socket::AnyPort);
	m_Window = new sf::RenderWindow(sf::VideoMode(800, 600), "LAN Shooter Game");
	m_Context = new SharedContext();

	m_Player = new Player(m_Context);
	m_GameObjects.push_back(m_Player);

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


sf::Packet & operator<<(sf::Packet& packet, const PlayerInfo& s)
{
	return packet << s.Position.x << s.Position.y << s.Speed << s.IP.toString() << s.Port;
}


sf::Packet & operator>>(sf::Packet& packet, PlayerInfo& s)
{
	return packet >> s.Position.x >> s.Position.y >> s.Speed >> s.IP.toString() >> s.Port;
}
