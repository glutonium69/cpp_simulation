#include <SFML/Graphics.hpp>
#include <iostream>

#include "../include/classes/Polygon.hpp"

void control_shape(Polygon &polygon);

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Window");
    window.setFramerateLimit(60);

    Polygon player(window, 3, 50, {100, 100}, sf::Color(255, 194, 61));
    player.linear_speed = 5;
    player.rotational_speed = 3;

    Polygon obstacle1(window, 5, 100, {300, 300}, sf::Color(0, 74, 119));
    Polygon obstacle2(window, 4, 70, {500, 300}, sf::Color(0, 74, 119));

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();
        }

        window.clear();

        control_shape(player);

        if (player.isColliding(obstacle1) || player.isColliding(obstacle2))
        {
            player.setFillColor(sf::Color::Red);
        }
        else
        {
            player.setFillColor(sf::Color(255, 194, 61));
        }

        player.draw();
        obstacle1.draw();
        obstacle2.draw();

        obstacle1.rotate(1);
        obstacle2.rotate(-2);

        window.display();
    }

    return 0;
}

void control_shape(Polygon &polygon)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        polygon.rotate(+polygon.rotational_speed);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        polygon.rotate(-polygon.rotational_speed);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        polygon.walk(+polygon.linear_speed);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        polygon.walk(-polygon.linear_speed);
    }
}