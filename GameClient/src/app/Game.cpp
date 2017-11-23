#include "Game.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "gameobjects/Player.h"
#include "gameobjects/Projectile.h"
#include "app/SharedContext.h"

sf::RectangleShape Game::groundFloor;

Game::Game() : m_Window(nullptr), m_Context(nullptr)
{
	Init();
}

Game::Game(int argc, char * argv[]) : m_Window(nullptr), m_Context(nullptr)
{
	if (argc == 2)
	{
		// Treat arg 2 as IP-address
		m_ServerIP = argv[1];
		std::cout << "ServerIP: " << m_ServerIP << std::endl;

	}
	else if (argc >= 3)
	{
		// Treat arg 2 as IP-address
		m_ServerIP = argv[1];
		std::cout << "ServerIP: " << m_ServerIP << std::endl;
		// Treat arg 3 as Port
		m_ServerPort = std::atoi(argv[2]);
		std::cout << "ServerPort: " << m_ServerPort << std::endl;
	}
	Init();
}

Game::~Game()
{
	// Send a packet to the server where info says the player is no longer
	// connected.

	Disconnect();

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

int Game::Run()
{
	m_Context->game = this;
	m_Context->window = m_Window;

	//m_Window->setFramerateLimit(200);

	static sf::Clock clock;
	sf::Time time;
	float elapsed = 0;
	const float TICKRATE = 1000.0f / 300.0f;

	while (m_Window->isOpen())
	{
		//time = clock.getElapsedTime();
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

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Return))
			{
				m_Window->close();
				return 1;
			}
		}

		// Gamelogic
		Update();
		// Draw
		Draw();

		// TODO: Breaks net-update, fix to not race for CPU
		/*sf::Time t2 = clock.getElapsedTime();
		while (t2.asMilliseconds() < TICKRATE)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(long long(TICKRATE - t2.asMilliseconds())));
			t2 = clock.getElapsedTime();
		}*/

		// Add packets to gameobjects if they don't exist
		// Update all gameobjects
	}

	return 0;
}

void Game::AddObject(GameObject* object)
{
	//m_GameObjects.push_back(object);
}

void Game::Init()
{
	m_Socket.bind(sf::Socket::AnyPort);
	m_Socket.setBlocking(true);
	m_Window = new sf::RenderWindow(sf::VideoMode(1600, 900), "LAN Shooter Game");
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
	const static float TICKRATE = 1.0f / 60.0f;
	sf::Time time;
	float dt;

	time = clock.restart();
	dt = time.asSeconds();
	elapsed += dt;

	// Send Updates to server
	if (elapsed > TICKRATE)
	{
		Send();
		elapsed -= TICKRATE;
	}
	// Recieve packets from server
	Recieve();

	//Temp Code
	
	groundFloor.setSize(sf::Vector2f(800, 100));
	groundFloor.setPosition(0, 500);
	groundFloor.setFillColor(sf::Color::Green);
	

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
	m_Window->draw(groundFloor);

	m_Window->display();
}

void Game::Send()
{
	sf::Packet packet;
	ObjectInfo info = m_Player->GetObjectInfo();
	info.Port = m_Socket.getLocalPort();
	packet << info;

	sf::Socket::Status status = m_Socket.send(packet, m_ServerIP, m_ServerPort);

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
	sf::Socket::Status recieveStatus;
	recieveStatus = sf::Socket::NotReady;
	recieveStatus = m_Socket.receive(recievePak, recieveIP, recievePort);
	m_Socket.setBlocking(true);

	ObjectInfo recieveInfo;
	recievePak >> recieveInfo;

	switch (recieveStatus)
	{
	case sf::Socket::Done: {
		GameObject* object;
		if (m_GameObjects.find(recieveInfo.ID) == m_GameObjects.end())
		{
			if (recieveInfo.ObjectType == ObjectInfo::ObjectTypes::Player)
			{
				object = static_cast<Player*>(new Player(m_Context, false));
				object->SetObjectInfo(recieveInfo);
				m_GameObjects.emplace(recieveInfo.ID, object);
			}
			else if (recieveInfo.ObjectType == ObjectInfo::ObjectTypes::Projectile)
			{
				object = static_cast<Projectile*>(new Projectile(recieveInfo.Position, recieveInfo.Direction));
				object->SetObjectInfo(recieveInfo);
				m_GameObjects.emplace(recieveInfo.ID, object);
			}
		}
		// Get an already existing player
		object = m_GameObjects.find(recieveInfo.ID)->second;
		
		// Update the players info.
		object->SetObjectInfo(recieveInfo);
		
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
	ObjectInfo recieveInfo;
	recievePak >> recieveInfo;

	switch (recieveStatus)
	{
	case sf::Socket::Done: 
		m_Player->SetObjectInfo(recieveInfo);
		m_GameObjects.emplace(m_Player->GetObjectInfo().ID, m_Player);
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

void Game::Disconnect()
{
	ObjectInfo info = m_Player->GetObjectInfo();
	info.Connected = false;
	m_Player->SetObjectInfo(info);
	sf::Packet packet;
	packet << info;
	m_Socket.send(packet, m_ServerIP, m_ServerPort);
}

sf::Packet & operator<<(sf::Packet& packet, const ObjectInfo& s)
{
	return packet << s.ID << s.Position.x << s.Position.y << s.Direction.x << s.Direction.y << s.Speed << s.IP.toString() << s.Port << s.Connected << s.Shooting << s.MousePosition.x << s.MousePosition.y << s.ObjectType << s.SocketType;
}

sf::Packet & operator>>(sf::Packet& packet, ObjectInfo& s)
{
	return packet >> s.ID >> s.Position.x >> s.Position.y >> s.Direction.x >> s.Direction.y >> s.Speed >> s.IP.toString() >> s.Port >> s.Connected >> s.ObjectType;
}
