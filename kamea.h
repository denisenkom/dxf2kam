#ifndef KameaH
#define KameaH

namespace Denisenko {
namespace Dxf2Kam {
namespace Kamea {

class Statement
{
public:
	// opcodes 1 byte
	enum OpcodeEnum {PPLine = 0x00, PPArc = 0x01, PRArc = 0x02, PZArc = 0x03,
		PRZArc = 0x1e, Line = 0x04, Arc = 0x06, RelArc = 0x07, SetPark = 0x08, 
		GoPark = 0x09, SetZero = 0x0a, GoZero = 0x0b, On = 0x0c,
		Off = 0x0d, Speed = 0x0e, ScaleX = 0x0f, ScaleY = 0x10,
		ScaleZ = 0x11, Turn = 0x12, Label = 0x13, Call = 0x14,
		Ret = 0x15, Goto = 0x16, Loop = 0x17, EndLoop = 0x18,
		Stop = 0x19, Finish = 0x1a, Comment = 0x1b, Pause = 0x1c,
		Sub = 0x1f, Spline = 0x28};	// SPLINE - в новой версии программы

	OpcodeEnum Opcode;

	Statement(OpcodeEnum opc) : Opcode(opc) {}
	virtual void Write(FILE *stream) {fwrite(&Opcode, 1, 1, stream);}
};

typedef std::list<Statement*> Statements;

class Speed : public Statement
{
public:
	unsigned Value;
	Speed(unsigned spd, OpcodeEnum c = Statement::Speed) : Statement(c), Value(spd) {}
	virtual void Write(FILE *stream) {fwrite(&Opcode, 1, 1, stream);}
};

class Line : public Speed
{
public:
	float DeltaX, DeltaY, DeltaZ;
	Line(float dx, float dy, float dz, unsigned spd)
		: Speed(spd, Statement::Line), DeltaX(dx), DeltaY(dy), DeltaZ(dz) {}
};

class PPLine : public Speed
{
public:
	unsigned PointRefs[2];
	float DeltaZ;
	bool UpDown;
	PPLine(unsigned ptRefs[2], float dz, bool upDown, unsigned spd)
		: Speed(spd, Statement::PPLine), UpDown(upDown), DeltaZ(dz)
	{
		PointRefs[0] = ptRefs[0];
		PointRefs[1] = ptRefs[1];
	}
};

class PPArc : public Speed
{
public:
	unsigned PointRefs[3];
	PPArc(unsigned ptRefs[3], unsigned spd)
		: Speed(spd, Statement::PPArc)
	{
		PointRefs[0] = ptRefs[0];
		PointRefs[1] = ptRefs[1];
		PointRefs[2] = ptRefs[2];
	}
};

class PRArc : public Speed
{
public:
	unsigned PointRefs[2];
	float Radius;
	PRArc(unsigned ptRefs[2], float rad, unsigned spd)
		: Speed(spd, Statement::PRArc), Radius(rad)
	{
		PointRefs[0] = ptRefs[0];
		PointRefs[1] = ptRefs[1];
	}
};

class PZArc : public Speed
{
public:
	unsigned PointRefs[3];
	float DeltaZ;
	PZArc(unsigned ptRefs[3], float dz, unsigned spd)
		: Speed(spd, Statement::PZArc), DeltaZ(dz)
	{
		PointRefs[0] = ptRefs[0];
		PointRefs[1] = ptRefs[1];
		PointRefs[2] = ptRefs[2];
	}
};

class PRZArc : public Speed
{
public:
	unsigned PointRefs[2];
	float Radius;
	float DeltaZ;
	PRZArc(unsigned ptRefs[2], float rad, float dz, unsigned spd)
		: Speed(spd, Statement::PRZArc), Radius(rad), DeltaZ(dz)
	{
		PointRefs[0] = ptRefs[0];
		PointRefs[1] = ptRefs[1];
	}
};

class Arc : public Speed
{
public:
	float Radius;
	float StartAngle;
	float AngleSpan;
	Arc(float rad, float startAngle, float angleSpan, unsigned spd)
		: Speed(spd, Statement::Arc), Radius(rad), StartAngle(startAngle), AngleSpan(angleSpan)
	{
	}
};

class RelArc : public Speed
{
public:
	float DeltaX;
	float DeltaY;
	float Radius;
	RelArc(float dx, float dy, float rad, unsigned spd)
		: Speed(spd, Statement::RelArc), DeltaX(dx), DeltaY(dy), Radius(rad) {}
};

class Scale : public Statement
{
public:
	unsigned Old;
	unsigned New;
	bool Relative;

	Scale(OpcodeEnum c, unsigned old, unsigned nw, bool rel) : Statement(c), Old(old), New(nw), Relative(rel)
	{
		assert(c == ScaleX || c == ScaleY || c == ScaleZ);
	}
};

class ScaleX : public Scale
{
public:
	ScaleX(unsigned old, unsigned nw, bool rel) : Scale(Statement::ScaleX, old, nw, rel) {}
};

class ScaleY : public Scale
{
public:
	ScaleY(unsigned old, unsigned nw, bool rel) : Scale(Statement::ScaleX, old, nw, rel) {}
};

class ScaleZ : public Scale
{
public:
	ScaleZ(unsigned old, unsigned nw, bool rel) : Scale(Statement::ScaleX, old, nw, rel) {}
};

class OnOff : public Statement
{
public:
	unsigned Device;

	OnOff(OpcodeEnum c, unsigned device) : Statement(c), Device(device)
	{
		assert(c == On || c == Off);
	}
};

class Turn : public Statement
{
public:
	bool MirrorX;
	bool MirrorY;
	float Angle;

	Turn(bool mirrorX, bool mirrorY, float angle)
		: Statement(Statement::Turn), MirrorX(mirrorX), MirrorY(mirrorY), Angle(angle) {}
};

class Label : public Statement
{
public:
	std::string Value;

	Label(const char *label, OpcodeEnum c = Statement::Label)
		: Statement(c), Value(label)
	{
		assert(c == Comment || c == Sub || c == Goto || c == Statement::Label || c == Call);
	}
};

class Comment : public Label {public: Comment(const char *label) : Label(label, Statement::Comment){}};
class Sub : public Label {public: Sub(const char *label) : Label(label, Statement::Sub){}};
class Goto : public Label {public: Goto(const char *label) : Label(label, Statement::Goto){}};
class Call : public Label {public: Call(const char *label) : Label(label, Statement::Call){}};

class Loop : public Statement
{
public:
	unsigned Iterations;

	Loop(unsigned iters) : Statement(Statement::Loop), Iterations(iters) {}
};

class Pause : public Statement
{
public:
	float Delay;

	Pause(float delay) : Statement(Statement::Pause), Delay(delay) {}
};

class Spline : public Statement
{
public:
	unsigned PointRefs[4];

	Spline(unsigned ptRefs[4]) : Statement(Statement::Spline)
	{
		PointRefs[0] = ptRefs[0];
		PointRefs[1] = ptRefs[1];
		PointRefs[2] = ptRefs[2];
		PointRefs[3] = ptRefs[3];
	}
};

class Point
{
public:
	union {
		struct {
			float X;
			float Y;
		};
		float Coords[2];
	};

	Point(float x, float y) : X(x), Y(y) {}
	Point(float coords[2]) : X(coords[0]), Y(coords[1]) {}
};

typedef std::list<Point> Points;

class Program
{
public:
	Statements Statements;
	Points Points;

	void LoadKam(const char* fileName);
	void LoadTxt(const char* fileName);
	void SaveKam(const char* fileName);
	//void SaveKam(IStream& stream);
	void SaveTxt(const char* fileName);
	//void SaveTxt(IStream& stream);
};

class InvalidOpcode {};
class InvalidCommandLength {};
class InvalidCommandFormat {};
class InvalidSpeedValue {};

} // namespace Kamea
} // namespace Dxf2Kam
} // namespace Denisenko

#endif KameaH