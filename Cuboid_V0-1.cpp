#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>
#include <cmath>

#include <SFML/Graphics.hpp>

int main()
{
	const std::string program_name{"Cuboid V0.1"};
	assert(program_name != "");
	
	const float time_quant{0.001f};
	assert(time_quant > 0.0f);
	
	const float time_mult{25.0f};
	assert(time_mult > 0.0f);
	
	const float time_delta{time_mult*time_quant};
	
	const float window_x{704.0f};
	assert(window_x > 0.0f);
	
	const float window_y{704.0f};
	assert(window_y > 0.0f);
	
	const sf::Vector2f window_sizes{window_x, window_y};
	
	const sf::Color black{sf::Color(0, 0, 0)};
	
	sf::RenderWindow window{sf::VideoMode(window_x, window_y), program_name, sf::Style::Default};
	
	while (window.isOpen())
	{
		sf::Event event;
		
		sf::Clock clock;
		
		window.clear(black);
		
		window.display();
		
		while (clock.getElapsedTime().asSeconds() < time_delta)
		{
		}
		
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			window.close();                    
			return 1;
		}
		
		while (window.pollEvent(event))
		{				
			if (event.type == sf::Event::Closed)
			{
				window.close();                
				return 2;
			}			
		}		
	}	
	
	return 0;
}
