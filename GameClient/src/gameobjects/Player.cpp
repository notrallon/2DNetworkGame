#include "Player.h"
#include "Projectile.h"

#include <app/SharedContext.h>

Player::Player() : m_MousePressed(false)
{
	m_Sprite.setSize(sf::Vector2f(40, 80));
	m_Sprite.setFillColor(sf::Color::Blue);
	m_Speed = 300;
}

Player::Player(SharedContext * context) : GameObject::GameObject(context)
{
	m_Sprite.setSize(sf::Vector2f(40, 80));
	m_Sprite.setOrigin(m_Sprite.getSize().x / 2, m_Sprite.getSize().y / 4);
	m_Sprite.setFillColor(sf::Color::Blue);
	m_Speed = 300;
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

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !m_MousePressed)
	{
		Shoot();
	}
	else if (!sf::Mouse::isButtonPressed(sf::Mouse::Left) && m_MousePressed)
	{
		m_MousePressed = false;
	}
}

void Player::Draw(sf::RenderWindow& window)
{
	window.draw(m_Sprite);
}

void Player::Shoot()
{
	m_MousePressed = true;
	sf::Vector2i mousepos = sf::Mouse::getPosition(*m_Context->window);


	sf::Vector2f direction = sf::Vector2f(mousepos.x - m_Sprite.getPosition().x, mousepos.y - m_Sprite.getPosition().y);

	float length = abs(sqrt(direction.x * direction.x + direction.y * direction.y));

	direction.x /= length;
	direction.y /= length;
	
	m_Context->game->AddObject(new Projectile(m_Sprite.getPosition(), direction));
}
