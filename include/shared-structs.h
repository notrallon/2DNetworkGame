#pragma once

//Automatically optimzes struct size
#pragma pack(push, 1)
struct ObjectInfo								// 68 (total size)
{
	enum ObjectTypes							// 4 (does not increase size)
	{
		Null,
		Player,
		Projectile,
		OutOfBounds
	};
	sf::Vector2f	Position;					// 8
	sf::Vector2f	Size;						// 8
	sf::Vector2f	Direction;					// 8
	sf::IpAddress	IP;							// 8
	sf::Time		LastPing;					// 8
	sf::Time		LastShot;					// 8

	sf::Vector2f	MousePosition;				// 8

	unsigned int	ID;							// 4
	float			Speed;						// 4
	int ObjectType;								// 4
	int SocketType;								// 4
	unsigned short  Port;						// 2
	bool			Shooting = false;			// 1
	bool			Connected = true;			// 1
												// = 68
};
#pragma pack(pop)
