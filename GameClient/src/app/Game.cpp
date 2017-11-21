#include "Game.h"
#include <iostream>
#include "gameobjects/Player.h"
#include "app/SharedContext.h"

Game::Game() : m_Window(nullptr), m_Context(nullptr)
{
	Init();
}

Game::Game(int argc, char * argv[]) : m_Window(nullptr), m_Context(nullptr)
{
	if (argc >= 2)
	{
		// Treat arg 2 as IP-address
		m_ServerIP = argv[1];
	}
	Init();
}

Game::~Game()
{
	// Send a packet to the server where info says the player is no longer
	// connected.



	for (auto it : m_GameObjects)
	{
		delete it.second;
		it.second = nullptr;
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
	float tickrate = 1.0f / 60.0f;

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
				PlayerInfo info = m_Player->GetPlayerInfo();
				info.Connected = false;
				m_Player->SetPlayerInfo(info);
				sf::Packet packet;
				packet << info;
				m_Socket.send(packet, m_ServerIP, 55002);

				m_Window->close();
			}
		}

		// Gamelogic
		Update();
		// Draw
		Draw();

		

		// Add packets to gameobjects if they don't exist
		// Update all gameobjects
	}
}

void Game::AddObject(GameObject* object)
{
	//m_GameObjects.push_back(object);
}

void Game::Init()
{
	m_Socket.bind(sf::Socket::AnyPort);
	m_Socket.setBlocking(true);
	m_Window = new sf::RenderWindow(sf::VideoMode(800, 600), "LAN Shooter Game");
	m_Context = new SharedContext();

	//m_Player = new Player(m_Context, true);

	
	CreateClientPlayer();
	// Recieve a message that updates the player info.
	//Recieve();


}

void Game::Update()
{
	static sf::Clock clock;
	static float elapsed = 0;
	static float tickrate = 1.0f / 120.0f;
	sf::Time time;
	float dt;

	time = clock.restart();
	dt = time.asSeconds();
	elapsed += dt;

	// Send Updates
	if (elapsed > tickrate)
	{
		Send();
		elapsed -= tickrate;
	}
	// Recieve packets
	Recieve();


	for (ObjectMap::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		if (it->second != nullptr)
		{
			it->second->Update(dt);
		}
	}

}

void Game::Draw()
{
	m_Window->clear(sf::Color::Magenta);

	for (ObjectMap::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		it->second->Draw(*m_Window);
	}

	m_Window->display();
}

void Game::Send()
{
	sf::Packet packet;
	PlayerInfo info = m_Player->GetPlayerInfo();
	info.Port = m_Socket.getLocalPort();
	packet << info;

	sf::Socket::Status status = m_Socket.send(packet, m_ServerIP, 55002);

	switch (status)
	{
	case sf::Socket::Done:
		break;
	case sf::Socket::NotReady:
		std::cout << "Socket NotReady!" << std::endl;
		break;
	case sf::Socket::Partial:
		std::cout << "Socket Partial!" << std::endl;
		break;
	case sf::Socket::Disconnected:
		std::cout << "Socket Disconnected!" << std::endl;
		break;
	case sf::Socket::Error:
		std::cout << "Socket Error!" << std::endl;
		break;
	default:
		break;
	}
}

void Game::Recieve()
{
	sf::Packet recievePak;
	sf::IpAddress recieveIP;
	unsigned short recievePort;
	m_Socket.setBlocking(false);
	sf::Clock clock;
	sf::Time time = clock.restart();
	sf::Socket::Status recieveStatus;
	recieveStatus = sf::Socket::NotReady;
	recieveStatus = m_Socket.receive(recievePak, recieveIP, recievePort);
	m_Socket.setBlocking(true);

	PlayerInfo recieveInfo;
	recievePak >> recieveInfo;

	switch (recieveStatus)
	{
	case sf::Socket::Done: {
		Player* player;
		if (m_GameObjects.find(recieveInfo.ID) == m_GameObjects.end())
		{
			player = new Player(m_Context, false);
			player->SetPlayerInfo(recieveInfo);
			m_GameObjects.emplace(recieveInfo.ID, player);
		}
		// Get an already existing player
		Player* object = static_cast<Player*>(m_GameObjects.find(recieveInfo.ID)->second);
		
		// Update the players info.
		object->SetPlayerInfo(recieveInfo);
		
		// If another player disconnected.
		if (!recieveInfo.Connected)
		{
			delete m_GameObjects.find(recieveInfo.ID)->second;
			m_GameObjects.find(recieveInfo.ID)->second = nullptr;
			m_GameObjects.erase(recieveInfo.ID);
		}
	} break;
	case sf::Socket::NotReady:
		break;
	case sf::Socket::Partial:
		break;
	case sf::Socket::Disconnected:
		break;
	case sf::Socket::Error:
		break;
	default:
		break;
	}
}

void Game::CreateClientPlayer()
{
	m_Player = new Player(m_Context, true);

	Send();

	sf::Packet recievePak;
	sf::IpAddress recieveIP;
	unsigned short recievePort;
	m_Socket.setBlocking(false);
	sf::Socket::Status recieveStatus = m_Socket.receive(recievePak, recieveIP, recievePort);

	while (recieveStatus != sf::Socket::Status::Done)
	{
		recieveStatus = m_Socket.receive(recievePak, recieveIP, recievePort);
	}

	m_Socket.setBlocking(true);
	PlayerInfo recieveInfo;
	recievePak >> recieveInfo;

	switch (recieveStatus)
	{
	case sf::Socket::Done: 
		m_Player->SetPlayerInfo(recieveInfo);
		m_GameObjects.emplace(m_Player->GetPlayerInfo().ID, m_Player);
		break;
	case sf::Socket::NotReady:
		break;
	case sf::Socket::Partial:
		break;
	case sf::Socket::Disconnected:
		break;
	case sf::Socket::Error:
		break;
	default:
		break;
	}
}


sf::Packet & operator<<(sf::Packet& packet, const PlayerInfo& s)
{
	return packet << s.ID << s.Position.x << s.Position.y << s.Direction.x << s.Direction.y << s.Speed << s.IP.toString() << s.Port << s.Connected;
}


sf::Packet & operator>>(sf::Packet& packet, PlayerInfo& s)
{
	return packet >> s.ID >> s.Position.x >> s.Position.y >> s.Direction.x >> s.Direction.y >> s.Speed >> s.IP.toString() >> s.Port >> s.Connected;
}
