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
	~GameServer();

	void InitServer();
	void RunServer();


private:
	using PlayerMap = std::map<unsigned int, ObjectInfo*>;

	void			DisconnectPlayer(ObjectInfo& info);
	void			UpdatePlayerInfo(ObjectInfo& info, ObjectInfo* player);
	ObjectInfo*		CreateNewPlayer(unsigned int ID, ObjectInfo& info);
	void			SendUpdateToClients();

	bool			m_Running;
	sf::UdpSocket	m_Socket;
	PlayerMap		m_Players;
};