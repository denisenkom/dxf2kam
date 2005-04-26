#ifndef DxfH
#define DxfH

#include <map>
#include <istream>
#include "DxfDatabase.h"

namespace Denisenko {
namespace Dxf2Kam {
namespace Dxf {


enum Manipulator {
	Eof,
	EndSection,
	EndTable,
	EndBlock,
	BeginNode
};

class Constructor
{
	std::string _xyz[10][3];
	std::stack<Node*> _stack;

public:
	Constructor& operator << (Manipulator);
	Constructor& operator << (Attribute &);
};

class Parser
{
public:
	static void Parse(std::istream &, Constructor &);
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