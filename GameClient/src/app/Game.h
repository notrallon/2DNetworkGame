#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include <map>

enum class SocketType {
	Player,
	Disconnect
};

struct SharedContext;
class Player; 
class GameObject;

struct PlayerInfo
{
	unsigned int	ID;
	sf::Vector2f	Position;
	sf::Vector2f	Direction;
	float			Speed;
	sf::IpAddress	IP;
	unsigned short  Port;
	bool			Connected = true;
};


class Game
{
public:
	Game();
	~Game();

	void						Run();

	void						AddObject(GameObject* object);

	friend sf::Packet& operator<<(sf::Packet& packet, const PlayerInfo& s);
	friend sf::Packet& operator>>(sf::Packet& packet, PlayerInfo& s);
private:
	using ObjectMap = std::map<unsigned int, GameObject*>;
	void						Init();
	void						Update();
	void						Draw();

	void						Send();
	void						Recieve();
	void						CreateClientPlayer();

	sf::RenderWindow*			m_Window;
	Player*						m_Player;
	ObjectMap					m_GameObjects;

	sf::UdpSocket				m_Socket;

	SharedContext*				m_Context;
};

