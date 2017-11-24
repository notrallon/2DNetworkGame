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

	//TODO: Temp
	static sf::RectangleShape groundFloor;

	int							Run();

	void						AddObject(GameObject* object);

	friend sf::Packet& operator<<(sf::Packet& packet, const ObjectInfo& s);
	friend sf::Packet& operator>>(sf::Packet& packet, ObjectInfo& s);
private:
	using ObjectMap = std::map<unsigned int, GameObject*>;
	void						Init();
	void						Update();
	void						Draw();

	void						Send();
	void						Recieve();
	void						CreateClientPlayer();
	void						Disconnect();

	sf::RenderWindow*			m_Window;
	Player*						m_Player;
	ObjectMap					m_GameObjects;

	sf::UdpSocket				m_Socket;
	sf::IpAddress				m_ServerIP;
	unsigned short				m_ServerPort;

	SharedContext*				m_Context;
};

