#ifndef POLYGON_HPP
#define POLYGON_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <array>

class Polygon : public sf::ConvexShape
{
public:
    sf::RenderWindow &window;

    float radius;
    float linear_speed = 5;
    float rotational_speed = 2;

    Polygon(sf::RenderWindow &window, int vertex_count, float radius, sf::Vector2f position, sf::Color color);

    void setVertecies();
    void walk(float displacement);
    void draw() const;
    bool isColliding(const Polygon &target) const;
    bool boundingCircleCollision(const Polygon &target) const;
    bool separatingAxisTheorem(const Polygon &target) const;

private:
    void m_drawLineToVertex0() const;
    std::vector<sf::Vector2f> m_computeNormals(const Polygon &poly) const;
    std::array<float, 2> m_calculateProjections(const Polygon &poly, const sf::Vector2f &axis) const;
};

#endif