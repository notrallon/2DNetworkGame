#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <SFML/Network.hpp>
#include <gameobjects/GameObject.h>

struct SharedContext;

struct PlayerInfo
{
	sf::Vector2f	Position;
	float			Speed;
	sf::IpAddress	IP;
	unsigned short  Port;
};

sf::Packet& operator<<(sf::Packet& packet, const PlayerInfo& s);
sf::Packet& operator>>(sf::Packet& packet, PlayerInfo& s);

class Game
{
public:
	Game();
	~Game();

	void						Run();

	void						AddObject(GameObject* object);

private:
	void						Init();
	void						Update();
	void						Draw();

	sf::RenderWindow*			m_Window;
	Player*						m_Player;
	std::vector<GameObject*>	m_GameObjects;

	sf::UdpSocket				m_Socket;

	SharedContext*				m_Context;
};

