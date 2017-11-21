#pragma once

#include <SFML/Network.hpp>
#include <map>

#include <shared-structs.h>

sf::Packet& operator<<(sf::Packet& packet, const PlayerInfo& s);
sf::Packet& operator>>(sf::Packet& packet, PlayerInfo& s);

class GameServer
{
public:
	GameServer();
	~GameServer();

	void InitServer();
	void RunServer();


private:
	using PlayerMap = std::map<unsigned int, PlayerInfo*>;

	void			DisconnectPlayer(PlayerInfo& info);
	void			UpdatePlayerInfo(PlayerInfo& info, PlayerInfo* player);
	PlayerInfo*		CreateNewPlayer(unsigned int ID, PlayerInfo& info);
	void			SendUpdateToClients();

	bool			m_Running;
	sf::UdpSocket	m_Socket;
	PlayerMap		m_Players;
};