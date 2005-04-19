#ifndef Dxf2KamH
#define Dxf2KamH

#include <Parsers\Kamea\Kamea.h>
#include <Parsers\Dxf\Dxf.h>

namespace Denisenko {
namespace Dxf2Kam {

class Convertor
{
public:
	Parsers::Kamea::Program Convert(const Parsers::Dxf::Database &in);
};

} // namespace Dxf2Kam
} // namespace Denisenko

#endif // Dxf2KamH