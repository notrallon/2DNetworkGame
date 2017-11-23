#include "GameServer.h"
#include <iostream>
#include <thread>
#include <chrono>

GameServer::GameServer()
{
	InitServer();
}

GameServer::GameServer(int argc, char * argv[])
{
	if (argc >= 2)
	{
		// Treat arg 2 as IP-address
		m_Port = std::atoi(argv[1]);
	}
	InitServer();
}

GameServer::~GameServer()
{
}

void GameServer::InitServer()
{
	std::cout << "Local IP-adress: "	<<	sf::IpAddress::getLocalAddress()	<< std::endl;
	std::cout << "Public IP-adress: "	<<	sf::IpAddress::getPublicAddress()	<< std::endl;

	if (m_Port > 5 && m_Port < 70000)
	{
		m_Socket.bind(m_Port);
		std::cout << "Port: " << m_Port << std::endl;
	}
	else
	{
		m_Socket.bind(55002);
		std::cout << "Port: " << "55002" << std::endl;
	}

	
}

void GameServer::RunServer()
{
	m_Running = true;

	static unsigned int IDCounter = 0;

	static sf::Clock clock;
	static sf::Time time;
	float elapsed = 0; // for tick rate

	// Tickrate
	const static float SEND_TICKRATE = 1.0f / 60.0f;
	const static float UPDATE_RATE = 1.0f / 500.0f;

	clock.restart();
	while (m_Running)
	{
		time =		clock.restart();
		float dt =	time.asSeconds();
		elapsed +=	dt;

		UpdateObjects(dt);

		// Receive holders
		sf::Packet		recievepacket;
		sf::IpAddress	sender;
		unsigned short	port;
		// Receive packets
		m_Socket.setBlocking(false);
		sf::Socket::Status socketStatus = m_Socket.receive(recievepacket, sender, port);
		m_Socket.setBlocking(true);
		// Set default id to max unsigned int value
		unsigned int ID = UINT_MAX;
		ObjectInfo info;
		switch (socketStatus)
		{
		case sf::Socket::Done:
			recievepacket >> info;
			ID = info.ID;
			break;
		case sf::Socket::NotReady:
			//std::cerr << "Socket is not ready!" << std::endl;
			break;
		case sf::Socket::Error:
			std::cerr << "An unexpected error happened." << std::endl;
			std::cerr << "Port is: " << port << " sender is " << sender.toString() << std::endl;
			continue;
		default:
			break;
		}

		ObjectInfo* player = nullptr;
		
		if (ID == 0)
		{
			if (m_Players.find(info.ID) == m_Players.end())
			{
				IDCounter++;
				info.IP = sender;
				info.Port = port;
				player = CreateNewPlayer(IDCounter, info);
			}
		}
		else if (ID == UINT_MAX)
		{
			//std::cout << "ID is max";
			continue;
		}
		else if (m_Players.find(ID)->second == nullptr)
		{
			continue;
		}
		else
		{
			player = m_Players.find(ID)->second;
		}

		UpdatePlayerInfo(info, player);
		
		if (info.Shooting)
		{
			IDCounter++;
			SpawnProjectile(info, IDCounter);
		}

		// Send message to clients x times per second
		if (elapsed > SEND_TICKRATE)
		{
			SendUpdateToClients();
			elapsed -= SEND_TICKRATE;
		}

		sf::Time t2 = clock.getElapsedTime();
		while (t2.asSeconds() < UPDATE_RATE)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			t2 = clock.getElapsedTime();
		}
	}
}

void GameServer::DisconnectPlayer(ObjectInfo & info)
{

	//TODO: Calling upon disconnect (on client side) causes server to crash. Probably something wrong with client side having no player but still sends info, but server should handle this.
	PlayerMap::iterator it;
	for (PlayerMap::iterator it = m_Players.begin(); it != m_Players.end(); it++)
	{
		if (it->second == nullptr || it->second->ObjectType != ObjectInfo::ObjectTypes::Player)
			continue;

		sf::Packet sendpacket;
		sendpacket << info;
		// Grab a player to send info to
		ObjectInfo* playerToSend = it->second;

		m_Socket.send(sendpacket, playerToSend->IP, playerToSend->Port);
	}
	PlayerMap::iterator obj = m_Players.find(info.ID);
	delete obj->second;
	obj->second = nullptr;
	m_Players.erase(obj);



	int hello = 1;
}

void GameServer::UpdatePlayerInfo(ObjectInfo & info, ObjectInfo* player)
{
	if (player == nullptr) { return; }
	static sf::Clock runningClock;

	// Set to new player position.
	float playerDT = runningClock.getElapsedTime().asSeconds() - player->LastPing.asSeconds();

	float length = sqrt(info.Direction.x * info.Direction.x + info.Direction.y * info.Direction.y);

	if (length != 0)
	{
		info.Direction.x /= length;
		info.Direction.y /= length;
	}

	player->Position += info.Direction * player->Speed * playerDT;
	player->Connected = info.Connected;
	player->LastPing = runningClock.getElapsedTime();

	if (!info.Connected)
	{
		DisconnectPlayer(info);
	}
}

ObjectInfo * GameServer::CreateNewPlayer(unsigned int ID, ObjectInfo& info)
{
	ObjectInfo* player = new ObjectInfo();
	*player = info;
	player->ID = ID;
	player->IP = info.IP;
	player->Port = info.Port;
	player->Speed = 600;
	player->ObjectType = ObjectInfo::ObjectTypes::Player;
	m_Players.emplace(ID, player);

	// Create a package and send it back to the player as a confirmation
	// that they successfully connected and make sure they get the correct ID.
	sf::Packet confirmationPackage;
	confirmationPackage << *player;
	m_Socket.send(confirmationPackage, player->IP, player->Port);

	return player;
}

void GameServer::SpawnProjectile(ObjectInfo & info, unsigned int& ID)
{
	//ID++;
	// Hitta spelaren som skjuter
	ObjectInfo* player = m_Players.find(info.ID)->second;

	// Skapa ny projectile och lägg till bland objects
	ObjectInfo* projectile = new ObjectInfo();
	projectile->ObjectType = ObjectInfo::ObjectTypes::Projectile;
	projectile->Speed = 100;
	projectile->Connected = true;
	projectile->ID = ID;
	projectile->Position = player->Position;
	m_Players.emplace(ID, projectile);

	// Skapa en vektor utifrån var spelaren siktar och normalisera den
	sf::Vector2f dir = info.MousePosition - player->Position;
	float dirLength = sqrt(dir.x * dir.x + dir.y * dir.y);
	dir.x /= dirLength;
	dir.y /= dirLength;

	projectile->Direction = dir;
}

void GameServer::UpdateObjects(const float & dt)
{
	PlayerMap destroy;
	for (PlayerMap::iterator it = m_Players.begin(); it != m_Players.end(); it++)
	{
		if (it->second == nullptr)
		{
			//it++;
			continue;
		}
		ObjectInfo* obj = it->second;


		if (obj->ObjectType != ObjectInfo::ObjectTypes::Player)
		{
			obj->Position += obj->Direction * obj->Speed * dt;

			if (!obj->Connected)
			{
				destroy.emplace(obj->ID, obj);
				continue;
			}

			//out of bounds x
			if (obj->Position.x > 2000 || obj->Position.x < -2000)
			{
				obj->Connected = false;
				//it++;
				continue;
			}
			//out of bounds y
			else if (obj->Position.y > 2000 || obj->Position.y < -2000)
			{
				obj->Connected = false;
				//it++;
				continue;
			}
		}
		
	}

	for (auto it : destroy)
	{
		DisconnectPlayer(*it.second);
	}
	int done = 1;
}

void GameServer::SendUpdateToClients()
{
	// send to clients
	for (PlayerMap::iterator it = m_Players.begin(); it != m_Players.end(); it++)
	{
		if (it->second == nullptr || it->second->ObjectType == ObjectInfo::ObjectTypes::Projectile)
			continue;

		// Grab a player to send info to
		ObjectInfo* playerToSend = it->second;

		// Loop through all players and send the info to the recieving player.
		for (PlayerMap::iterator it2 = m_Players.begin(); it2 != m_Players.end(); it2++)
		{
			sf::Packet sendpacket;
			if (it2->second == nullptr)
				continue;

			ObjectInfo* pakInfo = it2->second;
			sendpacket << *pakInfo;
			m_Socket.send(sendpacket, playerToSend->IP, playerToSend->Port);
		}
	}
}

sf::Packet & operator<<(sf::Packet & packet, const ObjectInfo & s)
{
	return packet << s.ID << s.Position.x << s.Position.y << s.Direction.x << s.Direction.y << s.Speed << s.IP.toString() << s.Port << s.Connected << s.ObjectType << s.SocketType;
}

sf::Packet & operator>>(sf::Packet & packet, ObjectInfo & s)
{
	return packet >> s.ID >> s.Position.x >> s.Position.y >> s.Direction.x >> s.Direction.y >> s.Speed >> s.IP.toString() >> s.Port >> s.Connected >> s.Shooting >> s.MousePosition.x >> s.MousePosition.y;
}
