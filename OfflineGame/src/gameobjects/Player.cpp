#include "Player.h"

Player::Player()
{
	m_Sprite.setSize(sf::Vector2f(40, 80));
	m_Sprite.setFillColor(sf::Color::Blue);
	m_Speed = 50;
}

Player::~Player()
{
}

void Player::Update(const float& dt)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
	{
		m_Sprite.move(sf::Vector2f(-m_Speed * dt, 0));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
	{
		m_Sprite.move(sf::Vector2f(m_Speed * dt, 0));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
	{
		m_Sprite.move(sf::Vector2f(0, -m_Speed * dt));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
	{
		m_Sprite.move(sf::Vector2f(0, m_Speed * dt));
	}
}

void Player::Draw(sf::RenderWindow& window)
{
	window.draw(m_Sprite);
}
