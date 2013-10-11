#include "StdAfx.h"
#include "DxfDatabase.h"

namespace Denisenko {
namespace Dxf2Kam {
namespace Dxf {

Entity::Entity(Node &source)
{
}

#define DEFINE_METHOD(cls, meth, id) Attribute& cls::meth() {return Attributes.find(id)->second;}

DEFINE_METHOD(Entity, Type,           0)
DEFINE_METHOD(Entity, Handle,         5)
DEFINE_METHOD(Entity, LayerName,      8)
DEFINE_METHOD(Entity, LinetypeName,   6)
DEFINE_METHOD(Entity, ColorNumber,   62)
DEFINE_METHOD(Entity, Lineweight,   370)
DEFINE_METHOD(Entity, LinetypeScale, 48)
DEFINE_METHOD(Entity, Visibility,    60)

DEFINE_METHOD(Line, Thickness,    39)
DEFINE_METHOD(Line, StartPoint,    10)
DEFINE_METHOD(Line, EndPoint,    11)

DEFINE_METHOD(Point, Location,    10)
DEFINE_METHOD(Point, Thickness,    39)

DEFINE_METHOD(Circle, Thickness,    39)
DEFINE_METHOD(Circle, Center,    10)
DEFINE_METHOD(Circle, Radius,    40)

DEFINE_METHOD(Arc, StartAngle,    50)
DEFINE_METHOD(Arc, EndAngle,    51)


#undef DEFINE_METHOD

}
}
}