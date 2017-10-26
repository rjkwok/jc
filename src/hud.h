//make a hud
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

#include "player.h"

void hudUpdate (sf::RenderWindow& window, sf::View window_view, float mHealthPercent) {

	const int maxHealthLength = 250;
	const int maxHealthWidth = 30;

	int healthLenght = mHealthPercent*maxHealthLength;

	//draw health bar outline
	sf::RectangleShape healthBarOutline;
	healthBarOutline.setSize(sf::Vector2f(maxHealthLength, maxHealthWidth));
	healthBarOutline.setOutlineColor(sf::Color::Red);
	healthBarOutline.setFillColor(sf::Color::Transparent);
	healthBarOutline.setOutlineThickness(5);
	healthBarOutline.setPosition(20, 20);
	
	//draw health bar
	sf::RectangleShape healthBar;
	healthBar.setSize(sf::Vector2f(healthLenght, maxHealthWidth));
	healthBar.setOutlineColor(sf::Color::Red);
	healthBar.setFillColor(sf::Color::Red);
	healthBar.setOutlineThickness(5);
	healthBar.setPosition(20, 20);

	//save the curent view
	sf::View ogView = window.getView();
	window.setView(window_view);
	window.draw(healthBarOutline);
	window.draw(healthBar);
	window.setView(ogView);
}