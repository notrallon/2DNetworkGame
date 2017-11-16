#pragma once

#include "GameObject.h"
#include <app/Game.h>

#include <vector>

class Player : public GameObject
{
	using GameObject::GameObject;
public:
	Player();
	Player(SharedContext* context, bool owned);
	~Player();

	virtual void				Draw(sf::RenderWindow& window) override;
	virtual void				Update(const float& dt) override;

	const PlayerInfo&			GetPlayerInfo() const;
	void						SetPlayerInfo(PlayerInfo info);

private:
	//static int					playerCount;

	void						Shoot();

	PlayerInfo					m_PlayerInfo;
	sf::RectangleShape			m_Sprite;
	unsigned short				m_Speed;
	bool						m_MousePressed;
	bool						m_ClientOwned;

	std::vector<GameObject*>	m_Projectiles;
};