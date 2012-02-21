#pragma once

#include <vector>
#include <cmath>

namespace MyGeometryTools {
	template <typename scalar> scalar sin(scalar);
	template <typename scalar> scalar cos(scalar);
	template <typename scalar> scalar tan(scalar);
	template <typename scalar> scalar atan2(scalar y, scalar x);
	template <typename scalar> scalar sqrt(scalar);
	//template <typename scalar> scalar abs(scalar);

	inline double sin(double x) {return ::sin(x);}
	inline float sin(float x) {return ::sinf(x);}

	inline double cos(double x) {return ::cos(x);}
	inline float cos(float x) {return ::cosf(x);}

	inline double tan(double x) {return ::tan(x);}
	inline float tan(float x) {return ::tanf(x);}

	inline double atan2(double y, double x) {return ::atan2(y, x);}
	inline float atan2(float y, float x) {return ::atan2f(y, x);}

	inline double sqrt(double x) {return ::sqrt(x);}
	inline float sqrt(float x) {return ::sqrtf(x);}

	/*inline double abs(double x) {return ::fabs(x);}
	inline float abs(float x) {return ::fabsf(x);}
	inline int abs(int x) {return std	::abs(x);}*/

/*#ifdef min
	#undef min
#endif

	template<class ty>
		ty & min(ty &a, ty &b) {return a > b ? a : b;}
	template<class ty>
		const ty & min(const ty &a, const ty &b) {return a > b ? a : b;}*/

	template<class scalar>
	class vec2
	{
	public:
		union {
			scalar coords[2];
			struct {
				scalar x, y;
			};
		};
		vec2(void) {}
		vec2(const scalar coords[2]) : x(coords[0]), y(coords[1]) {}
		vec2(scalar x, scalar y) : x(x), y(y) {}
		static vec2 by_len_ang(scalar len, scalar ang) {return vec2(cos(ang)*len, sin(ang)*len);}
		//vec2(scalar len, scalar ang, unsigned) : x(cos(ang)*len), y(sin(ang)*len) {}
		operator scalar * (void) {return coords;}
		operator const scalar * (void) const {return coords;}
		vec2 operator - (void) const {return vec2(-x, -y);}
		vec2 & operator *= (scalar k) {x *= k; y *= k; return *this;}
		vec2 & operator /= (scalar k) {x /= k; y /= k; return *this;}
		vec2 & operator += (const vec2 &vec) {x += vec.x; y += vec.y; return *this;}
		vec2 & operator -= (const vec2 &vec) {x -= vec.x; y -= vec.y; return *this;}
		vec2 operator + (const vec2 &vec) const {return vec2(x + vec.x, y + vec.y);}
		vec2 operator - (const vec2 &vec) const {return vec2(x - vec.x, y - vec.y);}
		vec2 operator * (scalar k) const {return vec2(k*x, k*y);}
		bool operator == (const vec2 &vec) const {return x == vec.x && y == vec.y;}
		bool operator != (const vec2 &vec) const {return x != vec.x || y != vec.y;}
		vec2 & reverse(void) {return *this =- *this;};
		scalar len(void) const {return sqrt(x*x + y*y);};
		scalar angle(void) const {return atan2(y, x);};
		//vec2 normalized(void) {double ln = len(); ln != 1 ? return *this/ln : return *this;}
		vec2 & normalize(void) {scalar ln = len(); return ln != 1 ? *this /= ln : *this;}
	};

	typedef vec2<float> vec2f;
	typedef vec2<double> vec2d;

	template<class scalar>
	scalar dotproduct(const vec2<scalar> &vec1, const vec2<scalar> &vec2) {return vec1.x*vec2.x+vec1.y*vec2.y;}

	template<class scalar>
	class vec3 {
	public:
		union {
			scalar coords[3];
			struct {
				scalar x, y, z;
			};
		};
		vec3(void) {}
		vec3(const scalar coords[3]) : x(coords[0]), y(coords[1]), z(coords[2]) {}
		vec3(const scalar coords[2], scalar z) : x(coords[0]), y(coords[1]), z(z) {}
		vec3(scalar x, scalar y, scalar z) : x(x), y(y), z(z) {}
		operator scalar * () {return coords;}
		vec3 operator + (const vec3 &b) {return vec3(x + b.x, y + b.y, z + b.z);}
		vec3 operator - (const vec3 &b) {return vec3(x - b.x, y - b.y, z - b.z);}
	};

	typedef vec3<float> vec3f;
	typedef vec3<double> vec3d;

	template <typename scalar> class matrix2
	{
	public:
		union {
			scalar matr[2][2];
			struct {
				scalar m11, m12;
				scalar m21, m22;
			};
		};
		matrix2(scalar m11, scalar m12, scalar m21, scalar m22)
			: m11(m11), m12(m12),
			m21(m21), m22(m22)
		{
		}

		static matrix2 rotate(scalar angle)
		{
			return matrix2(
				cos(angle), -sin(angle),
				sin(angle), cos(angle));
		}

		static matrix2 scale(scalar x, scalar y)
		{
			return matrix2(
				x, 0,
				0, y);
		}

		matrix2 operator * (scalar k)
		{
			return matrix2(
				k*m11, k*m12,
				k*m21, k*m22);
		}

		matrix2 operator * (const matrix2 &b)
		{
			return matrix2(
				m11*b.m11 + m12*b.m21, m11*b.m12 + m12*b.m22,
				m21*b.m11 + m22*b.m21, m21*b.m12 + m22*b.m22);
		}

		vec2<scalar> operator * (const vec2<scalar> &b)
		{
			return vec2<scalar>(m11*b.x + m12*b.y, m21*b.x + m22*b.y);
		}
	};

	typedef matrix2<float> matrix2f;
	typedef matrix2<double> matrix2d;

	/*template <typename scalar> class matrix3
	{
	public:
		scalar matr[3][3];
	};*/

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
	class Arc
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

		Arc(scalar center[2], scalar radius, scalar start_angle, scalar sweep_angle)	// Дуга по центру, радиусу, начальному углу и углу поворота
			: center(center), radius(abs(radius)), start_angle(start_angle), sweep_angle(sweep_angle)
		{
		}
		static Arc by_3_points(scalar p1[2], scalar p2[2], scalar p3[2]);	// Дуга по 3-м точкам
		static Arc by_2_pt_rad_rot(scalar p1[2], scalar p2[2], scalar radius, bool ccw, bool left);	// дуга по 2-м точкам, радиусу и направлению вращения, и расположению центра слева или справа от вектора (a1,a2) (радиус > 0)
		static Arc by_pt_rad_ang(scalar pt[2], scalar radius, scalar start_angle, scalar sweep_angle)	// Дуга по начальной точке, радиусу, начальному углу и углу поворота
		{
			scalar k = tan(start_angle);
			scalar rx = radius/sqrt(1+sqr(k)), ry = k*rx;
			vec2<scalar> center(pt[0] + rx, pt[1] + ry);
			return Arc(center, radius, start_angle, sweep_angle);
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

	typedef Arc<float> arcf;
	typedef Arc<double> arcd;

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