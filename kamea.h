#ifndef KAMEA_H
#define KAMEA_H

#include <stdexcept>
#include <vector>
#include <memory>
#include <algorithm>
#include "geometry.h"

namespace Kamea {

// opcodes 1 byte
enum ECmdId {PP_LINE = 0x00, PP_ARC = 0x01, PR_ARC = 0x02, PZ_ARC = 0x03,
	PRZ_ARC = 0x1e, LINE = 0x04, ARC = 0x06, REL_ARC = 0x07, SET_PARK = 0x08, 
	GO_PARK = 0x09, SET_ZERO = 0x0a, GO_ZERO = 0x0b, ON = 0x0c,
	OFF = 0x0d, SPEED = 0x0e, SCALE_X = 0x0f, SCALE_Y = 0x10,
	SCALE_Z = 0x11, TURN = 0x12, LABEL = 0x13, CALL = 0x14,
	RET = 0x15, GOTO = 0x16, LOOP = 0x17, ENDLOOP = 0x18,
	STOP = 0x19, FINISH = 0x1a, COMMENT = 0x1b, PAUSE = 0x1c,
	SUB = 0x1f, SPLINE = 0x28};	// SPLINE - в новой версии программы

// speeds
enum ESpeed {SPDDEF=0, SPD1=1, SPD2=2, SPD3=3, SPD4=4, SPD5=5,
	SPD6=6, SPD7=7, SPD8=8};

// devices
enum EDevice {SPINDEL=1};

class EParseError : public std::runtime_error {
public:
	unsigned cmd_indx;
	EParseError(const char *desc, unsigned cmd_indx)
		: std::runtime_error(desc), cmd_indx(cmd_indx)
	{
	}
};

class EInvalidCommandId : public EParseError
{
public:
	unsigned cmd_id;
	EInvalidCommandId(const char *desc, unsigned cmd_indx, unsigned cmd_id)
		: EParseError(desc, cmd_indx), cmd_id(cmd_id)
	{
	}

	/*virtual const char * what(void)
	{
		std::stringstream str;
		str << "#" << (cmd_indx+1) << ": Неправильный код команды: " << std::setbase(16) << cmd_id;
		return str.str().c_str();
	}*/
};

class EInvalidCommandLength : public EParseError
{
public:
	ECmdId cmd_id;
	size_t cmd_length;
	EInvalidCommandLength(const char *desc, unsigned cmd_indx, ECmdId cmd_id, size_t cmd_length)
		: EParseError(desc, cmd_indx), cmd_id(cmd_id), cmd_length(cmd_length)
	{
	}

	/*virtual const char * what(void)
	{
		std::stringstream str;
		str << "#" << (cmd_indx+1) << ": " << text_names_rus[cmd_id] << ": Неверная длина команды: " << cmd_length;
		return str.str().c_str();
	}*/
};

class EInvalidCommandFormat : public EParseError
{
public:
	ECmdId cmd_id;

	EInvalidCommandFormat(const char *desc, unsigned cmd_indx, ECmdId cmd_id)
		: EParseError(desc, cmd_indx), cmd_id(cmd_id)
	{
	}

	/*virtual const char * what(void)
	{
		std::stringstream str;
		str << "#" << (cmd_indx+1) << ": " << text_names_rus[cmd_id] << ": Неправильный формат команды";
		return str.str().c_str();
	}*/
};

class EInvalidSpeedValue : public EParseError
{
public:
	ECmdId cmd_id;
	int spd;

	EInvalidSpeedValue(const char *desc, unsigned cmd_indx, ECmdId cmd_id, int spd)
		: EParseError(desc, cmd_indx), cmd_id(cmd_id), spd(spd)
	{
	}

	/*virtual const char * what(void)
	{
		std::stringstream str;
		str << "#" << (cmd_indx+1) << ": " << text_names_rus[cmd_id] << ": Неверное значение скорости: " << spd;
		return str.str().c_str();
	}*/
};

class EInvalidDevice : public EParseError
{
public:
	ECmdId cmd_id;
	unsigned device;

	EInvalidDevice(const char *desc, unsigned cmd_indx, ECmdId cmd_id, unsigned device)
		: EParseError(desc, cmd_indx), cmd_id(cmd_id), device(device)
	{
	}

	/*virtual const char * what(void)
	{
		std::stringstream str;
		str << "#" << (cmd_indx+1) << ": " << text_names_rus[cmd_id] << ": Неверное устройство: " << device;
		return str.str().c_str();
	}*/
};

class Command {
public:
	virtual void dispatch(class Dispatcher&)=0;
};

class Point {
public:
	float x, y;

	Point(void) {}

	Point(float x, float y)
		: x(x), y(y)
	{
	}
};

typedef std::vector<Command*> t_commands_vector;
typedef std::vector<Point> t_points_vector;

class Program {
	typedef t_commands_vector t_commands;
	t_commands commands;
	struct del_cmd {void operator () (Command *pcmd) {delete pcmd;}} del_cmd;
	struct Zero {void operator() (Command *&pcmd) {pcmd = 0;}} zero;
public:
	typedef t_points_vector t_points;
	t_points points;

	Program(void) {}

	Program(Program && other)
		: commands(other.commands), points(other.points)
	{
		std::for_each(other.commands.begin(), other.commands.end(), zero);
	}

	Program & operator = (Program && other)
	{
		if (this == &other)
			return *this;
		commands = other.commands;
		points = other.points;
		std::for_each(other.commands.begin(), other.commands.end(), zero);
		return *this;
	}

	~Program() {std::for_each(commands.begin(), commands.end(), del_cmd);}
	void addCommand(std::auto_ptr<Command> pcommand) {commands.push_back(pcommand.release());}
	const t_commands & getCommands(void) const {return commands;}
};

class Dispatcher {
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

class CPP_LINE : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = PP_LINE;
	unsigned startPoint, endPoint;
	float deltaZ;
	bool UpAndDown;
	ESpeed speed;
	CPP_LINE(int startPoint, int endPoint, float deltaZ, bool UpAndDown=false, ESpeed speed=SPDDEF) : 
		startPoint(startPoint), endPoint(endPoint), deltaZ(deltaZ), UpAndDown(UpAndDown), speed(speed) {}
};

class CPP_ARC : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = PP_ARC;
	unsigned startPoint, midPoint, endPoint;
	ESpeed speed;
	CPP_ARC(int startPoint, int midPoint, int endPoint, ESpeed spd=SPDDEF) : startPoint(startPoint), 
		midPoint(midPoint), endPoint(endPoint), speed(spd) {};
};

class CPR_ARC : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = PR_ARC;
	unsigned startPoint, endPoint;
	float radius;
	ESpeed speed;
	CPR_ARC(int startPoint, int endPoint, float radius, ESpeed speed=SPDDEF) :
		startPoint(startPoint), endPoint(endPoint), radius(radius), speed(speed) {};
};

class CPZ_ARC : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = PZ_ARC;
	unsigned startPoint, midPoint, endPoint;
	float deltaZ;
	ESpeed speed;
	CPZ_ARC(int startPoint, int midPoint, int endPoint, float deltaZ, ESpeed speed=SPDDEF) : 
		startPoint(startPoint), midPoint(midPoint), endPoint(endPoint), deltaZ(deltaZ),
		speed(speed) {};
};

class CPRZ_ARC : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = PRZ_ARC;
	unsigned startPoint, endPoint;
	float radius, deltaZ;
	ESpeed speed;
	CPRZ_ARC(int startPoint, int endPoint, float radius, float deltaZ, ESpeed speed=SPDDEF) : 
		startPoint(startPoint), endPoint(endPoint), radius(radius), deltaZ(deltaZ),
		speed(speed) {};
};

class CLINE : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = LINE;
	float dx, dy, dz;
	ESpeed speed;
	CLINE(float dx, float dy, float dz, ESpeed speed=SPDDEF) : dx(dx), dy(dy), dz(dz),
		speed(speed) {};
};

class CARC : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = ARC;
	float radius, al, fi;
	ESpeed speed;
	CARC(float radius, float al, float fi, ESpeed speed=SPDDEF) : radius(radius), al(al),
		fi(fi), speed(speed) {};
};

class CREL_ARC : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = REL_ARC;
	float dx, dy, radius;
	ESpeed speed;
	CREL_ARC(float dx, float dy, float radius, ESpeed speed=SPDDEF) : dx(dx), dy(dy),
		radius(radius), speed(speed) {};
};

class CSET_PARK : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = SET_PARK;
	CSET_PARK() {};
};

class CGO_PARK : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = GO_PARK;
	CGO_PARK() {};
};

class CSET_ZERO : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = SET_ZERO;
	CSET_ZERO() {};
};

class CGO_ZERO : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = GO_ZERO;
	CGO_ZERO() {};
};

class CON : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = ON;
	EDevice device;
	CON(EDevice device) : device(device) {};
};

class COFF : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = OFF;
	EDevice device;
	COFF(EDevice device) : device(device) {};
};

class CSPEED : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = SPEED;
	ESpeed speed;
	CSPEED(ESpeed speed) : speed(speed) {};
};

class CSCALEX : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = SCALE_X;
	bool relative;
	unsigned old_scale, new_scale;
	CSCALEX(unsigned old_scale, unsigned new_scale, bool relative=false) : relative(relative), old_scale(old_scale),
		new_scale(new_scale) {};
};

class CSCALEY : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = SCALE_Y;
	bool relative;
	unsigned old_scale, new_scale;
	CSCALEY(unsigned old_scale, unsigned new_scale, bool relative=false) : relative(relative), old_scale(old_scale),
		new_scale(new_scale) {};
};

class CSCALEZ : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = SCALE_Z;
	bool relative;
	unsigned old_scale, new_scale;
	CSCALEZ(unsigned old_scale, unsigned new_scale, bool relative=false) : relative(relative), old_scale(old_scale),
		new_scale(new_scale) {};
};

class CTURN : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = TURN;
	bool mirrorX;
	bool mirrorY;
	float angle;
	CTURN(bool mirrorX, bool mirrorY, float angle) : mirrorX(mirrorX), mirrorY(mirrorY),
		angle(angle) {};
};

class CSUB : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = SUB;
	std::string name;
	CSUB(const std::string name) : name(name) {};
};

class CCALL : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = CALL;
	std::string sub_name;
	CCALL(const std::string sub_name) : sub_name(sub_name) {};
};

class CRET : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = RET;
};

class CLABEL : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = LABEL;
	std::string name;
	CLABEL(const std::string name) : name(name) {};
};

class CGOTO : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = GOTO;
	std::string label_name;
	CGOTO(const std::string label_name) : label_name(label_name) {};
};

class CLOOP : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = LOOP;
	unsigned n;
	CLOOP(unsigned n) : n(n) {};
};

class CENDLOOP : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = ENDLOOP;
};

class CSTOP : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = STOP;
};

class CFINISH : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = FINISH;
};

class CCOMMENT : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = COMMENT;
	std::string comment;
	CCOMMENT(const std::string comment) : comment(comment) {};
};

class CPAUSE : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	const static ECmdId id = PAUSE;
	float delay;
	CPAUSE(float delay) : delay(delay) {};
};

class CSPLINE : public Command {
	void dispatch(Dispatcher& dispatcher) {dispatcher.command(*this);};
public:
	unsigned p1, p2, p3, p4;
	CSPLINE(unsigned p1, unsigned p2, unsigned p3, unsigned p4)	: p1(p1), p2(p2), p3(p3), p4(p4) {}
};

	class EInvalidSpeed : public std::runtime_error
	{
	public:
		ESpeed spd;
		EInvalidSpeed(const char *desc, ESpeed spd)
			: std::runtime_error(desc), spd(spd)
		{
		}
	};

	class Modeler {
	public:
		virtual void switchDevice(EDevice, bool on)=0;
		virtual bool getDevice(EDevice)=0;
		virtual void setSpeed(ESpeed) = 0;
		virtual ESpeed getSpeed(void) = 0;

		virtual void setMirror(const bool xy[2])=0;
		virtual void getMirror(bool xy[2])=0;
		virtual void setRotate(float angle)=0;
		virtual float getRotate(void)=0;
		virtual void setScale(const float xyz[3])=0;
		virtual void getScale(float xyz[3])=0;

		virtual void moveto(const float xy[2])=0;	// Moves spindle to position xy0
		virtual void displace(const float disp[3], ESpeed spd=SPDDEF)=0;	// Displaces spindle
		virtual void arc(float rad, float al, float fi, ESpeed spd=SPDDEF)=0;
		//virtual void line(const float xyz1[3], const float xyz2[3])=0;

		virtual MyGeometryTools::vec3f getPos(void)=0;
	};

	class ProgramWriter : Modeler {
		MyGeometryTools::vec3f pos;
		ESpeed speed;
		bool spindle;
		float rotate_angle;
		MyGeometryTools::vec3f scale;
		bool mirror_xy[2];
		Program program;
		MyGeometryTools::vec3f tform(MyGeometryTools::vec3f vec);
	public:
		ESpeed move_speed, cut_speed;
		float move_z;

		virtual void switchDevice(EDevice, bool on);
		virtual bool getDevice(EDevice);
		virtual void setSpeed(ESpeed);
		virtual ESpeed getSpeed();

		virtual void setMirror(const bool xy[2]);
		virtual void getMirror(bool xy[2]);
		virtual void setRotate(float angle);
		virtual float getRotate(void);
		virtual void setScale(const float xyz[3]);
		virtual void getScale(float xy[2]);

		virtual void moveto(const float xy[2]);
		virtual void displace(const float disp[3], ESpeed spd=SPDDEF);
		virtual void arc(float rad, float al, float fi, ESpeed spd=SPDDEF);
		//virtual void line(const float xyz1[3], const float xyz2[3]);

		virtual MyGeometryTools::vec3f getPos(void);

		ProgramWriter();
		void begin(void);
		Program end(void);
	};

	class Program load(std::istream&);
	void save(std::ostream&, class Program&);
}	// namespace Kamea

#endif
