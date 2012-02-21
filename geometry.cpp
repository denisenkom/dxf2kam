//#include "StdAfx.h"
#include <stdexcept>
#include <cassert>
#include "geometry.h"

namespace MyGeometryTools {

	unsigned Discretizator::discretization = 0;
	std::vector<double> Discretizator::sintable;
	std::vector<double> Discretizator::costable;

	class dummy {
	public:
		dummy() {
			Discretizator::SetDiscretization();
		}
	} dummy1;

	void Discretizator::SetDiscretization(unsigned new_discretization)
	{
		if (new_discretization < 4 && new_discretization%4 != 0)
			throw std::logic_error("«начение дискретизации в ф-ии SetDiscretization должно быть >= 4 и кратно 4");
		discretization = new_discretization;
		sintable.resize(new_discretization);
		costable.resize(new_discretization);
		for (unsigned i = 0; i < new_discretization; i++) {
			sintable[i] = ::sin(2*pi*i/new_discretization);
			costable[i] = ::cos(2*pi*i/new_discretization);
		}
	}

	// ошибки времени выполнени€
	const char *radius_must_be_greater_then_zero = "ѕри задании дуги радиус должен быть больше нул€.";
	const char *radius_too_small = "–адиус слишком малый.";
	const char *null_arc = "“очки дуги не должны распологатьс€ на одной пр€мой, также они не должны совпадать.";

/*#ifndef NDEBUG
	void Arc::__assert(void) {
		assert(this->radius > 0);
	}
#endif*/

	/*Arc::Arc(double cx, double cy, double radius, double start_angle, double sweep_angle, bool ccw)
		: cx(cx), cy(cy), radius(radius), start_angle(start_angle), sweep_angle(sweep_angle)
	{
		if (radius <= 0)
			throw std::runtime_error(radius_must_be_greater_then_zero);
		while (a1 < 0) a1 += 2*pi;
		while (a1 >= 2*pi) a1 -= 2*pi;
		this->a1 = a1;
		while (a2 < 0) a2 += 2*pi;
		while (a2 >= 2*pi) a2 -= 2*pi;
		this->a2 = a2;
		__assert();
	}*/

	/*Arc::Arc(double x1, double y1, double x2, double y2, double r, bool ccw, bool left)
		: ccw(ccw)
	{
		if (r <= 0)
			throw std::runtime_error(radius_must_be_greater_then_zero);
		if (x1 == x2 && y1 == y2)
			throw std::runtime_error(null_arc);
		double dx = x2-x1;
		double dy = y2-y1;
		if (sqrt(sqr(dx)+sqr(dy)) > 2*r)
			throw std::runtime_error(radius_too_small);
		radius = r;
		double len = sqrt(sqr(r)/(sqr(dx)+sqr(dy)) - 1.0/4.0);
		cx = x1 + dx*(1.0/2.0 - (ccw ? (len) : (-len)));
		cy = y1 + dy*(1.0/2.0 + (ccw ? (len) : (-len)));
		double ang1 = signz(y1-cx)*acos((x1-cx)/radius);
		double ang2 = signz(y2-cy)*acos((x2-cx)/radius);
		while (ang1 < 0) ang1 += 2*pi;
		while (ang1 >= 2*pi) ang1 -= 2*pi;
		this->a1 = ang1;
		while (ang2 < 0) ang2 += 2*pi;
		while (ang2 >= 2*pi) ang2 -= 2*pi;
		this->a2 = ang2;
		__assert();
	}*/

	/*Arc::Arc(double x1, double y1, double x2, double y2, double x3, double y3)
	{
		double det = 2*((x1-x3)*(y2-y3)-(x2-x3)*(y1-y3));
		if (det == 0)
			throw std::runtime_error(null_arc);
		double a = sqr(x1)-sqr(x3)+sqr(y1)-sqr(y3);
		double b = sqr(x2)-sqr(x3)+sqr(y2)-sqr(y3);
		double det_cx = a*(y2-y3)-b*(y1-y3);
		double det_cy = -a*(x2-x3)+b*(x1-x3);
		cx = det_cx/det;
		cy = det_cy/det;
		radius = sqrt(sqr(x1-cx)+sqr(y1-cy));
		double ang1 = signz(y1-cy)*acos((x1-cx)/radius);
		if (ang1 < 0) ang1 += 2*pi;
		double ang2 = signz(y2-cy)*acos((x2-cx)/radius);
		if (ang2 < 0) ang2 += 2*pi;
		double ang3 = signz(y3-cy)*acos((x3-cx)/radius);
		if (ang3 < 0) ang3 += 2*pi;
		assert(ang1 != ang2 && ang2 != ang3 && ang1 >= 0 && ang2 >= 0 && ang3 >= 0 && ang1 < 2*pi && ang2 < 2*pi && ang3 < 2*pi);
		start_angle = ang1;
		sweep_angle = ang3 - ang1;
		if (!(ang1 < ang2 && ang2 < ang3 || ang1 > ang2 && ang2 > ang3))
			sweep_angle += (ang1 < ang3) ? -2*pi : 2*pi;
		assert(sweep_angle < 2*pi && sweep_angle > -2*pi);
		__assert();
	}*/

	/*void Arc::vectorize(Vectorizator<double> &vectorizator)
	{
		__assert();
		assert(Discretizator::GetDiscretization() >= 4 && Discretizator::GetDiscretization()%4 == 0);
		vectorizator.moveto(static_cast<double>(cx+::cos(a1)*radius), static_cast<double>(cy+::sin(a1)*radius));
		unsigned i1 = static_cast<unsigned>(ceil(a1/2/pi*Discretizator::GetDiscretization()));
		unsigned i2 = static_cast<unsigned>(floor(a2/2/pi*Discretizator::GetDiscretization()));
		// a2<a1 необходимо т.к. дл€ малых углов дуги возможна ситуаци€ когда
		// i2<i1 и a1>a2, и в таком случае не нужно мен€ть местами i1 и i2
		// т.к. така€ дуга не вырождаетс€ в отрезок и цикл не должен сработать
		if (i2 < i1 && a2 < a1)
		{
			unsigned tmp = i2;
			i2 = i1;
			i1 = tmp;
		}
		for(unsigned i = i1; i <= i2; i++)
			vectorizator.lineto(static_cast<double>(cx+Discretizator::cos(i)*radius),
				static_cast<double>(cy+Discretizator::sin(i)*radius));
		vectorizator.lineto(static_cast<double>(cx+::cos(a2)*radius), static_cast<double>(cy+::sin(a2)*radius));
	}*/
}