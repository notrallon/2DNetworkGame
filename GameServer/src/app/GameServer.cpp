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

	std::cout << "ObjectInfo: " << sizeof(ObjectInfo) << " bytes\n";

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
			if (m_GameObjects.find(info.ID) == m_GameObjects.end())
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
		else if (m_GameObjects.find(ID)->second == nullptr)
		{
			continue;
		}
		else
		{
			player = m_GameObjects.find(ID)->second;
		}

		if (info.Shooting)
		{
			long shotDT = RUNNING_CLOCK.getElapsedTime().asMilliseconds() - player->LastShot.asMilliseconds();
			if (shotDT > 500)
			{
				IDCounter++;
				SpawnProjectile(info, IDCounter);
				player->LastShot = RUNNING_CLOCK.getElapsedTime();
			}
		}
		
		UpdatePlayerInfo(info, player);
		
		// Send message to clients x times per second
		if (elapsed > SEND_TICKRATE)
		{
			SendUpdateToClients();
			elapsed -= SEND_TICKRATE;
		}

		// Limit updates per second.
		sf::Time t2 = clock.getElapsedTime();
		while (t2.asSeconds() < UPDATE_RATE)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(long long((UPDATE_RATE * 1000) - t2.asMilliseconds())));
			t2 = clock.getElapsedTime();
		}
	}
}

void GameServer::DisconnectPlayer(ObjectInfo & info)
{
	ObjectMap::iterator it;
	for (ObjectMap::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		if (it->second == nullptr || it->second->ObjectType != ObjectInfo::ObjectTypes::Player)
			continue;

		sf::Packet sendpacket;
		sendpacket << info;
		// Grab a player to send info to
		ObjectInfo* playerToSend = it->second;

		m_Socket.send(sendpacket, playerToSend->IP, playerToSend->Port);
	}

	// Delete the object.
	ObjectMap::iterator obj = m_GameObjects.find(info.ID);
	delete obj->second;
	obj->second = nullptr;
	m_GameObjects.erase(obj);
}

void GameServer::UpdatePlayerInfo(ObjectInfo & info, ObjectInfo* player)
{
	if (player == nullptr) { return; }

	// Set to new player position.
	float playerDT = RUNNING_CLOCK.getElapsedTime().asSeconds() - player->LastPing.asSeconds();

	float length = sqrt(info.Direction.x * info.Direction.x + info.Direction.y * info.Direction.y);

	if (length != 0)
	{
		info.Direction.x /= length;
		info.Direction.y /= length;
	}
	player->Direction = info.Direction;
	player->Shooting = info.Shooting;
	player->Connected = info.Connected;
	player->LastPing = RUNNING_CLOCK.getElapsedTime();

	if (!info.Connected)
	{
		DisconnectPlayer(info);
	}
}

ObjectInfo* GameServer::CreateNewPlayer(unsigned int ID, ObjectInfo& info)
{
	ObjectInfo* player = new ObjectInfo();
	*player = info;
	player->ID = ID;
	player->IP = info.IP;
	player->Port = info.Port;
	player->Speed = 600;
	player->ObjectType = ObjectInfo::ObjectTypes::Player;
	player->Size = sf::Vector2f(40, 50);
	player->LastShot = RUNNING_CLOCK.getElapsedTime();
	m_GameObjects.emplace(ID, player);

	sf::FloatRect floatRect = sf::FloatRect(player->Position, player->Size);

	m_PlayerAreas.emplace(ID, floatRect);

	// Create a package and send it back to the player as a confirmation
	// that they successfully connected and make sure they get the correct ID.
	sf::Packet confirmationPackage;
	confirmationPackage << *player;
	m_Socket.send(confirmationPackage, player->IP, player->Port);

	return player;
}

void GameServer::SpawnProjectile(ObjectInfo & info, unsigned int& ID)
{
	// Find the player that is shooting.
	ObjectInfo* player = m_GameObjects.find(info.ID)->second;

	// Create a new projectile and add it to our objects.
	ObjectInfo* projectile = new ObjectInfo();

	// Get the directional vector to where the player is aiming.
	sf::Vector2f dir = info.MousePosition - player->Position;
	float dirLength = sqrt(dir.x * dir.x + dir.y * dir.y);
	dir.x /= dirLength;
	dir.y /= dirLength;

	projectile->Direction = dir;

	projectile->ObjectType = ObjectInfo::ObjectTypes::Projectile;
	projectile->Speed = 1000;
	projectile->Connected = true;
	projectile->ID = ID;
	projectile->Position = sf::Vector2f(player->Position.x + (dir.x * 50), player->Position.y + (dir.y * 60));
	projectile->Size = sf::Vector2f(12,12);
	m_GameObjects.emplace(ID, projectile);

	sf::FloatRect floatRect = sf::FloatRect(projectile->Position, projectile->Size);
	m_BulletAreas.emplace(ID, floatRect);
}

void GameServer::UpdateObjects(const float & dt)
{
	// Contains objects that was set to be disconnected last update 
	// and makes sure they are destroyed.
	ObjectMap destroy;
	for (ObjectMap::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		if (it->second == nullptr)
			continue;

		ObjectInfo* obj = it->second;

		// Update server side objects.
		if (!obj->Connected)
		{
			destroy.emplace(obj->ID, obj);
			continue;
		}

		/// Collision Handling (players getting hit by bullets)
		// if obj is a player
		if (obj->ObjectType == ObjectInfo::ObjectTypes::Player)
		{
			for (ObjectMap::iterator it2 = m_GameObjects.begin(); it2 != m_GameObjects.end(); it2++)
			{
				ObjectInfo* objBullet = it2->second;

				//if objBullet is not a projectile
				if (objBullet->ObjectType != ObjectInfo::ObjectTypes::Projectile)
					continue;
				//if objBullets ID is the same as the first (it) iteration
				if (objBullet->ID == obj->ID)
					continue;
				// if player collides with bullet
				if (m_PlayerAreas.find(obj->ID)->second.intersects(m_BulletAreas.find(objBullet->ID)->second))
				{
					std::cout << "Hit!" << std::endl;

					// Disconnect the player that was hit
                    obj->Connected = false;
				}
			}
		}// Collision handling End.

		if (obj->ObjectType != ObjectInfo::ObjectTypes::Player)
		{
			obj->Position += obj->Direction * obj->Speed * dt;
			sf::FloatRect floatRect = sf::FloatRect(obj->Position, obj->Size);

			
			//Why this no work?
			m_BulletAreas.find(obj->ID)->second = floatRect;


			//out of bounds x
			if (obj->Position.x > 2000 || obj->Position.x < -2000)
			{
				obj->Connected = false;
				continue;
			}
			//out of bounds y
			else if (obj->Position.y > 2000 || obj->Position.y < -2000)
			{
				obj->Connected = false;
				continue;
			}
		}
		else
		{
			long playerDT = RUNNING_CLOCK.getElapsedTime().asMilliseconds() - obj->LastPing.asMilliseconds();
			obj->Position += obj->Direction * obj->Speed * dt;

			sf::FloatRect floatRect = sf::FloatRect(obj->Position, obj->Size);
			m_PlayerAreas.find(obj->ID)->second = floatRect;

			if (playerDT > 10000)
				obj->Connected = false;
		}
	}

	for (auto it : destroy)
	{
		//Also disconnects bullets
		DisconnectPlayer(*it.second);
	}
}

void GameServer::SendUpdateToClients()
{
	// send to clients
	for (ObjectMap::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		// Don't send updates to something that's not a player.
		if (it->second == nullptr || it->second->ObjectType != ObjectInfo::ObjectTypes::Player)
			continue;

		// Grab a player to send info to
		ObjectInfo* playerToSend = it->second;

		// Loop through all players and send the info to the recieving player.
		for (ObjectMap::iterator it2 = m_GameObjects.begin(); it2 != m_GameObjects.end(); it2++)
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
	return packet << s.ID << s.Position.x << s.Position.y << s.Size.x << s.Size.y << s.Direction.x << s.Direction.y << s.Speed << s.IP.toString() << s.Port << s.Connected << s.ObjectType << s.SocketType << s.Shooting;
}

sf::Packet & operator>>(sf::Packet & packet, ObjectInfo & s)
{
	return packet >> s.ID >> s.Position.x >> s.Position.y >> s.Size.x >> s.Size.y >> s.Direction.x >> s.Direction.y >> s.Speed >> s.IP.toString() >> s.Port >> s.Connected >> s.Shooting >> s.MousePosition.x >> s.MousePosition.y;
}
