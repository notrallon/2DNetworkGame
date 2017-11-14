#pragma once

#include "GameObject.h"

class Player : public GameObject
{
public:
	Player();
	~Player();

	virtual void Update(const float& dt) override;
	virtual void Draw(sf::RenderWindow& window) override;

private:
	sf::RectangleShape m_Sprite;
	unsigned short m_Speed;
};