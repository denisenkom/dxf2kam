#ifndef GraphH
#define GraphH

namespace Denisenko {
namespace Dxf2Kam {

class Graph
{
public:
	struct Arc {unsigned short Begin; unsigned short End; int Weight;};
	typedef std::vector<int> Nodes;
	typedef std::vector<Arc> Arcs;

	Arcs Arcs;
	Nodes Nodes;

	// <summary>
	// Finds hamilton circuit for given graph
	// </symmary>
	Arcs FindHamilton()
	{
	}


	Traverse()
};

} // namespace Dxf2Kam
} // namespace Denisenko

#endif // GraphH