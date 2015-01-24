#include "pool.hpp"

#include <graphics/graphics.h>

Pool::Pool(double el, double fr, double del, double dfr) :
	elast(el), frict(fr),
	drag_elast(del), drag_frict(dfr)
{
	
}
Pool::~Pool()
{
	
}

void Pool::addCircle(Circle *c)
{
	list.push_back(c);
}
void Pool::removeCircle(Circle *c)
{
	list.remove(c);
}
void Pool::forEachCircle(std::function<void(Circle*)> func)
{
	for(Circle *c : list)
	{
		func(c);
	}
}

void Pool::setBorders(double t, double l, double b, double r)
{
	tb = t;
	bb = b;
	lb = l;
	rb = r;
}

void Pool::grabCircles(const vec2 &p)
{
	fix_point = p;
	for(Circle *c : list)
	{
		vec2 dir = c->getPosition() - fix_point;
		if(dir*dir < c->getRadius()*c->getRadius())
		{
			fix_list.push_back(c);
		}
	}
	fixed = true;
}
void Pool::dragCircles(const vec2 &p)
{
	fix_point = p;
}
void Pool::dropCircles()
{
	fixed = false;
	fix_list.clear();
}

void Pool::accelerate(const vec2 &a)
{
	for(Circle *c : list)
	{
		c->accelerate(a);
	}
}
void Pool::collide()
{
	for(Circle *c : fix_list)
	{
		vec2 dir = c->getPosition() - fix_point;
		c->addForce(-drag_elast*dir - drag_frict*c->getVelocity());
	}
	
	for(Circle *c : list)
	{
		double dist;
		dist = tb - c->getPosition().y() - c->getRadius();
		if(dist < 0.0)
		{
			c->addForce(elast*vec2(0.0,dist) + frict*vec2(0.0,c->getVelocity().y())*dist);
		}
		dist = c->getPosition().y() - c->getRadius() - bb;
		if(dist < 0.0)
		{
			c->addForce(elast*vec2(0.0,-dist) + frict*vec2(0.0,c->getVelocity().y())*dist);
		}
		dist = rb - c->getPosition().x() - c->getRadius();
		if(dist < 0.0)
		{
			c->addForce(elast*vec2(dist,0.0) + frict*vec2(c->getVelocity().x(),0.0)*dist);
		}
		dist = c->getPosition().x() - c->getRadius() - lb;
		if(dist < 0.0)
		{
			c->addForce(elast*vec2(-dist,0.0) + frict*vec2(c->getVelocity().x(),0.0)*dist);
		}
	}
}
void Pool::interact()
{
	for(Circle *a : list)
	{
		for(Circle *b : list)
		{
			if(a == b)
			{
				break;
			}
			vec2 dir = a->getPosition() - b->getPosition();
			double dist = dir*dir;
			double coll_dist = a->getRadius() + b->getRadius();
			if(dist < coll_dist*coll_dist)
			{
				dist = sqrt(dist);
				dir = dir/dist;
				double dev = coll_dist - dist;
				vec2 rel_vel = a->getVelocity() - b->getVelocity();
				a->addForce((elast*dev - frict*(dir*rel_vel)*dev)*dir);
				b->addForce((-elast*dev + frict*(dir*rel_vel)*dev)*dir);
			}
		}
	}
}
void Pool::step(double dt)
{
	for(Circle *c : list)
	{
		c->step(dt);
	}
}
void Pool::draw() const
{
	for(Circle *c : list)
	{
		c->draw();
	}
}
