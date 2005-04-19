#ifndef DxfH
#define DxfH

namespace Denisenko {
namespace Dxf2Kam {
namespace Dxf {

typedef std::list<class Node*> Nodes;
typedef std::list<class Attribute*> Attributes;

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

	Type        GetType();

	Attribute(unsigned c, const char *value);
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

class Database : public Node
{
	std::string _xyz[10][3];
	std::stack<Node*> _stack;
    
public:
    void Load(const char *fileName);
	void Save(const char *fileName);
};

class ParseError
{
public:
	const char *GetMessageA();
};

} // namespace Dxf
} // namespace Dxf2Kam
} // namespace Denisenko

#endif // DxfH