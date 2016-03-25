#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>
#include <cmath>

#include <SFML/Graphics.hpp>


float distance_squared(const sf::Vector3f& distance, const float sight_square, bool& sighted)
{
	
	const float mult{1.5f};
	const float square{distance.x*distance.x + distance.y*distance.y + distance.z*distance.z};
	
	if ((square <= sight_square) && (distance.x > 0.0f) &&
		(mult*distance.x >= std::abs(distance.y)) && (mult*distance.x >= std::abs(distance.z)))
	{
		sighted = true;
	}
	
	return square;
}

sf::Color square_to_color(const sf::Color& color, const float square, const float sight_inverse)
{	
	float mult{1.0f - sight_inverse*std::sqrt(square)};
	assert(mult <= 1.0f);
	
	if (mult < 0.0f)
	{
		mult = 0.0f;		
	}
	
	const int red{static_cast<int>(mult*static_cast<float>(color.r))};
	const int green{static_cast<int>(mult*static_cast<float>(color.g))};
	const int blue{static_cast<int>(mult*static_cast<float>(color.b))};
	
	return sf::Color (red, green, blue);
}

sf::Vector3f key_to_move()
{
	sf::Vector3f direction{0.0f, 0.0f, 0.0f};
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		direction = sf::Vector3f (1.0f, 0.0f, 0.0f);	
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		direction = sf::Vector3f (0.0f, 1.0f, 0.0f);	
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
	{
		direction = sf::Vector3f (0.0f, 0.0f, 1.0f);	
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		direction = sf::Vector3f (-1.0f, 0.0f, 0.0f);	
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		direction = sf::Vector3f (0.0f, -1.0f, 0.0f);	
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
	{
		direction = sf::Vector3f (0.0f, 0.0f, -1.0f);	
	}
	
	return direction;
}

sf::Vector3f key_to_rotate()
{
	sf::Vector3f direction{0.0f, 0.0f, 0.0f};
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::I))
	{
		direction = sf::Vector3f (0.0f, -1.0f, 0.0f);	
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
	{
		direction = sf::Vector3f (0.0f, 0.0f, 1.0f);	
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))
	{
		direction = sf::Vector3f (-1.0f, 0.0f, 0.0f);	
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::K))
	{
		direction = sf::Vector3f (0.0f, 1.0f, 0.0f);	
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::J))
	{
		direction = sf::Vector3f (0.0f, 0.0f, -1.0f);	
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::U))
	{
		direction = sf::Vector3f (1.0f, 0.0f, 0.0f);	
	}
	
	return direction;
}

void move_step(sf::Vector3f& position, const sf::Vector3f& direction, const float delta_dist)
{
	assert(delta_dist > 0.0f);	
	position = position - delta_dist*direction;
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
	
	sf::Vector2f posit{0.0f, 0.0f};
	
	if (std::abs(direction.x) > 0.5f)
	{
		posit.x = position.y;
		posit.y = position.z;
		
		rotate_stap(posit, direction.x*sin_phi, cos_phi);
		
		position.y = posit.x;
		position.z = posit.y;		
	}
	
	if (std::abs(direction.y) > 0.5f)
	{
		posit.x = position.z;
		posit.y = position.x;
		
		rotate_stap(posit, direction.y*sin_phi, cos_phi);
		
		position.z = posit.x;
		position.x = posit.y;		
	}
	
	if (std::abs(direction.z) > 0.5f)
	{
		posit.x = position.x;
		posit.y = position.y;
		
		rotate_stap(posit, direction.z*sin_phi, cos_phi);
		
		position.x = posit.x;
		position.y = posit.y;		
	}
}

void cubordination(std::vector <bool>& cubords, const bool count_1, const bool count_2, const bool count_3)
{
	cubords = {count_1, count_2, count_3};	
}

sf::Vector2f three_to_two(const sf::Vector3f& position)
{
	const float tad{0.0000001f};
	float posit_x{position.x};
	
	if (position.x <= 0.0f)
	{
		posit_x = tad;
	}
	
	posit_x = 1/posit_x;	
	return sf::Vector2f (0.5f*position.y*posit_x, -0.5f*position.z*posit_x);	
}

class cuboid
{
	sf::Color m_color{127, 127, 127};
	
	sf::Vector3f m_central_posit{0.0f, 0.0f, 0.0f};
	
	float m_sight{4.0f};
	float m_sight_square{m_sight*m_sight};
	float m_sight_inverse{1.0f/m_sight};
	
	float m_side_length{1.0f};
	
	float m_delta_dist{0.1f};
	
	float m_sin_phi{0.0f};
	
	float m_cos_phi{1.0f};
	
	std::vector <std::vector <std::vector <sf::Vector3f>>> m_abs_posits;
	
	std::vector <std::vector <std::vector <float>>> m_abs_squares;
	
	std::vector <bool> m_cubordinates{false, false, false};
	
	std::vector <float> m_average_squares{1.0e20f, 1.0e20f, 1.0e20f};
	
	bool m_sighted{false};
	
	sf::VertexArray m_quad{sf::Quads, 4};
	
	std::vector <sf::VertexArray> m_quads{m_quad, m_quad, m_quad};
	
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

	void setup_squares()
	{		
		m_abs_squares.resize(2);
		
		for (int count_1{0}; count_1 < 2; ++count_1)
		{			
			m_abs_squares[count_1].resize(2);
			
			for (int count_2{0}; count_2 < 2; ++ count_2)
			{
				m_abs_squares[count_1][count_2].resize(2);
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
					0.5f*m_side_length*sf::Vector3f((2.0f*static_cast<float>(count_1) - 1.0f),
													(2.0f*static_cast<float>(count_2) - 1.0f),
													(2.0f*static_cast<float>(count_3) - 1.0f));
				}				
			}		
		}		
	}
	
	public:
	
	void move_posits(const sf::Vector3f& direction)
	{
		move_step(m_central_posit, direction, m_delta_dist);
		
		for (int count_1{0}; count_1 < 2; ++count_1)
		{
			for (int count_2{0}; count_2 < 2; ++count_2)
			{
				for (int count_3{0}; count_3 < 2; ++count_3)
				{
					move_step(m_abs_posits[count_1][count_2][count_3], direction, m_delta_dist);
				}				
			}		
		}		
	}
	
	void rotate_posits(const sf::Vector3f& direction)
	{
		rotate_step(m_central_posit, direction, m_sin_phi, m_cos_phi);
		
		for (int count_1{0}; count_1 < 2; ++count_1)
		{
			for (int count_2{0}; count_2 < 2; ++count_2)
			{
				for (int count_3{0}; count_3 < 2; ++count_3)
				{
					rotate_step(m_abs_posits[count_1][count_2][count_3], direction, m_sin_phi, m_cos_phi);
				}				
			}		
		}
	}
	
	void calc_squares()
	{	
		m_sighted = false;
		
		for (int count_1{0}; count_1 < 2; ++count_1)
		{
			for (int count_2{0}; count_2 < 2; ++count_2)
			{
				for (int count_3{0}; count_3 < 2; ++count_3)
				{							
					m_abs_squares[count_1][count_2][count_3] =
					distance_squared(m_abs_posits[count_1][count_2][count_3], m_sight_square, m_sighted);
					// in_view(m_abs_posits[count_1][count_2][count_3], m_sighted);
				}				
			}		
		}		
	}
	
	void smallest_square()
	{
		if (m_sighted)
		{
			float small{1.0e20f};
			
			for (int count_1{0}; count_1 < 2; ++count_1)
			{
				for (int count_2{0}; count_2 < 2; ++count_2)
				{
					for (int count_3{0}; count_3 < 2; ++count_3)
					{
						if (m_abs_squares[count_1][count_2][count_3] < small)
						{
							small = m_abs_squares[count_1][count_2][count_3];
							cubordination(m_cubordinates, count_1, count_2, count_3);
						}
					}				
				}		
			}
		}
	}
	
	void cube_to_rects()
	{
		if (m_sighted)
		{		
			m_quads[0][0].position = three_to_two(m_abs_posits[m_cubordinates[0]][m_cubordinates[1]][m_cubordinates[2]]);
			m_quads[0][1].position = three_to_two(m_abs_posits[!m_cubordinates[0]][m_cubordinates[1]][m_cubordinates[2]]);
			m_quads[0][2].position = three_to_two(m_abs_posits[!m_cubordinates[0]][!m_cubordinates[1]][m_cubordinates[2]]);
			m_quads[0][3].position = three_to_two(m_abs_posits[m_cubordinates[0]][!m_cubordinates[1]][m_cubordinates[2]]);
			
			m_quads[0][0].color = square_to_color(m_color, m_abs_squares[m_cubordinates[0]][m_cubordinates[1]][m_cubordinates[2]], m_sight_inverse);
			m_quads[0][1].color = square_to_color(m_color, m_abs_squares[!m_cubordinates[0]][m_cubordinates[1]][m_cubordinates[2]], m_sight_inverse);
			m_quads[0][2].color = square_to_color(m_color, m_abs_squares[!m_cubordinates[0]][!m_cubordinates[1]][m_cubordinates[2]], m_sight_inverse);
			m_quads[0][3].color = square_to_color(m_color, m_abs_squares[m_cubordinates[0]][!m_cubordinates[1]][m_cubordinates[2]], m_sight_inverse);
			
			m_average_squares[0] = 0.25f*(m_abs_squares[m_cubordinates[0]][m_cubordinates[1]][m_cubordinates[2]] +
										  m_abs_squares[!m_cubordinates[0]][m_cubordinates[1]][m_cubordinates[2]] +
										  m_abs_squares[!m_cubordinates[0]][!m_cubordinates[1]][m_cubordinates[2]] +
										  m_abs_squares[m_cubordinates[0]][!m_cubordinates[1]][m_cubordinates[2]]);
			
			m_quads[1][0].position = three_to_two(m_abs_posits[m_cubordinates[0]][m_cubordinates[1]][m_cubordinates[2]]);
			m_quads[1][1].position = three_to_two(m_abs_posits[m_cubordinates[0]][!m_cubordinates[1]][m_cubordinates[2]]);
			m_quads[1][2].position = three_to_two(m_abs_posits[m_cubordinates[0]][!m_cubordinates[1]][!m_cubordinates[2]]);
			m_quads[1][3].position = three_to_two(m_abs_posits[m_cubordinates[0]][m_cubordinates[1]][!m_cubordinates[2]]);
			
			m_quads[1][0].color = square_to_color(m_color, m_abs_squares[m_cubordinates[0]][m_cubordinates[1]][m_cubordinates[2]], m_sight_inverse);
			m_quads[1][1].color = square_to_color(m_color, m_abs_squares[m_cubordinates[0]][!m_cubordinates[1]][m_cubordinates[2]], m_sight_inverse);
			m_quads[1][2].color = square_to_color(m_color, m_abs_squares[m_cubordinates[0]][!m_cubordinates[1]][!m_cubordinates[2]], m_sight_inverse);
			m_quads[1][3].color = square_to_color(m_color, m_abs_squares[m_cubordinates[0]][m_cubordinates[1]][!m_cubordinates[2]], m_sight_inverse);
			
			m_average_squares[1] = 0.25f*(m_abs_squares[m_cubordinates[0]][m_cubordinates[1]][m_cubordinates[2]] +
										  m_abs_squares[m_cubordinates[0]][!m_cubordinates[1]][m_cubordinates[2]] +
										  m_abs_squares[m_cubordinates[0]][!m_cubordinates[1]][!m_cubordinates[2]] +
										  m_abs_squares[m_cubordinates[0]][m_cubordinates[1]][!m_cubordinates[2]]);
			
			m_quads[2][0].position = three_to_two(m_abs_posits[m_cubordinates[0]][m_cubordinates[1]][m_cubordinates[2]]);
			m_quads[2][1].position = three_to_two(m_abs_posits[m_cubordinates[0]][m_cubordinates[1]][!m_cubordinates[2]]);
			m_quads[2][2].position = three_to_two(m_abs_posits[!m_cubordinates[0]][m_cubordinates[1]][!m_cubordinates[2]]);
			m_quads[2][3].position = three_to_two(m_abs_posits[!m_cubordinates[0]][m_cubordinates[1]][m_cubordinates[2]]);
			
			m_quads[2][0].color = square_to_color(m_color, m_abs_squares[m_cubordinates[0]][m_cubordinates[1]][m_cubordinates[2]], m_sight_inverse);
			m_quads[2][1].color = square_to_color(m_color, m_abs_squares[m_cubordinates[0]][m_cubordinates[1]][!m_cubordinates[2]], m_sight_inverse);
			m_quads[2][2].color = square_to_color(m_color, m_abs_squares[!m_cubordinates[0]][m_cubordinates[1]][!m_cubordinates[2]], m_sight_inverse);
			m_quads[2][3].color = square_to_color(m_color, m_abs_squares[!m_cubordinates[0]][m_cubordinates[1]][m_cubordinates[2]], m_sight_inverse);
			
			m_average_squares[2] = 0.25f*(m_abs_squares[m_cubordinates[0]][m_cubordinates[1]][m_cubordinates[2]] +
										  m_abs_squares[m_cubordinates[0]][m_cubordinates[1]][!m_cubordinates[2]] +
										  m_abs_squares[!m_cubordinates[0]][m_cubordinates[1]][!m_cubordinates[2]] +
										  m_abs_squares[!m_cubordinates[0]][m_cubordinates[1]][m_cubordinates[2]]);
		}
	}
	
	void show_rects(sf::RenderWindow& window)
	{
		if (m_sighted)
		{
			float smallest{1.0e20f};
			int last{-1};

			int middle{-1};
						
			float biggest{-1.0f};
			int first{-1};
			
			for (int count{0}; count < 3; ++count)
			{
				if (m_average_squares[count] < smallest)
				{
					smallest = m_average_squares[count];
					last = count;
				}
				
				if (m_average_squares[count] > biggest)
				{
					biggest = m_average_squares[count];
					first = count;
				}				
			}
			
			for (int count{0}; count < 3; ++count)
			{
				if ((count != first) && (count != last))
				{					
					middle = count;
				}
			}
			
			window.draw(m_quads[first]);
			window.draw(m_quads[middle]);
			window.draw(m_quads[last]);
		}	
	}
	
	bool sighted()
	{
		return m_sighted;
	}
	
	float middle_square()
	{
		return distance_squared(m_central_posit, m_sight_square, m_sighted);
	}
	
	cuboid(const sf::Color& color, const sf::Vector3f& central_posit,
		   const float side_length, const float delta_dist, const float sin_phi, const float cos_phi)
		   : m_color(color), m_central_posit(central_posit), m_side_length(side_length),
		   m_delta_dist(delta_dist), m_sin_phi(sin_phi), m_cos_phi(cos_phi),
		   m_abs_posits(), m_abs_squares()
	{
		setup_posits();
		setup_squares();
		place_posits();
		calc_squares();
	}
	
	~cuboid()
	{
	}
	
};

bool square_comp(cuboid& cube_i, cuboid& cube_j)
{
	return (cube_i.middle_square() > cube_j.middle_square());
}

void order_kubes(std::vector <cuboid>& vis_kubes, const int vis_kube_numbers)
{
	int nonswap_count{0};
	int loop_count{0};
	
	while ((nonswap_count < vis_kube_numbers - 1) && (loop_count < vis_kube_numbers))
	{
		nonswap_count = 0;
		
		for (int count{0}; count < vis_kube_numbers - 1; ++count)
		{
			if (vis_kubes[count].middle_square() < vis_kubes[count + 1].middle_square())
			{
				std::swap(vis_kubes[count], vis_kubes[count + 1]);
			}
			else
			{
				++nonswap_count;
			}
		}
		
		++loop_count;
	}
}

void show_kubes(std::vector <cuboid>& vis_kubes, sf::RenderWindow& window, const int vis_kube_numbers)
{
	std::cout << vis_kube_numbers << "\n";
	for (int count{0}; count < vis_kube_numbers; ++count)
	{
		vis_kubes[count].calc_squares();
	}
	
	order_kubes(vis_kubes, vis_kube_numbers);
	
	for (int count{0}; count < vis_kube_numbers; ++count)
	{
		vis_kubes[count].smallest_square();
		vis_kubes[count].cube_to_rects();	
		vis_kubes[count].show_rects(window);
	}		
}

void select_kubes(std::vector <cuboid>& kubes, sf::RenderWindow& window, const int kube_numbers)
{
	std::vector <cuboid> vis_kubes;
	
	int count{0};
	int vis_kube_numbers{0};
	
	while (count < kube_numbers)
	{
		kubes[count].calc_squares();
		
		if (kubes[count].sighted())
		{
			vis_kubes.push_back(kubes[count]);
			++vis_kube_numbers;
		}
		++count;		
	}
	
	if (vis_kube_numbers > 0)
	{
		show_kubes(vis_kubes, window, vis_kube_numbers);
	}	
}

int main()
{
	const std::string program_name{"Cuboid V0.6"};
	assert(program_name != "");
	
	const float pi{4.0f*atan(1.0f)};
	
	const float divisions{20.0f};
	assert(divisions > 0.0f);
	
	const float side_length{1.0f};
	assert(side_length > 0.0f);
	
	const float delta_dist{1.0f/divisions};
	const float delta_phi{0.25f*pi/divisions};
	const float sin_phi{sin(delta_phi)};
	const float cos_phi{cos(delta_phi)};
	
	const float time_quant{0.001f};
	assert(time_quant > 0.0f);
	
	const float time_mult{15.0f};
	assert(time_mult > 0.0f);
	
	const float time_delta{time_mult*time_quant};
	
	const float window_x{704.0f};
	assert(window_x > 0.0f);
	
	const float window_y{704.0f};
	assert(window_y > 0.0f);
	
	const sf::Vector2f window_sizes{window_x, window_y};
	
	const sf::Vector2f zero_point{0.0f, 0.0f};
	
	const sf::Color black{0, 0, 0};
	const sf::Color white{255, 255, 255};
	const sf::Color light_purple{255, 127, 255};
	const sf::Color light_orange{255, 195, 127};
	const sf::Color light_green{127, 255, 127};
	const sf::Color light_blue{127, 127, 255};
	
	std::vector <sf::Color> colors{light_green,
								   light_blue,
								   light_orange,
								   light_purple};
								   
	const int colors_count{static_cast<int>(colors.size())};	
	
	sf::RenderWindow window{sf::VideoMode(window_x, window_y), program_name, sf::Style::Default};
	
	const float spacing{1.0f};
	assert(spacing > 0.0f);

	std::vector <sf::Vector3f> central_posits;
	
		
	const int jump_count{2};
	assert(jump_count > 0);
	
	const int max_count{5};
	assert(max_count > 0);
	
	for (int count_x{-max_count}; count_x <= max_count; count_x += jump_count)
	{
		for (int count_y{-max_count}; count_y <= max_count; count_y += jump_count)
		{
			for (int count_z{-max_count}; count_z <= max_count; count_z += jump_count)
			{
				central_posits.push_back(sf::Vector3f (spacing*static_cast<float>(count_x),
													   spacing*static_cast<float>(count_y),
													   spacing*static_cast<float>(count_z)));
			}
		}
	}
	
	const int kube_numbers{static_cast<int>(central_posits.size())};
													
	std::vector <sf::Color> kolors;
	
	for (int count{0}; count < kube_numbers; ++count)
	{
		kolors.push_back(colors[count % colors_count]);
	}
														
	std::vector <cuboid> kubes;
	
	for (int count{0}; count < kube_numbers; ++count)
	{
		kubes.push_back(cuboid (kolors[count], central_posits[count], side_length, delta_dist, sin_phi, cos_phi));
	}
		
	const float dist{0.5f};
	assert(dist > 0.0f);
	
	const float radius{0.01f*dist};
	assert(radius > 0.0f);
	
	const sf::Vector2f circle_posit{0.0f*dist, 0.0f*dist};
	
	sf::CircleShape circle{radius};
	
	circle.setOrigin(radius, radius);
	circle.setPosition(circle_posit);
	circle.setFillColor(white);
	
	sf::Vector2f area{dist, dist};
	
	sf::View view;
	view.setCenter(zero_point);
	view.setSize(area);

	window.setView(view);
	
	while (window.isOpen())
	{
		sf::Event event;
		
		sf::Clock clock;
		
		window.clear(black);
		
		select_kubes(kubes, window, kube_numbers);
		
		window.draw(circle);
		
		window.display();
		
		const sf::Vector3f key_move{key_to_move()};
		const sf::Vector3f key_rotate{key_to_rotate()};
				
		for (int count{0}; count < kube_numbers; ++count)
		{
			kubes[count].move_posits(key_move);
			kubes[count].rotate_posits(key_rotate);
		}
						
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
