#include "GameServer.h"
#include <iostream>

GameServer::GameServer()
{
	InitServer();
}

GameServer::~GameServer()
{
}

void GameServer::InitServer()
{
	m_Socket.bind(55002);
	m_Socket.setBlocking(false);
}

void GameServer::RunServer()
{
	m_Running = true;

	static unsigned int IDCounter = 0;

	static sf::Clock clock;
	sf::Time time;
	float elapsed = 0; // for tick rate
	float frametime = 1 / 60;
	std::string c = "c";

	// NOTE: The socket won't wait for a recieve to finnish unless we do this.
	m_Socket.send(c.c_str(), c.length() + 1, "0.0.0.0", 0);
	m_Socket.setBlocking(true);

	clock.restart();
	while (m_Running)
	{
		time = clock.restart();
		float dt = time.asSeconds();
		elapsed += dt;

		sf::Packet recievepacket;
		sf::IpAddress sender;
		unsigned short port;
		// Receive packets

		m_Socket.setBlocking(false);
		sf::Socket::Status socketStatus = m_Socket.receive(recievepacket, sender, port);
		m_Socket.setBlocking(true);
		// Set default id to max unsigned int value
		unsigned int ID = UINT_MAX;
		PlayerInfo info;
		switch (socketStatus)
		{
		case sf::Socket::Done:
			recievepacket >> info;
			ID = info.ID;
			break;
		case sf::Socket::NotReady:
			std::cerr << "Socket is not ready!" << std::endl;
			break;
		case sf::Socket::Error:
			std::cerr << "An unexpected error happened." << std::endl;
			std::cerr << "Port is: " << port << " sender is " << sender.toString() << std::endl;
			continue;
		default:
			break;
		}

		PlayerInfo* player;

		if (m_Players.find(ID) == m_Players.end() && ID != UINT_MAX && ID == 0)
		{
			player = new PlayerInfo();
			IDCounter++;
			*player = info;
			player->ID = IDCounter;
			ID = IDCounter;
			player->IP = sender;
			player->Port = port;
			m_Players.emplace(ID, player);

			// Create a package and send it back to the player as a confirmation
			// that they successfully connected and make sure they get the correct ID.
			sf::Packet confirmationPackage;
			confirmationPackage << *player;
			m_Socket.send(confirmationPackage, player->IP, player->Port);
		}
		else if (ID == UINT_MAX)
		{
			std::cout << "ID is max";
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

		// Set to new player position.
		player->Position = info.Position;
		player->Connected = info.Connected;

		if (!info.Connected)
		{
			for (PlayerMap::iterator it = m_Players.begin(); it != m_Players.end(); it++)
			{
				if (it->second == nullptr)
					continue;
				// Grab a player to send info to
				PlayerInfo* playerToSend = it->second;

				// Loop through all players and send the info to the recieving player.
				for (PlayerMap::iterator it2 = m_Players.begin(); it2 != m_Players.end(); it2++)
				{
					sf::Packet sendpacket;

					if (it2->second == nullptr)
						continue;

					PlayerInfo* pakInfo = it2->second;
					sendpacket << *pakInfo;
					m_Socket.send(sendpacket, playerToSend->IP, playerToSend->Port);
				}
			}
			//delete m_Players.find(ID)->second;
			//m_Players.find(ID)->second = nullptr;
		}

		// Send message to clients x times per second
		if (elapsed > frametime)
		{
			// send to clients

			for (PlayerMap::iterator it = m_Players.begin(); it != m_Players.end(); it++)
			{
				if (it->second == nullptr)
					continue;
				// Grab a player to send info to
				PlayerInfo* playerToSend = it->second;

				// Loop through all players and send the info to the recieving player.
				for (PlayerMap::iterator it2 = m_Players.begin(); it2 != m_Players.end(); it2++)
				{
					sf::Packet sendpacket;
					if (it2->second == nullptr)
						continue;

					PlayerInfo* pakInfo = it2->second;
					sendpacket << *pakInfo;
					m_Socket.send(sendpacket, playerToSend->IP, playerToSend->Port);
				}
			}

			elapsed -= frametime;
		}
	}
}

sf::Packet & operator<<(sf::Packet & packet, const PlayerInfo & s)
{
	return packet << s.ID << s.Position.x << s.Position.y << s.Speed << s.IP.toString() << s.Port << s.Connected;
}


sf::Packet & operator>>(sf::Packet & packet, PlayerInfo & s)
{
	return packet >> s.ID >> s.Position.x >> s.Position.y >> s.Speed >> s.IP.toString() >> s.Port >> s.Connected;
}
