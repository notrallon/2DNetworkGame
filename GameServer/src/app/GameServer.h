#pragma once

#include <SFML/Network.hpp>
#include <map>

struct PlayerInfo
{
	unsigned int	ID;
	sf::Vector2f	Position;
	float			Speed;
	sf::IpAddress	IP;
	unsigned short  Port;
};

sf::Packet& operator<<(sf::Packet& packet, const PlayerInfo& s);
sf::Packet& operator>>(sf::Packet& packet, PlayerInfo& s);

class GameServer
{
public:
	GameServer();
	~GameServer();

	void InitServer();
	void RunServer();


private:
	using PlayerMap = std::map<unsigned int, PlayerInfo*>;

	bool			m_Running;
	sf::UdpSocket	m_Socket;
	PlayerMap		m_Players;
};


/*
struct ServerInfo
{
	int a;

};

struct PlayerInfo
{
	float b;
};

struct ProjectileInfo
{
	bool y;
};

struct PacketInfo
{
	ServerInfo* srvinfo;
	PlayerInfo* player;

};


sf::Packet& operator<<(sf::Packet& packet, const ServerInfo& s)
{
	return packet << s.a;
}

sf::Packet& operator<<(sf::Packet& packet, const PlayerInfo& s)
{
	return packet << s.b;
}

sf::Packet& operator<<(sf::Packet& packet, const ProjectileInfo& s)
{
	return packet << s.y;
}


sf::Packet& operator<<(sf::Packet& packet, const PacketInfo& s)
{
	return packet << s.srvinfo << s.player;
}




sf::Packet& operator<<(sf::Packet& packet, const GameServer& s)
{
	return packet << s.hej.x << s.hej.y;
}*/