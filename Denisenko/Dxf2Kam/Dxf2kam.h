#ifndef Dxf2KamH
#define Dxf2KamH

#include "Kamea\Kamea.h"
#include "Dxf\Dxf.h"

namespace Denisenko {
namespace Dxf2Kam {

class EntityBase
{
public:
	virtual void AddToGraph(Graph &) = 0;
	virtual void Dispath(class Convertor &) = 0;
};

class Entity : EntityBase
{
	Dxf::Entity *entity;
public:
	int Handle;

	virtual void AddToGraph(Graph &);
	virtual void Dispath(class Convertor &);
};

class Line
{
	Dxf::Line *line;
public:
	float Start[3];
	float End[3];

	virtual void AddToGraph(Graph &);
	virtual void Dispath(class Convertor &);
};

class Circle
{
	Dxf::Circle *circle;
public:
	float Center[3];
	float Radius;
	virtual void AddToGraph(Graph &);
	virtual void Dispath(class Convertor &);
};

class Arc
{
	Dxf::Arc *arc;
public:
	float Start;
	float End;

	virtual void AddToGraph(Graph &);
	virtual void Dispath(class Convertor &);
};

class Ellipse
{
	Dxf::Ellipse *ellipse;
public:
	float Center[3];
	float Major[3];
	float Ratio;
	float Start;
	float End;

	virtual void AddToGraph(Graph &);
	virtual void Dispath(class Convertor &);
};

class Path : private Arcs, public EntityBase
{
public:
	virtual void AddToGraph(Graph &);
	virtual void Dispath(class Convertor &);
};

class Factory
{
	Factory();
public:
	Entity * Create(const Dxf::Entity &);
	Path * Create(const Arcs &);
	Factory & GetInstance() {static Factory f; return f;};
};

class Convertor
{
public:
	Kamea::Program Convert(const Dxf::Database &in);
};

} // namespace Dxf2Kam
} // namespace Denisenko

#endif // Dxf2KamH