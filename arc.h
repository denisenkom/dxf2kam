#pragma once
#include <math.h>
#include <vector>

#include "geometry.h"

namespace MyGeometryTools {

	template<class scalar>
	inline scalar sqr(scalar x) {return x*x;}	// квадрат x

	template<class scalar>
	inline scalar sign(scalar x) {	// знак x, f(x>0)=+1, f(0)=0, f(x<0)=-1
		return x > 0 ? 1 : x < 0 ? -1 : 0;
	}

	template<class scalar>
	inline scalar signz(scalar x) {	// знак x без 0, f(x>=0)=+1, f(x<0)=-1
		return x >= 0 ? +1 : -1;
	}

	const double pi = 3.14159265358979323846;

	template<typename scalar> scalar grad2rad(scalar x) {return scalar(x*pi/180.0);}

	template <class scalar>
	class Vectorizator {
	public:
		virtual void moveto(scalar x, scalar y) = 0;
		virtual void lineto(scalar x, scalar y) = 0;
	};

	template <class scalar>
	class arc
	{
		scalar radius;	// радиус дуги >0
		//double a1, a2;	// начальный и конечный углы дуги
		//bool ccw;	// вращение против часовой стрелки

#ifndef NDEBUG
		void __assert(void);
#endif

	public:
		vec2<scalar> center;
		scalar start_angle, sweep_angle;

		arc(scalar center[2], scalar radius, scalar start_angle, scalar sweep_angle)	// Дуга по центру, радиусу, начальному углу и углу поворота
			: center(center), radius(abs(radius)), start_angle(start_angle), sweep_angle(sweep_angle)
		{
		}
		static arc by_3_points(scalar p1[2], scalar p2[2], scalar p3[2]);	// Дуга по 3-м точкам
		static arc by_2_pt_rad_rot(scalar p1[2], scalar p2[2], scalar radius, bool ccw, bool left);	// дуга по 2-м точкам, радиусу и направлению вращения, и расположению центра слева или справа от вектора (a1,a2) (радиус > 0)
		static arc by_pt_rad_ang(scalar pt[2], scalar radius, scalar start_angle, scalar sweep_angle)	// Дуга по начальной точке, радиусу, начальному углу и углу поворота
		{
			scalar k = tan(start_angle);
			scalar rx = radius/sqrt(1+sqr(k)), ry = k*rx;
			vec2<scalar> center(pt[0] + rx, pt[1] + ry);
			return arc(center, radius, start_angle, sweep_angle);
		}

		void vectorize(Vectorizator<double> &vectorizator);	// векторизация дуги

		vec2d normal(scalar pt[2]) {return (vec2<scalar>(pt[0], pt[1])-center).normalize();}
		//vec2d getCenter(void) {return vec2d(cx,cy);}
		scalar getRadius(void) {return radius;}
		//double getStartAng(void) {return start_angle;}
		scalar getEndAng(void) {return start_angle + sweep_angle;}
		//double getSweepAng(void) {return sweep_angle;}
		bool isCCW(void) {return sweep_angle>0;}
		vec2<scalar> calcStartPoint(void) {return vec2<scalar>::by_len_ang(radius, start_angle) + center;}
		vec2<scalar> calcEndPoint(void) {return vec2<scalar>::by_len_ang(radius, start_angle+sweep_angle) + center;}
	};

	typedef arc<float> arcf;
	typedef arc<double> arcd;

	class Discretizator {
		static unsigned discretization;
		static std::vector<double> sintable, costable;

	public:
		static void SetDiscretization(unsigned new_discretization=32);
		static unsigned GetDiscretization() {return discretization;};
		static double cos(unsigned i) {return costable[i%discretization];};
		static double sin(unsigned i) {return sintable[i%discretization];};
	};

	// inline definations
}