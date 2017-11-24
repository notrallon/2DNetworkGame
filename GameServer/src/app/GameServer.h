#pragma once

#include <SFML/Network.hpp>
#include <map>

#include <shared-structs.h>

sf::Packet& operator<<(sf::Packet& packet, const ObjectInfo& s);
sf::Packet& operator>>(sf::Packet& packet, ObjectInfo& s);

class GameServer
{
public:
					GameServer();
					GameServer(int argc, char* argv[]);
					~GameServer();

	void			InitServer();
	void			RunServer();


private:
	using ObjectMap = std::map<unsigned int, ObjectInfo*>;

	void			DisconnectPlayer(ObjectInfo& info);
	void			UpdatePlayerInfo(ObjectInfo& info, ObjectInfo* player);
	ObjectInfo*		CreateNewPlayer(unsigned int ID, ObjectInfo& info);
	void			SpawnProjectile(ObjectInfo& info, unsigned int& ID);
	void			UpdateObjects(const float& dt);
	void			SendUpdateToClients();

	bool			m_Running;
	sf::UdpSocket	m_Socket;
	ObjectMap		m_GameObjects;
	unsigned short	m_Port;
	const sf::Clock	RUNNING_CLOCK; // Const so we still can get the time, but can't restart.
};