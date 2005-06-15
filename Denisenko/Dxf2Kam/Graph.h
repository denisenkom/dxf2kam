#ifndef GraphH
#define GraphH

namespace Denisenko {
namespace Dxf2Kam {
namespace Graph {

struct Arc {unsigned short Begin; unsigned short End; int Weight;};
typedef std::vector<int> t_Nodes;
typedef std::vector<Arc> t_Arcs;

class Graph
{
public:
	t_Arcs Arcs;
	t_Nodes Nodes;

	// <summary>
	// Finds hamilton circuit for given graph
	// </symmary>
	t_Arcs FindHamilton() {
		assert(0);
		return t_Arcs();
	}

	t_Arcs FindBestPath() {
		assert(0);
		return t_Arcs();
	}

	void Traverse();

	bool IsEmpty() {return (Nodes.size() == 0);}

	Graph& operator -=(const t_Arcs &path) {
		assert(0);
		return *this;
	}
};

} // namespace Graph
} // namespace Dxf2Kam
} // namespace Denisenko

#endif // GraphH