#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

#include "../include/classes/Polygon.hpp"

struct LinearEquation
{
    float a;
    float b;
    float c;

    bool operator==(const LinearEquation &other) const
    {
        return a == other.a && b == other.b && c == other.c;
    }
};

struct Ray
{
    sf::Vector2f origin;
    sf::Vector2f head;
};

void control_shape(Polygon &polygon);
void resolve_collision(Polygon &player, Polygon &obstacle);
LinearEquation get_linear_equation(sf::Vector2f p1, sf::Vector2f p2);
sf::Vector2f get_intersection_point(LinearEquation eq1, LinearEquation eq2);
float distance(sf::Vector2f p1, sf::Vector2f p2);
bool ray_intersection_detection(Ray ray1, Ray ray2);
bool ray_casting(const Polygon &shape, sf::Vector2f point);

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Window");
    window.setFramerateLimit(60);

    Polygon player(window, 7, 50, {100, 100}, sf::Color(255, 194, 61));
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

        if (player.isColliding(obstacle1))
        {
            resolve_collision(player, obstacle1);
        }

        // else
        // {
        //     player.setFillColor(sf::Color(255, 194, 61));
        // }

        obstacle1.draw();
        player.draw();
        // obstacle2.draw();

        // obstacle1.rotate(1);
        // obstacle2.rotate(-2);

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

void resolve_collision(Polygon &player, Polygon &obstacle)
{
    for (int i = 0; i < player.getPointCount(); i++)
    {
        auto curr_vertex = player.getTransform().transformPoint(player.getPoint(i));
        auto next_vertex = player.getTransform().transformPoint(player.getPoint((i + 1) % player.getPointCount()));

        Ray ray1{player.getPosition(), obstacle.getPosition()};
        Ray ray2{curr_vertex, next_vertex};

        if (ray_casting(obstacle, curr_vertex))
        {
            player.setPosition(
                player.getPosition().x - player.linear_speed * cos(player.getRotation() * (M_PI / 180)),
                player.getPosition().y - player.linear_speed * sin(player.getRotation() * (M_PI / 180)));
        }
        else if (ray_intersection_detection(ray1, ray2))
        {
            auto intersection_edge = next_vertex - curr_vertex;
            auto normal = sf::Vector2f(-intersection_edge.y, intersection_edge.x);

            float angle = atan2(normal.y, normal.x);

            player.setPosition(
                player.getPosition().x + player.linear_speed * cos(angle),
                player.getPosition().y + player.linear_speed * sin(angle));

            return;
        }
    }
}

LinearEquation get_linear_equation(const sf::Vector2f p1, const sf::Vector2f p2)
{
    float a = p2.y - p1.y;
    float b = p1.x - p2.x;
    float c = (p2.x * p1.y) - (p1.x * p2.y);

    return LinearEquation{a, b, c};
}

sf::Vector2f get_intersection_point(const LinearEquation eq1, const LinearEquation eq2)
{
    if (eq1 == eq2)
    {
        throw std::invalid_argument("Both equations must be different");
    }

    float x = (eq1.b * eq2.c - eq2.b * eq1.c) / (eq1.a * eq2.b - eq2.a * eq1.b);
    float y = (eq2.a * eq1.c - eq1.a * eq2.c) / (eq1.a * eq2.b - eq2.a * eq1.b);

    return sf::Vector2f(x, y);
}

float distance(sf::Vector2f p1, sf::Vector2f p2)
{
    return sqrt(pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2));
}

bool ray_intersection_detection(Ray ray1, Ray ray2)
{
    LinearEquation eq1 = get_linear_equation(ray1.origin, ray1.head);
    LinearEquation eq2 = get_linear_equation(ray2.origin, ray2.head);

    sf::Vector2f intersection_point = get_intersection_point(eq1, eq2);

    float origin_to_intersection = distance(ray1.origin, intersection_point);
    float head_to_intersection = distance(ray1.head, intersection_point);
    float length = distance(ray1.origin, ray1.head);

    bool is_internally_intersecting = !(origin_to_intersection + head_to_intersection > length);

    // std::cout << "eq1: a: " << eq1.a << "  b: " << eq1.b << "  c: " << eq1.c << std::endl;
    // std::cout << "eq2: a: " << eq2.a << "  b: " << eq2.b << "  c: " << eq2.c << std::endl;
    // std::cout << "inter: (" << intersection_point.x << ", " << intersection_point.y << ")" << std::endl;
    // std::cout << "origin_to_intersection: " << origin_to_intersection << std::endl;
    // std::cout << "head_to_intersection: " << head_to_intersection << std::endl;
    // std::cout << "length: " << length << std::endl;
    // std::cout << "is internally intersecting: " << is_internally_intersecting << std::endl;

    return is_internally_intersecting;
}

bool ray_casting(const Polygon &shape, sf::Vector2f point)
{
    if (!shape.getGlobalBounds().contains(point))
        return false;

    int crossings = 0;

    for (int i = 0; i < shape.getPointCount(); i++)
    {
        auto curr_vertex = shape.getTransform().transformPoint(shape.getPoint(i));
        auto next_vertex = shape.getTransform().transformPoint(shape.getPoint((i + 1) % shape.getPointCount()));

        bool cond1 = curr_vertex.y <= point.y;
        bool cond2 = point.y < next_vertex.y;
        bool cond3 = (point.x - curr_vertex.x) * (next_vertex.y - curr_vertex.y) < (next_vertex.x - curr_vertex.x) * (point.y - curr_vertex.y);

        if (cond1 && cond2 && cond3)
            crossings += 1;
    }

    return crossings % 2 == 1;
}