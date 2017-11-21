#pragma once

enum class SocketType {
	Player,
	Disconnect
};

struct PlayerInfo
{
	unsigned int	ID;
	sf::Vector2f	Position;
	sf::Vector2f	Direction;
	float			Speed;
	sf::IpAddress	IP;
	unsigned short  Port;
	bool			Connected = true;
	sf::Time		LastPing;
};
