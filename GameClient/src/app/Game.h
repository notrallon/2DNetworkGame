#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include <map>

#include <shared-structs.h>

struct SharedContext;
class Player; 
class GameObject;

class Game
{
public:
	Game();
	Game(int argc, char* argv[]);
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
	sf::IpAddress				m_ServerIP;

	SharedContext*				m_Context;
};

