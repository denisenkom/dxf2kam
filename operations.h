#ifndef __OPERATIONS_H_INCLUDED__
#define __OPERATIONS_H_INCLUDED__

#include "opcodes.h"

#include <vector>
#include <memory>
#include <algorithm>

namespace Kamea {

class command {
public:
	virtual void dispatch(class dispatcher&)=0;
};

typedef std::vector<command*> t_commands_vector;
typedef std::vector<class point> t_points_vector;

class program {
	typedef t_commands_vector t_commands;
	t_commands commands;
	struct del_cmd {void operator () (command *pcmd) {delete pcmd;}} del_cmd;
	struct zero {void operator() (command *&pcmd) {pcmd = 0;}} zero;
public:
	typedef t_points_vector t_points;
	t_points points;

	program(void) {}

	program(program &other)
		: commands(other.commands), points(other.points)
	{
		std::for_each(other.commands.begin(), other.commands.end(), zero);
	}

	program & operator = (program &other)
	{
		if (this == &other)
			return *this;
		commands = other.commands;
		points = other.points;
		std::for_each(other.commands.begin(), other.commands.end(), zero);
		return *this;
	}

	~program() {std::for_each(commands.begin(), commands.end(), del_cmd);}
	void addCommand(std::auto_ptr<command> pcommand) {commands.push_back(pcommand.release());}
	const t_commands & getCommands(void) const {return commands;}
};

class dispatcher {
public:
	virtual void command(class CPP_LINE&)=0;
	virtual void command(class CPP_ARC&)=0;
	virtual void command(class CPR_ARC&)=0;
	virtual void command(class CPZ_ARC&)=0;
	virtual void command(class CPRZ_ARC&)=0;
	virtual void command(class CLINE&)=0;
	virtual void command(class CARC&)=0;
	virtual void command(class CREL_ARC&)=0;
	virtual void command(class CSET_PARK&)=0;
	virtual void command(class CGO_PARK&)=0;
	virtual void command(class CSET_ZERO&)=0;
	virtual void command(class CGO_ZERO&)=0;
	virtual void command(class CON&)=0;
	virtual void command(class COFF&)=0;
	virtual void command(class CSPEED&)=0;
	virtual void command(class CSCALEX&)=0;
	virtual void command(class CSCALEY&)=0;
	virtual void command(class CSCALEZ&)=0;
	virtual void command(class CTURN&)=0;
	virtual void command(class CSUB&)=0;
	virtual void command(class CCALL&)=0;
	virtual void command(class CRET&)=0;
	virtual void command(class CLABEL&)=0;
	virtual void command(class CGOTO&)=0;
	virtual void command(class CLOOP&)=0;
	virtual void command(class CENDLOOP&)=0;
	virtual void command(class CSTOP&)=0;
	virtual void command(class CFINISH&)=0;
	virtual void command(class CCOMMENT&)=0;
	virtual void command(class CPAUSE&)=0;
	virtual void command(class CSPLINE&)=0;
};

class point {
public:
	float x, y;

	point(void) {}

	point(float x, float y)
		: x(x), y(y)
	{
	}
};

class CPP_LINE : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = PP_LINE;
	unsigned startPoint, endPoint;
	float deltaZ;
	bool UpAndDown;
	ESpeed speed;
	CPP_LINE(int startPoint, int endPoint, float deltaZ, bool UpAndDown=false, ESpeed speed=SPDDEF) : 
		startPoint(startPoint), endPoint(endPoint), deltaZ(deltaZ), UpAndDown(UpAndDown), speed(speed) {}
};

class CPP_ARC : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = PP_ARC;
	unsigned startPoint, midPoint, endPoint;
	ESpeed speed;
	CPP_ARC(int startPoint, int midPoint, int endPoint, ESpeed spd=SPDDEF) : startPoint(startPoint), 
		midPoint(midPoint), endPoint(endPoint), speed(spd) {};
};

class CPR_ARC : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = PR_ARC;
	unsigned startPoint, endPoint;
	float radius;
	ESpeed speed;
	CPR_ARC(int startPoint, int endPoint, float radius, ESpeed speed=SPDDEF) :
		startPoint(startPoint), endPoint(endPoint), radius(radius), speed(speed) {};
};

class CPZ_ARC : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = PZ_ARC;
	unsigned startPoint, midPoint, endPoint;
	float deltaZ;
	ESpeed speed;
	CPZ_ARC(int startPoint, int midPoint, int endPoint, float deltaZ, ESpeed speed=SPDDEF) : 
		startPoint(startPoint), midPoint(midPoint), endPoint(endPoint), deltaZ(deltaZ),
		speed(speed) {};
};

class CPRZ_ARC : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = PRZ_ARC;
	unsigned startPoint, endPoint;
	float radius, deltaZ;
	ESpeed speed;
	CPRZ_ARC(int startPoint, int endPoint, float radius, float deltaZ, ESpeed speed=SPDDEF) : 
		startPoint(startPoint), endPoint(endPoint), radius(radius), deltaZ(deltaZ),
		speed(speed) {};
};

class CLINE : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = LINE;
	float dx, dy, dz;
	ESpeed speed;
	CLINE(float dx, float dy, float dz, ESpeed speed=SPDDEF) : dx(dx), dy(dy), dz(dz),
		speed(speed) {};
};

class CARC : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = ARC;
	float radius, al, fi;
	ESpeed speed;
	CARC(float radius, float al, float fi, ESpeed speed=SPDDEF) : radius(radius), al(al),
		fi(fi), speed(speed) {};
};

class CREL_ARC : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = REL_ARC;
	float dx, dy, radius;
	ESpeed speed;
	CREL_ARC(float dx, float dy, float radius, ESpeed speed=SPDDEF) : dx(dx), dy(dy),
		radius(radius), speed(speed) {};
};

class CSET_PARK : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = SET_PARK;
	CSET_PARK() {};
};

class CGO_PARK : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = GO_PARK;
	CGO_PARK() {};
};

class CSET_ZERO : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = SET_ZERO;
	CSET_ZERO() {};
};

class CGO_ZERO : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = GO_ZERO;
	CGO_ZERO() {};
};

class CON : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = ON;
	EDevice device;
	CON(EDevice device) : device(device) {};
};

class COFF : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = OFF;
	EDevice device;
	COFF(EDevice device) : device(device) {};
};

class CSPEED : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = SPEED;
	ESpeed speed;
	CSPEED(ESpeed speed) : speed(speed) {};
};

class CSCALEX : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = SCALE_X;
	bool relative;
	unsigned old_scale, new_scale;
	CSCALEX(unsigned old_scale, unsigned new_scale, bool relative=false) : relative(relative), old_scale(old_scale),
		new_scale(new_scale) {};
};

class CSCALEY : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = SCALE_Y;
	bool relative;
	unsigned old_scale, new_scale;
	CSCALEY(unsigned old_scale, unsigned new_scale, bool relative=false) : relative(relative), old_scale(old_scale),
		new_scale(new_scale) {};
};

class CSCALEZ : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = SCALE_Z;
	bool relative;
	unsigned old_scale, new_scale;
	CSCALEZ(unsigned old_scale, unsigned new_scale, bool relative=false) : relative(relative), old_scale(old_scale),
		new_scale(new_scale) {};
};

class CTURN : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = TURN;
	bool mirrorX;
	bool mirrorY;
	float angle;
	CTURN(bool mirrorX, bool mirrorY, float angle) : mirrorX(mirrorX), mirrorY(mirrorY),
		angle(angle) {};
};

class CSUB : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = SUB;
	std::string name;
	CSUB(const std::string name) : name(name) {};
};

class CCALL : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = CALL;
	std::string sub_name;
	CCALL(const std::string sub_name) : sub_name(sub_name) {};
};

class CRET : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = RET;
};

class CLABEL : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = LABEL;
	std::string name;
	CLABEL(const std::string name) : name(name) {};
};

class CGOTO : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = GOTO;
	std::string label_name;
	CGOTO(const std::string label_name) : label_name(label_name) {};
};

class CLOOP : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = LOOP;
	unsigned n;
	CLOOP(unsigned n) : n(n) {};
};

class CENDLOOP : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = ENDLOOP;
};

class CSTOP : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = STOP;
};

class CFINISH : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = FINISH;
};

class CCOMMENT : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = COMMENT;
	std::string comment;
	CCOMMENT(const std::string comment) : comment(comment) {};
};

class CPAUSE : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = PAUSE;
	float delay;
	CPAUSE(float delay) : delay(delay) {};
};

class CSPLINE : public command {
	void dispatch(dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	unsigned p1, p2, p3, p4;
	CSPLINE(unsigned p1, unsigned p2, unsigned p3, unsigned p4)	: p1(p1), p2(p2), p3(p3), p4(p4) {}
};

}	// namespace Kamea

#endif	// __OPERATIONS_H_INCLUDED__