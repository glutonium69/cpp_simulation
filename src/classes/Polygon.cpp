#include "../include/classes/Polygon.hpp"
#include <cmath>
#include <algorithm>

Polygon::Polygon(sf::RenderWindow &window, int vertex_count, float radius, sf::Vector2f position, sf::Color color)
    : window(window), radius(radius)
{
    if (vertex_count < 3)
    {
        throw std::invalid_argument("Vertex count must be at least 3");
    }

    setPointCount(vertex_count);
    setFillColor(color);
    setPosition(position);
    setVertecies();
}

void Polygon::setVertecies()
{
    float angleIncrement = 2 * M_PI / (float)getPointCount();

    for (int i = 0; i < getPointCount(); i++)
    {
        float angle = i * angleIncrement;

        setPoint(i, {radius * cos(angle), radius * sin(angle)});
    }
}

void Polygon::walk(float displacement)
{
    setPosition(
        getPosition().x + displacement * cos(getRotation() * (M_PI / 180.0)),
        getPosition().y + displacement * sin(getRotation() * (M_PI / 180.0)));
}

void Polygon::draw() const
{
    window.draw(*this);
    m_drawLineToVertex0();
}

void Polygon::m_drawLineToVertex0() const
{
    sf::Vector2f center = getPosition();
    sf::Vector2f vertex0 = getPoint(0);
    sf::Transform transform = getTransform();

    vertex0 = transform.transformPoint(vertex0); // this will convert it from  local space to world space

    sf::Vertex verticies[] = {
        sf::Vertex(center, sf::Color::White),
        sf::Vertex(vertex0, sf::Color::White)};

    window.draw(verticies, 2, sf::Lines);
}

bool Polygon::isColliding(const Polygon &target) const
{
    if (boundingCircleCollision(target))
    {
        return separatingAxisTheorem(target);
    }

    return false;
}

bool Polygon::boundingCircleCollision(const Polygon &target) const
{
    sf::Vector2f this_pos = getPosition();
    sf::Vector2f target_pos = target.getPosition();

    float distance = sqrt(
        pow((this_pos.x - target_pos.x), 2) +
        pow((this_pos.y - target_pos.y), 2));

    return distance < (radius + target.radius);
}

bool Polygon::separatingAxisTheorem(const Polygon &target) const
{
    std::vector<sf::Vector2f> this_normals = m_computeNormals((*this));
    std::vector<sf::Vector2f> target_normals = m_computeNormals(target);

    // concat two vecs. from stack overflow.
    // vector1.insert( vector1.end(), vector2.begin(), vector2.end() );
    this_normals.insert(this_normals.end(), target_normals.begin(), target_normals.end());

    for (const sf::Vector2f &normal : this_normals)
    {
        std::array<float, 2> min_max_1 = m_calculateProjections((*this), normal);
        std::array<float, 2> min_max_2 = m_calculateProjections(target, normal);

        if (min_max_1[1] < min_max_2[0] || min_max_2[1] < min_max_1[0])
        {
            return false;
        }
    }

    return true;
}

std::vector<sf::Vector2f> Polygon::m_computeNormals(const Polygon &poly) const
{
    std::vector<sf::Vector2f> normals;

    for (int i = 0; i < poly.getPointCount(); i++)
    {
        sf::Vector2f curr_vertex = poly.getTransform().transformPoint(poly.getPoint(i));
        sf::Vector2f next_vertex = poly.getTransform().transformPoint(poly.getPoint((i + 1) % poly.getPointCount()));

        sf::Vector2f edge = next_vertex - curr_vertex;
        normals.push_back(sf::Vector2f(-edge.y, edge.x));
    }

    return normals;
}

std::array<float, 2> Polygon::m_calculateProjections(const Polygon &poly, const sf::Vector2f &axis) const
{
    std::array<float, 2> min_max;

    min_max[0] = INFINITY;
    min_max[1] = -INFINITY;

    for (int i = 0; i < poly.getPointCount(); i++)
    {
        sf::Vector2f curr_vertex = poly.getTransform().transformPoint(poly.getPoint(i));
        float projection_value = curr_vertex.x * axis.x + curr_vertex.y * axis.y;

        if (projection_value < min_max[0])
            min_max[0] = projection_value;
        if (projection_value > min_max[1])
            min_max[1] = projection_value;
    }

    return min_max;
}