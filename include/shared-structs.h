#pragma once





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

	bool			Shooting = false;
	sf::Vector2f	MousePosition;

	enum ObjectTypes
	{
		Null,
		Player,
		Projectile,
		OutOfBounds
	};

	int ObjectType;
	int SocketType;
};
