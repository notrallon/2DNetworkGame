#include "Player.h"
#include "Projectile.h"

#include <app/SharedContext.h>

static int playerCount = 0;

Player::Player() : m_MousePressed(false)
{
	m_Sprite.setSize(sf::Vector2f(40, 80));
	m_Sprite.setFillColor(sf::Color::Blue);
	m_Speed = 300;
}

Player::Player(SharedContext * context, bool owned) : GameObject::GameObject(context), m_ClientOwned(owned)
{
	
	m_Sprite.setSize(sf::Vector2f(40, 80));
	m_Sprite.setOrigin(m_Sprite.getSize().x / 2, m_Sprite.getSize().y / 4);
	if(playerCount > 0)
	m_Sprite.setFillColor(sf::Color::Blue);
	else
	m_Sprite.setFillColor(sf::Color::Red);
	m_Speed = 300;
	m_PlayerInfo.IP = sf::IpAddress::getLocalAddress();
	m_PlayerInfo.Speed = m_Speed;
	m_ClientOwned = owned;
	m_PlayerInfo.ID = 0;

	playerCount++;
}

Player::~Player()
{
	int hej = 1;
}

void Player::Update(const float& dt)
{
	// Update the players info
	m_PlayerInfo.Position = m_Sprite.getPosition();
	if (!m_ClientOwned) return;

	sf::Vector2f dir = sf::Vector2f(0.0f, 0.0f);

	if (m_Context->window->hasFocus())
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
		{
			m_Sprite.move(sf::Vector2f(-m_Speed * dt, 0));
			dir += sf::Vector2f(-1, 0);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
		{
			m_Sprite.move(sf::Vector2f(m_Speed * dt, 0));
			dir += sf::Vector2f(1, 0);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
		{
			m_Sprite.move(sf::Vector2f(0, -m_Speed * dt));
			dir += sf::Vector2f(0, -1);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
		{
			m_Sprite.move(sf::Vector2f(0, m_Speed * dt));
			dir += sf::Vector2f(0, 1);
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
	m_PlayerInfo.Direction = dir;
}

const PlayerInfo& Player::GetPlayerInfo() const
{
	return m_PlayerInfo;
}

void Player::SetPlayerInfo(PlayerInfo info)
{
	m_PlayerInfo = info;
	m_Sprite.setPosition(m_PlayerInfo.Position);
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
