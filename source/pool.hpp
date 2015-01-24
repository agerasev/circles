#pragma once

#include <list>
#include <functional>

#include "circle.hpp"

class Pool
{
private:
	std::list<Circle*> list;
	double elast;
	double frict;
	
	double drag_elast;
	double drag_frict;
	
	double tb,bb,lb,rb;
	
	std::list<Circle*> fix_list;
	vec2 fix_point;
	bool fixed = false;
	
public:
	Pool(double el, double fr, double del, double dfr);
	~Pool();
	
	void addCircle(Circle *c);
	void removeCircle(Circle *c);
	void forEachCircle(std::function<void(Circle*)> func);
	
	void setBorders(double t, double l, double b, double r);
	
	void grabCircles(const vec2 &p);
	void dragCircles(const vec2 &p);
	void dropCircles();
	
	void accelerate(const vec2 &a);
	void collide();
	void interact();
	void step(double dt);
	
	void draw() const;
};
