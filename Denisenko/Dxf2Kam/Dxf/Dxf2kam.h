#ifndef Dxf2KamH
#define Dxf2KamH

#include "Kamea\Kamea.h"
#include "Dxf\Dxf.h"

namespace Denisenko {
namespace Dxf2Kam {

class Convertor
{
public:
	Kamea::Program Convert(const Dxf::Database &in);
};

} // namespace Dxf2Kam
} // namespace Denisenko

#endif // Dxf2KamH