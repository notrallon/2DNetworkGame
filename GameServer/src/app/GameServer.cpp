#include "GameServer.h"

GameServer::GameServer()
{
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

	int a = 2;
	std::string str = "Hej";

	GameServer server;

	static sf::Clock clock;
	sf::Time time;
	float elapsed = 0; // for tick rate
	float frametime = 1 / 60;

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
		m_Socket.receive(recievepacket, sender, port);

		// Unpack the data
		PlayerInfo info;
		recievepacket >> info;

		PlayerInfo* player;

		if (m_Players.find(sender) == m_Players.end())
		{
			player = new PlayerInfo();
			*player = info;
			m_Players.emplace(sender, player);
		}

		// Set to new player position.
		player = m_Players.find(sender)->second;
		player->Position = info.Position;

		if (elapsed > frametime)
		{
			// send to clients

			for (PlayerMap::iterator it = m_Players.begin(); it != m_Players.end(); it++)
			{
				// Grab a player to send info to
				PlayerInfo* playerToSend = it->second;

				// Loop through all players and send the info to the recieving player.
				for (PlayerMap::iterator it2 = m_Players.begin(); it2 != m_Players.end(); it2++)
				{
					sf::Packet sendpacket;
					sendpacket >> *it2->second;
					m_Socket.send(sendpacket, playerToSend->IP, playerToSend->Port);
				}
			}

			elapsed -= frametime;
		}
	}
}

sf::Packet & operator<<(sf::Packet & packet, const PlayerInfo & s)
{
	return packet << s.Position.x << s.Position.y << s.Speed << s.IP.toString() << s.Port;
}


sf::Packet & operator>>(sf::Packet & packet, PlayerInfo & s)
{
	return packet >> s.Position.x >> s.Position.y >> s.Speed >> s.IP.toString() >> s.Port;
}
