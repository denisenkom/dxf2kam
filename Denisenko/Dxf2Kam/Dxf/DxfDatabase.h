#ifndef DxfDatabaseH
#define DxfDatabaseH

#include <vector>

namespace Denisenko {
namespace Dxf2Kam {
namespace Dxf {

typedef std::map<std::string, class Node*> Nodes;
typedef std::map<unsigned, class Attribute> Attributes;

class Node
{
public:
	Nodes Children;
	Attributes Attributes;
};

class Attribute : public Node
{
public:
	enum Type {String, Short, Long, Double, Boolean, Vector};

	const char *GetName();

	//explicit operator (const char *());
	operator int();
	operator float();
	operator float*();

	const char *GetValue();

	Type        GetType() {return _type;};

	unsigned GetCode() {return _code;};

	Attribute(unsigned c, std::string value);
	Attribute(int rem, std::string value[3]);

private:
	unsigned _code;
	Type _type;
	std::string _sVal[3];
	union {
		long _lVal;
		short _shVal;
		double _dVal;
		bool _bVal;
		double _vecVal[3];
	};
};

class Entity : public Node
{
public:
	Entity(Node &source);

	Attribute& Type();          // 0 Entity type

	Attribute& Handle();        // 5 Handle

	Attribute& LayerName();     // 8 Layer name

	Attribute& LinetypeName();  // 6 Linetype name (present if not BYLAYER). The special name BYBLOCK indicates a floating linetype (optional)

	// <summary>
	// Code: 62
	// Color number (present if not BYLAYER); zero indicates the BYBLOCK (floating) color;
	// 256 indicates BYLAYER; a negative value indicates that the layer is turned off (optional) 
	// </summary>
	Attribute& ColorNumber(); 

	Attribute& Lineweight();    // 370 Lineweight enum value. Stored and moved around as a 16-bit integer.

	Attribute& LinetypeScale(); // 48 Linetype scale (optional) (1.0)

	Attribute& Visibility();    // 60 Object visibility (optional): 0 = Visible; 1 = Invisible (0)
};

typedef std::vector<Entity*> Entities;
typedef Entities::iterator EntitiesIt;

class Line : public Entity
{
public:
	Attribute& Thickness();  // 39 Thickness (optional; default = 0)

	Attribute& StartPoint(); // 10 Start point (in WCS)

	Attribute& EndPoint();   // 11 Endpoint (in WCS)
};

class Point : public Entity
{
public:
	Attribute& Location();   // 10 Point location (in WCS)

	Attribute& Thickness();  // 39 Thickness (optional; default = 0)
};

class Circle : public Entity
{
public:
	Attribute& Thickness();  // 39 Thickness (optional; default = 0)

	Attribute& Center();     // 10 Center point (in OCS)

	Attribute& Radius();     // 40 Radius
};

class Arc : public Circle
{
public:
	Attribute& StartAngle(); // 50 Start angle

	Attribute& EndAngle();   // 51 End angle
};

class Ellipse : public Entity
{
public:
	Attribute& Center();   // 10 Center point (in WCS)

	Attribute& MajorEnd(); // 11 Endpoint of major axis, relative to the center (in WCS)

	Attribute& Ratio();    // 40 Ratio of minor axis to major axis

	Attribute& Start();    // 41 Start parameter (this value is 0.0 for a full ellipse)

	Attribute& End();      // 42 End parameter (this value is 2pi for a full ellipse)
};

class Database : public Node
{
	std::string _xyz[10][3];
	std::stack<Node*> _stack;
    
public:
	Nodes Sections;

	Nodes::iterator FindSection(const char *name);

	Entities GetEntities() const;

    void Load(const char *fileName);

	void Save(const char *fileName);
};

}
}
}

#endif // DxfDatabaseH