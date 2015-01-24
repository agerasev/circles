#include "circle.hpp"

#include <graphics/graphics.h>

Circle::Circle(
	double m, // mass
	double r, // radius
	const vec2 &p, // position
	unsigned c // color
) :
	mass(m), radius(r),
	pos(p), vel(nullvec2), 
	acc(nullvec2), color(c), 
	tl{static_cast<float>(p.x()),static_cast<float>(p.y())},
	tf{static_cast<float>(r),0.0f,0.0f,static_cast<float>(r)}, 
	rm(1.0f/static_cast<float>(m))
{
	
}

Circle::~Circle()
{
	
}

vec2 Circle::getPosition() const
{
	return pos;
}
vec2 Circle::getVelocity() const
{
	return vel;
}

void Circle::addForce(const vec2 &f)
{
	acc += f*rm;
}
void Circle::accelerate(const vec2 &a)
{
	acc += a;
}

void Circle::step(double dt)
{
	vel += acc*dt;
	pos += vel*dt;
	acc = nullfvec2;
}

double Circle::getMass() const
{
	return mass;
}
double Circle::getRadius() const
{
	return radius;
}
void Circle::draw() const
{
	tl[0] = static_cast<float>(pos.x());
	tl[1] = static_cast<float>(pos.y());
	
	setColor(color);
	translate(tl);
	transform(tf);
	circle();
}
