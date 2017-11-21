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
}

void GameServer::RunServer()
{
	m_Running = true;

	static unsigned int IDCounter = 0;

	static sf::Clock clock;
	sf::Time time;
	float elapsed = 0; // for tick rate

	// Tickrate
	float tickrate = 1.0f / 60.0f;

	clock.restart();
	while (m_Running)
	{
		time =		clock.restart();
		float dt =	time.asSeconds();
		elapsed +=	dt;

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
		PlayerInfo info;
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

		PlayerInfo* player;
		
		if (m_Players.find(info.ID) == m_Players.end() && ID == 0)
		{
			IDCounter++;
			info.IP = sender;
			info.Port = port;
			player = CreateNewPlayer(IDCounter, info);
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

		// Send message to clients x times per second
		if (elapsed > tickrate)
		{
			SendUpdateToClients();
			elapsed -= tickrate;
		}
	}
}

void GameServer::DisconnectPlayer(PlayerInfo & info)
{
	for (PlayerMap::iterator it = m_Players.begin(); it != m_Players.end(); it++)
	{
		if (it->second == nullptr)
			continue;

		sf::Packet sendpacket;
		sendpacket << info;
		// Grab a player to send info to
		PlayerInfo* playerToSend = it->second;

		m_Socket.send(sendpacket, playerToSend->IP, playerToSend->Port);
	}
	delete m_Players.find(info.ID)->second;
	m_Players.find(info.ID)->second = nullptr;
}

void GameServer::UpdatePlayerInfo(PlayerInfo & info, PlayerInfo* player)
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

PlayerInfo * GameServer::CreateNewPlayer(unsigned int ID, PlayerInfo& info)
{
	PlayerInfo* player = new PlayerInfo();
	*player = info;
	player->ID = ID;
	player->IP = info.IP;
	player->Port = info.Port;
	player->Speed = 600;
	m_Players.emplace(ID, player);

	// Create a package and send it back to the player as a confirmation
	// that they successfully connected and make sure they get the correct ID.
	sf::Packet confirmationPackage;
	confirmationPackage << *player;
	m_Socket.send(confirmationPackage, player->IP, player->Port);

	return player;
}

void GameServer::SendUpdateToClients()
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
}

sf::Packet & operator<<(sf::Packet & packet, const PlayerInfo & s)
{
	return packet << s.ID << s.Position.x << s.Position.y << s.Direction.x << s.Direction.y << s.Speed << s.IP.toString() << s.Port << s.Connected;
}


sf::Packet & operator>>(sf::Packet & packet, PlayerInfo & s)
{
	return packet >> s.ID >> s.Position.x >> s.Position.y >> s.Direction.x >> s.Direction.y >> s.Speed >> s.IP.toString() >> s.Port >> s.Connected;
}
