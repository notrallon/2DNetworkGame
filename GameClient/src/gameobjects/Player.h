#pragma once

#include "GameObject.h"

#include <vector>

class Player : public GameObject
{
	using GameObject::GameObject;
public:
	Player();
	Player(SharedContext* context);
	~Player();

	virtual void				Draw(sf::RenderWindow& window) override;
	virtual void				Update(const float& dt) override;

private:
	void						Shoot();

	sf::RectangleShape			m_Sprite;
	unsigned short				m_Speed;
	bool						m_MousePressed;

	std::vector<GameObject*>	m_Projectiles;
};