#ifndef Dxf2KamH
#define Dxf2KamH

#include "Kamea\Kamea.h"
#include "Dxf\Dxf.h"
#include "Graph.h"

namespace Denisenko {
namespace Dxf2Kam {

class EntityBase
{
public:
	virtual void AddToGraph(Graph::Graph &) = 0;
	virtual void Dispath(class Convertor &) = 0;
};

class Entity : EntityBase
{
	Dxf::Entity *entity;
public:
	int Handle;

	virtual void AddToGraph(Graph::Graph &);
	virtual void Dispath(class Convertor &);
};

class Line
{
	Dxf::Line *line;
public:
	float Start[3];
	float End[3];

	virtual void AddToGraph(Graph::Graph &);
	virtual void Dispath(class Convertor &);
};

class Circle
{
	Dxf::Circle *circle;
public:
	float Center[3];
	float Radius;
	virtual void AddToGraph(Graph::Graph &);
	virtual void Dispath(class Convertor &);
};

class Arc
{
	Dxf::Arc *arc;
public:
	float Start;
	float End;

	virtual void AddToGraph(Graph::Graph &);
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

	virtual void AddToGraph(Graph::Graph &);
	virtual void Dispath(class Convertor &);
};

class Path : private Graph::t_Arcs, public EntityBase
{
public:
	virtual void AddToGraph(Graph::Graph &);
	virtual void Dispath(class Convertor &);
};

class Factory
{
	Factory();
public:
	Entity * Create(const Dxf::Entity &) {assert(0); return 0;};
	Path * Create(const Graph::t_Arcs &) {assert(0); return 0;};
	static Factory & GetInstance() {static Factory f; return f;};
};

class Convertor
{
public:
	Kamea::Program Convert(const Dxf::Database &in);
};

} // namespace Dxf2Kam
} // namespace Denisenko

#endif // Dxf2KamH