#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>
#include <cmath>

#include <SFML/Graphics.hpp>



void move_step(sf::Vector3f& position, const sf::Vector3f& direction, const float delta_dist)
{
	assert(delta_dist > 0.0f);	
	position = position + delta_dist*direction;
}

void rotate_stap(sf::Vector2f& posit, const float sin_phi, const float cos_phi)
{
	const sf::Vector2f temp{cos_phi*posit.x + sin_phi*posit.y, -sin_phi*posit.x + cos_phi*posit.y};
	posit = temp;	
}

void rotate_step(sf::Vector3f& position, const sf::Vector3f& direction, const float sin_phi, const float cos_phi)
{
	assert(sin_phi != 0.0f);
	assert(cos_phi != 1.0f);
	assert(direction.x > -0.5f);
	assert(direction.y > -0.5f);
	assert(direction.z > -0.5f);
	
	sf::Vector2f posit{0.0f, 0.0f};
	
	if (direction.x > 0.5f)
	{
		posit.x = position.y;
		posit.y = position.z;
		
		rotate_stap(posit, sin_phi, cos_phi);
		
		position.y = posit.x;
		position.z = posit.y;		
	}
	
	if (direction.y > 0.5f)
	{
		posit.x = position.z;
		posit.y = position.x;
		
		rotate_stap(posit, sin_phi, cos_phi);
		
		position.z = posit.x;
		position.x = posit.y;		
	}
	
	if (direction.z > 0.5f)
	{
		posit.x = position.x;
		posit.y = position.y;
		
		rotate_stap(posit, sin_phi, cos_phi);
		
		position.x = posit.x;
		position.y = posit.y;		
	}
}

bool in_view(const sf::Vector3f& position)
{
	if ((position.x >= position.y) && (position.x >= position.z))
	{
		return true;
	}
	
	return false;
}

class cuboid
{
	sf::Color m_color{127, 127, 127};
	
	sf::Vector3f m_central_posit{0.0f, 0.0f, 0.0f};
	
	const float m_side_length{1.0f};
	
	const float m_sin_phi{0.0f};
	
	const float m_cos_phi{1.0f};
	
	std::vector <std::vector <std::vector <sf::Vector3f>>> m_abs_posits;
	
	std::vector <sf::Vector3f> m_rel_posits;
	
	sf::VertexArray m_quad{sf::Quads, 4};
	
	std::vector <sf::VertexArray> m_quads;

	void setup_posits()
	{		
		m_abs_posits.resize(2);
		
		for (int count_1{0}; count_1 < 2; ++count_1)
		{			
			m_abs_posits[count_1].resize(2);
			
			for (int count_2{0}; count_2 < 2; ++ count_2)
			{
				m_abs_posits[count_1][count_2].resize(2);
			}			
		}		
	}
	
	void place_posits()
	{
		assert(m_side_length > 0.0f);
		
		for (int count_1{0}; count_1 < 2; ++count_1)
		{
			for (int count_2{0}; count_2 < 2; ++count_2)
			{
				for (int count_3{0}; count_3 < 2; ++count_3)
				{
					m_abs_posits[count_1][count_2][count_3] =
					m_central_posit +
					0.5f*m_side_length*sf::Vector3f((2.0f*count_1 - 1.0f), (2.0f*count_2 - 1.0f), (2.0f*count_3 - 1.0f));
				}				
			}		
		}		
	}
	
	void init_posits()
	{		
		m_rel_posits.resize(8);
	}
	
	void arrange_posits()
	{
		assert(m_side_length > 0.0f);
		
		for (int count{0}; count < 8; ++count)
		{
			m_rel_posits[count] =
			m_central_posit +
			0.5f*m_side_length*
			sf::Vector3f((2.0f*(count % 2 ) - 1.0f), (2.0f*((count / 2) % 2) - 1.0f), (2.0f*(((count / 4) % 2) - 1.0f)));
		}		
	}
	
	void set_quads_x()
	{
		for (int count{0}; count < 6; ++count)
		{
			m_quads.push_back(m_quad);
		}
	}
	
	bool point_sighted()
	{
		// return in_view(const sf::Vector3f& position);
		
		return false;
	}
	
	float closest()
	{
		float min_dist{1000000.0f};
		
		for(int count{0}; count < 8; ++count)
		{
			const float cur_dist{m_rel_posits[count].x*m_rel_posits[count].x +
								 m_rel_posits[count].y*m_rel_posits[count].y +
								 m_rel_posits[count].z*m_rel_posits[count].z};
								 
			if (cur_dist < min_dist)
			{
				min_dist = 	cur_dist;					
			}
		}
		
		return min_dist;
	}
	
	
	
		
};

class observoid
{
	sf::Vector3f m_central_posit{0.0f, 0.0f, 0.0f};
	
	sf::Vector3f m_central_sight{1.0f, 0.0f, 0.0f};
	
	sf::Vector2f m_port_view{1.0f, 1.0f};
	
	sf::Vector2f m_port_window{1.0f, 1.0f};
	
	const float m_port_scale{m_port_window.x/m_port_view.x};
	
	
	
	
};

int main()
{
	const std::string program_name{"Cuboid V0.2"};
	assert(program_name != "");
	
	const float pi{4.0f*atan(1.0f)};
	
	const float divisions{20.0f};
	assert(divisions > 0.0f);
	
	const float delta_dist{1.0f/divisions};
	const float delta_phi{0.5f*pi/divisions};
	const float sin_phi{sin(delta_phi)};
	const float cos_phi{cos(delta_phi)};
	
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
	
	const sf::Vector2f zero_point{0.0f, 0.0f};
	
	const sf::Color black{0, 0, 0};
	const sf::Color light_purple{255, 127, 255};
	
	sf::RenderWindow window{sf::VideoMode(window_x, window_y), program_name, sf::Style::Default};
	
	const float dist{1.0f};
	assert(dist > 0.0f);
	
	sf::Vector2f area{dist, dist};
	
	sf::View view;
	view.setCenter(zero_point);
	view.setSize(area);

	const float radius{0.5f*dist};
	
	sf::CircleShape circle(radius);
	circle.setOrigin(radius, radius);
	circle.setFillColor(light_purple);
	window.setView(view);
	
	while (window.isOpen())
	{
		sf::Event event;
		
		sf::Clock clock;
		
		window.clear(black);
		
		window.draw(circle);
		
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
