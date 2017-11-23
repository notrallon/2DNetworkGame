#pragma once

enum class SocketTypes 
{
	Null,
	Player,
	Disconnect,
	Projectile,
	OutOfBounds
};

enum class ObjectTypes
{
	Null,
	Player,
	Projectile
};

struct ObjectInfo
{
	unsigned int	ID;
	sf::Vector2f	Position;
	sf::Vector2f	Direction;
	float			Speed;
	sf::IpAddress	IP;
	unsigned short  Port;
	bool			Connected = true;
	sf::Time		LastPing;

	bool			Shooting;
	sf::Vector2f	MousePosition;

	ObjectTypes		ObjectType;
	SocketTypes		PacketType;
};
