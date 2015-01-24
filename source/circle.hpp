#pragma once

#include <vec.hpp>

class Circle
{
private:

	double mass;
	double radius;
	
	vec2 pos;
	vec2 vel;
	vec2 acc;
	
	unsigned color;
	
	mutable float tl[2];
	mutable float tf[4];
	mutable double rm;
	
public:

	Circle(
		double m, // mass
		double r, // radius
		const vec2 &p, // position
		unsigned c // color
	);
	~Circle();
	
	vec2 getPosition() const;
	vec2 getVelocity() const;
	
	void addForce(const vec2 &f);
	void accelerate(const vec2 &a);
	
	void step(double dt);
	
	double getMass() const;
	double getRadius() const;
	void draw() const;
};
