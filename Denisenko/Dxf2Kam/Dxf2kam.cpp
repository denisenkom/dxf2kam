#include "StdAfx.h"
#include "Dxf2Kam.h"

#if 0

namespace mgt=MyGeometryTools;
using mgt::vec2f;
using mgt::vec3f;
using mgt::matrix2f;

namespace Denisenko
{
namespace Dxf2Kam
{
	class dispatcher {
	public:
		virtual void line(class line &line) = 0;
		virtual void arc(class arc &arc) = 0;
		virtual void ellipse(class ellipse &ellipse) = 0;
		virtual void circle(class circle &circle) = 0;
		virtual void point(class point &point) = 0;
		virtual void spline(class spline &) = 0;
	};

	class entity {
	public:
		int handle;
		int color;
		double thickness;
		entity(int handle, int color=0, double thickness=0)
			: handle(handle), color(color), thickness(thickness)
		{
		}
		virtual void dispatch(dispatcher&) = 0;
		virtual float range2d(float vec[2]) = 0;	// for optimizer
	};

	class line : public entity {
	public:
		mgt::vec3f p1, p2;
		line(int handle, float p1[3],
			float p2[3], float thickness=0, int color=0)
			: entity(handle, color, thickness), p1(p1), p2(p2)
		{
		}
		virtual void dispatch(dispatcher &dispatcher) {dispatcher.line(*this);}
		virtual float range2d(float vec[2])
		{
			return mgt::min((vec2f(p1) - vec2f(vec)).len(), (vec2f(p2) - vec2f(vec)).len());
		}
	};

	class arc : public entity, mgt::arcf {
		mgt::vec3f p1, p2;
	public:
		arc(int handle, float center[3],
			float radius, float start_angle, float end_angle,
			float thickness=0, int color=0)
			: entity(handle, color, thickness), mgt::arcf(center, radius, start_angle, end_angle - start_angle),
			p1(calcStartPoint(), center[2]), p2(calcEndPoint(), center[2])
		{
		}

		virtual void dispatch(dispatcher &dispatcher) {dispatcher.arc(*this);}
		virtual float range2d(float vec[2])
		{
			return mgt::min((vec2f(p1) - vec2f(vec)).len(), (vec2f(p2) - vec2f(vec)).len());
		}
		float getRadius(void) {return mgt::arcf::getRadius();}
		vec3f getStartPoint(void) {return p1;}
		vec3f getEndPoint(void) {return p2;}
		float & z1(void) {return p1.z;}
		float z1(void) const {return p1.z;}
		float & z2(void) {return p2.z;}
		float z2(void) const {return p2.z;}
		float getStartAngle(void) {return start_angle;}
		float getSweepAngle(void) {return sweep_angle;}
		void reverse(void) {std::swap(p1, p2); start_angle += sweep_angle; sweep_angle = -sweep_angle;}
	};

	class circle : public entity {
		float radius;
	public:
		vec3f center;
		circle(int handle, float center[3],
			float radius, float thickness=0, int color=0)
			: entity(handle, color, thickness), center(center),
			radius(mgt::abs(radius))
		{
		}
		virtual void dispatch(dispatcher &dispatcher) {dispatcher.circle(*this);}
		virtual float range2d(float vec[2]) {return (vec2f(center) - vec2f(vec)).len() - radius;};
		float getRadius(void) {return radius;}
	};

	class ellipse : public entity {
		vec3f p1, p2;
		vec2f center, major;
		float ratio, start_angle, sweep_angle;
	public:
		ellipse(int handle, float vcenter[3], float majr[3],
			float ratio, float start_angle, float end_angle,
			float thickness=0, int color=0)
			: entity(handle, color, thickness), center(vcenter), major(majr),
			ratio(ratio), start_angle(start_angle), sweep_angle(end_angle - start_angle)
		{
			mgt::arcf arc(vec2f(0, 0), major.len(), start_angle, sweep_angle);
			matrix2f mtform = matrix2f::rotate(major.angle())*matrix2f::scale(1, ratio);
			p1 = vec3f(center + mtform*arc.calcStartPoint(), 0);
			p2 = vec3f(center + mtform*arc.calcEndPoint(), 0);
		}
		virtual void dispatch(dispatcher &dispatcher) {dispatcher.ellipse(*this);}
		virtual float range2d(float vec[2])
		{
			return mgt::min((vec2f(p1) - vec2f(vec)).len(), (vec2f(p2) - vec2f(vec)).len());
		}
		vec3f getStartPoint(void) const {return p1;}
		vec3f getEndPoint(void) const {return p2;}
		vec2f getMajor(void) const {return major;}
		float getRatio(void) const {return ratio;}
		float getStartAngle(void) const {return start_angle;}
		float getSweepAngle(void) const {return sweep_angle;}
		float & z1(void) {return p1.z;}
		float & z2(void) {return p2.z;};
	};

	class point : public entity, public vec3f {
	public:
		point(int handle, vec3f pt, float thickness=0, int color=0)
			: entity(handle, color, thickness), vec3f(pt)
		{
		}
		virtual void dispatch(dispatcher &dispatcher) {dispatcher.point(*this);}
		virtual float range2d(float vec[2]) {return (vec2f(*this) - vec2f(vec)).len();}
	};

	class spline : public entity {
	public:
		spline(int handle, float thickness, int color) : entity(handle, color, thickness) {}
		virtual void dispatch(dispatcher &dispatcher) {dispatcher.spline(*this);}
		virtual float range2d(float vec[2]) {return 10000;}
	};

	typedef std::vector<entity*> t_entities_vector;
	typedef std::list<entity*> t_entities_list;

	class factory : public dxf::factory
	{
	public:
		typedef t_entities_list t_entities;
		t_entities entities;

		virtual void circle(int handle, float center[3], float radius, float thickness=0, int color=0)
		{
			entities.push_back(new class circle(handle, center, radius, thickness, color));
		}
		virtual void arc(int handle, float center[3], float radius, float start_angle, float end_angle, float thickness=0, int color=0)
		{
			entities.push_back(new class arc(handle, center, radius, start_angle, end_angle, thickness, color));
		}
		virtual void line(int handle, float p1[3], float p2[3], float thickness=0, int color=0)
		{
			entities.push_back(new class line(handle, p1, p2, thickness, color));
		}
		virtual void ellipse(int handle, float center[3], float major[3], float ratio, float start_angle, float end_angle, float thickness=0, int color=0)
		{
			entities.push_back(new class ellipse(handle, center, major, ratio, start_angle, end_angle, thickness, color));
		}
		virtual void point(int handle, float pt[3], float thickness, int color)
		{
			entities.push_back(new class point(handle, pt, thickness, color));
		}
		virtual void spline(int handle, float thickness, int color)
		{
			entities.push_back(new class spline(handle, thickness, color));
		}
	};

	using std::istream;

	class convertor : public dispatcher, public Kamea::program_writer {
		virtual void circle(class circle &circle);
		virtual void arc(class arc &arc);
		virtual void line(class line &line);
		virtual void ellipse(class ellipse &ellipse);
		virtual void point(class point &);
		virtual void spline(class spline &);
		t_entities_list entities;
		t_entities_list::iterator mini;
		void optimize();
	public:
		Kamea::program convert(istream &);
	};
}

void dxf2kam::convertor::arc(class arc &arc)
{
	if ((vec2f(arc.getStartPoint()) - vec2f(getPos())).len() > (vec2f(arc.getEndPoint()) - vec2f(getPos())).len())
		arc.reverse();
	moveto(arc.getStartPoint());
	setSpeed(cut_speed);
	displace(vec3f(0, 0, arc.z1() - getPos().z));
	if (arc.z1() == arc.z2())
	{
		program_writer::arc(arc.getRadius(), arc.getStartAngle(), arc.getSweepAngle());
	}
	else
	{
		//program_writer::arc();
	}
	entities.erase(mini);
}

void dxf2kam::convertor::circle(class circle &circle)
{
	vec2f pos(getPos());
	vec2f dir = vec2f(circle.center) - pos;
	float len = dir.len() - circle.getRadius();
	dir.normalize() *= len;

	moveto(pos + dir);
	setSpeed(cut_speed);
	displace(vec3f(0, 0, circle.center.z - getPos().z));
	//displace(vec3f(0, 0, circle.center.z - getPos().z));
	program_writer::arc(circle.getRadius(), float((-dir).angle()), float(2*mgt::pi));
	entities.erase(mini);
}

void dxf2kam::convertor::line(class line &line)
{
	if ((vec2f(line.p1) - vec2f(getPos())).len() > (vec2f(line.p2) - vec2f(getPos())).len())
		std::swap(line.p1, line.p2);
	moveto(vec2f(line.p1));
	setSpeed(cut_speed);
	displace(vec3f(0, 0, line.p1.z - getPos().z));
	displace(line.p2 - line.p1);
	entities.erase(mini);
}

void dxf2kam::convertor::ellipse(class ellipse &ellipse)
{
	moveto(ellipse.getStartPoint());
	setSpeed(cut_speed);
	displace(vec3f(0, 0, ellipse.getStartPoint().z - getPos().z));
	setRotate(ellipse.getMajor().angle());
	setScale(vec3f(1, ellipse.getRatio(), 1));
	program_writer::arc(ellipse.getMajor().len(), ellipse.getStartAngle(), ellipse.getSweepAngle());
	setScale(vec3f(1, 1, 1));
	setRotate(0);
	entities.erase(mini);
}

void dxf2kam::convertor::point(class point &point)
{
	moveto(vec2f(point));
	setSpeed(cut_speed);
	displace(vec3f(0, 0, point.z - getPos().z));
	entities.erase(mini);
}

void dxf2kam::convertor::spline(class spline &spline)
{
	entities.erase(mini);
}

void dxf2kam::convertor::optimize()
{
	while (1)
	{
		bool first = true;
		t_entities_list::iterator entityi;
		float minrange;
		for (entityi = entities.begin(); entityi != entities.end(); entityi++)
		{
			float range = (*entityi)->range2d(getPos());
			if (first || range < minrange)
			{
				minrange = range;
				mini = entityi;
				first = false;
				if (minrange == 0)
					break;
			}
		}
		if (!first)
			(*mini)->dispatch(*this);
		else
			return;
	}
}

Kamea::program dxf2kam::convertor::convert(istream &stream)
{
	factory factory;
	dxf::parse(stream, factory);
	entities = factory.entities;
	begin();
	optimize();
	return end();
}

Kamea::program dxf2kam::convert(istream &input)
{
	convertor convertor;
	return convertor.convert(input);
}
} // namespace Dxf2Kam
} // namespace Denisenko

#endif