#include <stdexcept>
#include <iostream>
#include <string>
#include <cassert>
#include <cstring>
#include "kamea.h"

using namespace Kamea;
using namespace std;
using namespace MyGeometryTools;

Program load(istream &stream)
{
	Program program;
	stream.exceptions(ios::eofbit | ios::badbit | ios::failbit);
	unsigned short cmds_num;
	stream.read(reinterpret_cast<char*>(&cmds_num), 2);
	for (unsigned i = 0; i < cmds_num; i++)
	{
		char cmd_id = stream.get();
		if (cmd_id > 0x1f)
			throw EInvalidCommandId("", i, cmd_id);
		// безопасно т.к. на предыдущем шаге значение было проверено
		ECmdId cmdId;
		cmdId = static_cast<ECmdId>(cmd_id);

		unsigned param_l = stream.get();
		if (param_l > 30)
			throw EInvalidCommandLength("", i, cmdId, param_l);

		char cmdbuf[31];
		stream.read(cmdbuf, 30);
		cmdbuf[param_l] = 0;

		unsigned startPoint, midPoint, endPoint, spd = SPDDEF, device, mirrorX, mirrorY, n,
			old_scale, new_scale, p1, p2, p3, p4;
		int res;
		float dx, dy, dz, radius, al, fi, angle, delay;
		bool updown, relative;

		switch (cmdId) {
		case PP_LINE:
			updown = (cmdbuf[param_l-1] == 0);
			for (char *ptr = cmdbuf; ptr != cmdbuf+param_l; ptr++)
				if (*ptr < ' ') *ptr = ' ';
			res = sscanf(cmdbuf, "%u,%u,%f,%u", &startPoint, &endPoint, &dz, &spd);
			if (res < 3)
				throw EInvalidCommandFormat("", i, cmdId);
			if (spd > SPD8 || spd < SPDDEF)
				throw EInvalidSpeedValue("", i, cmdId, spd);
			program.addCommand(auto_ptr<Command>(new CPP_LINE(startPoint, endPoint, dz, updown, static_cast<ESpeed>(spd))));
			break;

		case PP_ARC:
			res = sscanf(cmdbuf, "%u,%u,%u,%u", &startPoint, &midPoint, &endPoint, &spd);	
			if (res < 3)
				throw EInvalidCommandFormat("", i, cmdId);
			if (spd > SPD8 || spd < SPDDEF)
				throw EInvalidSpeedValue("", i, cmdId, spd);
			program.addCommand(auto_ptr<Command>(new CPP_ARC(startPoint, midPoint, endPoint, static_cast<ESpeed>(spd))));
			break;

		case PR_ARC:
			res = sscanf(cmdbuf, "%u,%u,%f,%u", &startPoint, &endPoint, &radius, &spd);	
			if (res < 3)
				throw EInvalidCommandFormat("", i, cmdId);
			if (spd > SPD8 || spd < SPDDEF)
				throw EInvalidSpeedValue("", i, cmdId, spd);
			program.addCommand(auto_ptr<Command>(new CPR_ARC(startPoint, endPoint, radius, static_cast<ESpeed>(spd))));
			break;

		case PZ_ARC:
			res = sscanf(cmdbuf, "%u,%u,%u,%f,%u", &startPoint, &midPoint, &endPoint, &dz, &spd);	
			if (res < 4)
				throw EInvalidCommandFormat("", i, cmdId);
			if (spd > SPD8 || spd < SPDDEF)
				throw EInvalidSpeedValue("", i, cmdId, spd);
			program.addCommand(auto_ptr<Command>(new CPZ_ARC(startPoint, midPoint, endPoint, dz, static_cast<ESpeed>(spd))));
			break;

		case PRZ_ARC:
			res = sscanf(cmdbuf, "%u,%u,%u,%f,%u", &startPoint, &endPoint, &radius, &dz, &spd);	
			if (res < 4)
				throw EInvalidCommandFormat("", i, cmdId);
			if (spd > SPD8 || spd < SPDDEF)
				throw EInvalidSpeedValue("", i, cmdId, spd);
			program.addCommand(auto_ptr<Command>(new CPRZ_ARC(startPoint, endPoint, radius, dz, static_cast<ESpeed>(spd))));
			break;

		case LINE:
			res = sscanf(cmdbuf, "%f,%f,%f,%u", &dx, &dy, &dz, &spd);	
			if (res < 3)
				throw EInvalidCommandFormat("", i, cmdId);
			if (spd > SPD8 || spd < SPDDEF)
				throw EInvalidSpeedValue("", i, cmdId, spd);
			program.addCommand(auto_ptr<Command>(new CLINE(dx, dy, dz, static_cast<ESpeed>(spd))));
			break;

		case ARC:
			res = sscanf(cmdbuf, "%f,%f,%f,%u", &radius, &al, &fi, &spd);	
			if (res < 3)
				throw EInvalidCommandFormat("", i, cmdId);
			if (spd > SPD8 || spd < SPDDEF)
				throw EInvalidSpeedValue("", i, cmdId, spd);
			program.addCommand(auto_ptr<Command>(new CARC(radius, al, fi, static_cast<ESpeed>(spd))));
			break;

		case REL_ARC:
			res = sscanf(cmdbuf, "%f,%f,%f,%u", &dx, &dy, &radius, &spd);	
			if (res < 3)
				throw EInvalidCommandFormat("", i, cmdId);
			if (spd > SPD8 || spd < SPDDEF)
				throw EInvalidSpeedValue("", i, cmdId, spd);
			program.addCommand(auto_ptr<Command>(new CREL_ARC(dx, dy, radius, static_cast<ESpeed>(spd))));
			break;

		case SET_PARK: program.addCommand(auto_ptr<Command>(new CSET_PARK)); break;
		case GO_PARK: program.addCommand(auto_ptr<Command>(new CGO_PARK)); break;
		case SET_ZERO: program.addCommand(auto_ptr<Command>(new CSET_ZERO)); break;
		case GO_ZERO: program.addCommand(auto_ptr<Command>(new CGO_ZERO)); break;

		case ON:
			res = sscanf(cmdbuf, "%u", &device);
			if (res < 1)
				throw EInvalidCommandFormat("", i, cmdId);
			if (device != SPINDEL)
				throw EInvalidDevice("", i, cmdId, device);
			program.addCommand(auto_ptr<Command>(new CON(static_cast<EDevice>(device))));
			break;

		case OFF:
			res = sscanf(cmdbuf, "%u", &device);
			if (res < 1)
				throw EInvalidCommandFormat("", i, cmdId);
			if (device != SPINDEL)
				throw EInvalidDevice("", i, cmdId, device);
			program.addCommand(auto_ptr<Command>(new COFF(static_cast<EDevice>(device))));
			break;

		case SPEED:
			res = sscanf(cmdbuf, "%u", &spd);
			if (res < 1)
				throw EInvalidCommandFormat("", i, cmdId);
			if (spd < SPD1 || spd > SPD8)
				throw EInvalidSpeedValue("", i, cmdId, spd);
			program.addCommand(auto_ptr<Command>(new CSPEED(static_cast<ESpeed>(spd))));
			break;

		case SCALE_X:
			relative = (cmdbuf[param_l-1] == 1);
			for (char *ptr = cmdbuf; ptr != cmdbuf+param_l; ptr++)
				if (*ptr < ' ') *ptr = ' ';
			res = sscanf(cmdbuf, "%u,%u", &old_scale, &new_scale);
			if (res < 2)
				throw EInvalidCommandFormat("", i, cmdId);
			program.addCommand(auto_ptr<Command>(new CSCALEX(old_scale, new_scale, relative)));
			break;

		case SCALE_Y:
			relative = (cmdbuf[param_l-1] == 1);
			for (char *ptr = cmdbuf; ptr != cmdbuf+param_l; ptr++)
				if (*ptr < ' ') *ptr = ' ';
			res = sscanf(cmdbuf, "%u,%u", &old_scale, &new_scale);
			if (res < 2)
				throw EInvalidCommandFormat("", i, cmdId);
			program.addCommand(auto_ptr<Command>(new CSCALEY(old_scale, new_scale, relative)));
			break;

		case SCALE_Z:
			relative = (cmdbuf[param_l-1] == 1);
			for (char *ptr = cmdbuf; ptr != cmdbuf+param_l; ptr++)
				if (*ptr < ' ') *ptr = ' ';
			res = sscanf(cmdbuf, "%u,%u", &old_scale, &new_scale);
			if (res < 2)
				throw EInvalidCommandFormat("", i, cmdId);
			program.addCommand(auto_ptr<Command>(new CSCALEZ(old_scale, new_scale, relative)));
			break;

		case TURN:
			res = sscanf(cmdbuf, "%u,%u,%f", &mirrorX, &mirrorY, &angle);
			if (res < 3)
				throw EInvalidCommandFormat("", i, cmdId);
			program.addCommand(auto_ptr<Command>(new CTURN(mirrorX > 0 ? true : false, mirrorY > 0 ? true : false, angle)));
			break;

		case SUB: program.addCommand(auto_ptr<Command>(new CSUB(cmdbuf))); break;
		case CALL: program.addCommand(auto_ptr<Command>(new CCALL(cmdbuf))); break;
		case RET: program.addCommand(auto_ptr<Command>(new CRET)); break;
		case LABEL: program.addCommand(auto_ptr<Command>(new CLABEL(cmdbuf))); break;
		case GOTO: program.addCommand(auto_ptr<Command>(new CGOTO(cmdbuf))); break;

		case LOOP:
			res = sscanf(cmdbuf, "%u", &n);
			if (res < 1)
				throw EInvalidCommandFormat("", i, cmdId);
			program.addCommand(auto_ptr<Command>(new CLOOP(n)));
			break;

		case ENDLOOP: program.addCommand(auto_ptr<Command>(new CENDLOOP)); break;
		case STOP: program.addCommand(auto_ptr<Command>(new CSTOP)); break;
		case FINISH: program.addCommand(auto_ptr<Command>(new CFINISH)); break;

		case PAUSE:
			res = sscanf(cmdbuf, "%f", &delay);
			if (res < 1)
				throw EInvalidCommandFormat("", i, cmdId);
			program.addCommand(auto_ptr<Command>(new CPAUSE(delay)));
			break;

		case COMMENT: program.addCommand(auto_ptr<Command>(new CCOMMENT(cmdbuf))); break;

		case SPLINE:
			res = sscanf(cmdbuf, "%i%i%i%i", &p1, &p2, &p3, &p4);
			if (res < 4)
				throw EInvalidCommandFormat("", i, cmdId);
			program.addCommand(auto_ptr<Command>(new CSPLINE(p1, p2, p3, p4)));
			break;
		}
	}

	unsigned short points_num;
	stream.read(reinterpret_cast<char*>(&points_num), 2);
	program.points.resize(points_num);
	while (points_num--) {
		short xy[2];
		stream.read(reinterpret_cast<char*>(xy), 4);
		program.points.push_back(Point(xy[0]/10.0f, xy[1]/10.0f));
	}

	return program;
}

class SaverDispatcher : public Dispatcher {
	virtual void command(class CPP_LINE&);
	virtual void command(class CPP_ARC&);
	virtual void command(class CPR_ARC&);
	virtual void command(class CPZ_ARC&);
	virtual void command(class CPRZ_ARC&);
	virtual void command(class CLINE&);
	virtual void command(class CARC&);
	virtual void command(class CREL_ARC&);
	virtual void command(class CSET_PARK&);
	virtual void command(class CGO_PARK&);
	virtual void command(class CSET_ZERO&);
	virtual void command(class CGO_ZERO&);
	virtual void command(class CON&);
	virtual void command(class COFF&);
	virtual void command(class CSPEED&);
	virtual void command(class CSCALEX&);
	virtual void command(class CSCALEY&);
	virtual void command(class CSCALEZ&);
	virtual void command(class CTURN&);
	virtual void command(class CSUB&);
	virtual void command(class CCALL&);
	virtual void command(class CRET&);
	virtual void command(class CLABEL&);
	virtual void command(class CGOTO&);
	virtual void command(class CLOOP&);
	virtual void command(class CENDLOOP&);
	virtual void command(class CSTOP&);
	virtual void command(class CFINISH&);
	virtual void command(class CCOMMENT&);
	virtual void command(class CPAUSE&);
	virtual void command(class CSPLINE&);

	void _before(ECmdId);
	void _after(void);
	void _simple(ECmdId);
	void _write_spd(ESpeed);
	void _text(ECmdId, string);
	void _scale(ECmdId, unsigned, unsigned, bool);
	void _check_constraints(void);

	ostream &stream;
	size_t _param_l;
	char _cmdbuf[256];
public:
	SaverDispatcher(ostream &stream) : stream(stream)
	{
		//memset(_cmdbuf, 0, sizeof(_cmdbuf));
	}
};

void SaverDispatcher::_check_constraints() {
	if (stream.bad())
		throw std::runtime_error("Файл не открыт или ошибка ввода-вывода.");
}

/*void SaverDispatcher::beginCommands(size_t numcommands)
{
	_check_constraints();
	stream.write(reinterpret_cast<char*>(&numcommands), 2);
}

void SaverDispatcher::beginPoints(size_t numpoints)
{
	_check_constraints();
	pstream->write(reinterpret_cast<char*>(&numpoints), 2);
}*/

inline void SaverDispatcher::_before(ECmdId cmdId) {
	_check_constraints();
	_param_l = 0;
	char cmd_id = cmdId;
	stream.write(&cmd_id, 1);
}

inline void SaverDispatcher::_write_spd(ESpeed spd)
{
	if (spd != SPDDEF)
		_param_l += sprintf(_cmdbuf+_param_l, ",%u", spd);
}

inline void SaverDispatcher::_after() {
	if (_param_l > 30)
		throw std::runtime_error("Слишком длинная строка параметров в команде ТТ ДУГА");
	char cparam_l = static_cast<char>(_param_l);
	stream.write(&cparam_l, 1);
	stream.write(_cmdbuf, 30);
}

void SaverDispatcher::command(CPP_LINE &cmd) {
	_before(PP_LINE);
	_param_l = sprintf(_cmdbuf, "%u,%u,%.1f", cmd.startPoint, cmd.endPoint, cmd.deltaZ);
	_write_spd(cmd.speed);
	_cmdbuf[_param_l++] = static_cast<unsigned char>(0xfd);	// программа UF3-5 непонятно почему требует этого
	_cmdbuf[_param_l++] = (cmd.UpAndDown ? 0 : 1);
	_after();
}

void SaverDispatcher::command(CPP_ARC &cmd) {
	_before(PP_ARC);
	_param_l = sprintf(_cmdbuf, "%u,%u,%u", cmd.startPoint, cmd.midPoint, cmd.endPoint);
	_write_spd(cmd.speed);
	_after();
}

void SaverDispatcher::command(CPR_ARC &cmd) {
	_before(PR_ARC);
	_param_l = sprintf(_cmdbuf, "%u,%u,%.1f", cmd.startPoint, cmd.endPoint, cmd.radius);
	_write_spd(cmd.speed);
	_after();
}

void SaverDispatcher::command(CPZ_ARC &cmd) {
	_before(PZ_ARC);
	_param_l = sprintf(_cmdbuf, "%u,%u,%u,%.1f", cmd.startPoint, cmd.midPoint, cmd.endPoint, cmd.deltaZ);
	_write_spd(cmd.speed);
	_after();
}

void SaverDispatcher::command(CPRZ_ARC &cmd) {
	_before(PRZ_ARC);
	_param_l = sprintf(_cmdbuf, "%u,%u,%.1f,%.1f", cmd.startPoint, cmd.endPoint, cmd.radius, cmd.deltaZ);
	_write_spd(cmd.speed);
	_after();
}

void SaverDispatcher::command(CLINE &cmd) {
	_before(LINE);
	_param_l = sprintf(_cmdbuf, "%.1f,%.1f,%.1f", cmd.dx, cmd.dy, cmd.dz);
	_write_spd(cmd.speed);
	_after();
}

void SaverDispatcher::command(CARC &cmd) {
	_before(ARC);
	_param_l = sprintf(_cmdbuf, "%.1f,%.1f,%.1f", cmd.radius, cmd.al, cmd.fi);
	_write_spd(cmd.speed);
	_after();
}

void SaverDispatcher::command(CREL_ARC &cmd) {
	_before(REL_ARC);
	_param_l = sprintf(_cmdbuf, "%.1f,%.1f,%.1f", cmd.dx, cmd.dy, cmd.radius);
	_write_spd(cmd.speed);
	_after();
}

void SaverDispatcher::command(CSET_PARK &cmd) {
	_simple(SET_PARK);
}

void SaverDispatcher::command(CGO_PARK &cmd) {
	_simple(GO_PARK);
}

void SaverDispatcher::command(CSET_ZERO &cmd) {
	_simple(SET_ZERO);
}

void SaverDispatcher::command(CGO_ZERO &cmd) {
	_simple(GO_ZERO);
}

void SaverDispatcher::command(CON &cmd) {
	_before(ON);
	_param_l = sprintf(_cmdbuf, "%u", cmd.device);
	_after();
}

void SaverDispatcher::command(COFF &cmd) {
	_before(OFF);
	_param_l = sprintf(_cmdbuf, "%u", cmd.device);
	_after();
}

void SaverDispatcher::command(CSPEED &cmd) {
	assert(cmd.speed >= 1 && cmd.speed <= 8);
	_before(SPEED);
	_param_l = sprintf(_cmdbuf, "%u", cmd.speed);
	_after();
}

inline void SaverDispatcher::_scale(ECmdId cmd_id, unsigned oldscale, unsigned newscale, bool relative)
{
	_before(cmd_id);
	_param_l = sprintf(_cmdbuf, "%u,%u", oldscale, newscale);
	_cmdbuf[_param_l++]=static_cast<unsigned char>(0xfd);
	_cmdbuf[_param_l++]=relative ? 1 : 0;
	_after();
}

void SaverDispatcher::command(CSCALEX &cmd) {
	_scale(SCALE_X, cmd.old_scale, cmd.new_scale, cmd.relative);
}

void SaverDispatcher::command(CSCALEY &cmd) {
	_scale(SCALE_Y, cmd.old_scale, cmd.new_scale, cmd.relative);
}

void SaverDispatcher::command(CSCALEZ &cmd) {
	_scale(SCALE_Z, cmd.old_scale, cmd.new_scale, cmd.relative);
}

void SaverDispatcher::command(CTURN &cmd) {
	_before(TURN);
	_param_l = sprintf(_cmdbuf, "%u,%u,%.1f", cmd.mirrorX, cmd.mirrorY, cmd.angle);
	_after();
}

inline void SaverDispatcher::_text(ECmdId cmd_id, string str)
{
	_before(cmd_id);
	strcpy(_cmdbuf, str.c_str());
	_param_l = strlen(_cmdbuf);
	_after();
}

void SaverDispatcher::command(CSUB &cmd) {
	_text(SUB, cmd.name);
}

void SaverDispatcher::command(CCALL &cmd) {
	_text(CALL, cmd.sub_name);
}

inline void SaverDispatcher::_simple(ECmdId cmd_id)
{
	_before(cmd_id);
	_after();
}

void SaverDispatcher::command(CRET &cmd) {
	_simple(RET);
}

void SaverDispatcher::command(CLABEL &cmd) {
	_text(LABEL, cmd.name);
}

void SaverDispatcher::command(CGOTO &cmd) {
	_text(GOTO, cmd.label_name);
}

void SaverDispatcher::command(CLOOP &cmd) {
	_before(LOOP);
	_param_l = sprintf(_cmdbuf, "%u", cmd.n);
	_after();
}

void SaverDispatcher::command(CENDLOOP &cmd) {
	_simple(ENDLOOP);
}

void SaverDispatcher::command(CSTOP &cmd) {
	_simple(STOP);
}

void SaverDispatcher::command(CFINISH &cmd) {
	_simple(FINISH);
}

void SaverDispatcher::command(CPAUSE &cmd) {
	_before(PAUSE);
	_param_l = sprintf(_cmdbuf, "%.1f", cmd.delay);
	_after();
}

void SaverDispatcher::command(CCOMMENT &cmd) {
	_text(COMMENT, cmd.comment);
}

void SaverDispatcher::command(CSPLINE &cmd)
{
	_before(PAUSE);
	_param_l = sprintf(_cmdbuf, "%i,%i,%i,%i", cmd.p1, cmd.p2, cmd.p3, cmd.p4);
	_after();
}

class func1 {
	Dispatcher &dispatcher;
public:
	func1(Dispatcher &dispatcher) : dispatcher(dispatcher) {}
	void operator () (Command *pcommand) {assert(pcommand); pcommand->dispatch(dispatcher);}
};

class func2 {
	ostream &stream;
public:
	func2(ostream &stream) : stream(stream) {}
	void operator () (Point &point) {
		short int x=static_cast<int>(point.x*10), y=static_cast<int>(point.y*10);
		stream.write(reinterpret_cast<char*>(&x), 2);
		stream.write(reinterpret_cast<char*>(&y), 2);
	};
};

void Kamea::save(ostream &stream, Program &program)
{
	// saving comands
	size_t commands = program.getCommands().size();
	stream.write(reinterpret_cast<char*>(&commands), 2);
	SaverDispatcher dispatcher(stream);
	func1 func1(dispatcher);
	for_each(program.getCommands().begin(), program.getCommands().end(), func1);
	// saving points
	size_t points = program.points.size();
	stream.write(reinterpret_cast<char*>(&points), 2);
	func2 func2(stream);
	for_each(program.points.begin(), program.points.end(), func2);
}

ProgramWriter::ProgramWriter(void)
	: speed(SPD8), pos(0, 0, 0), scale(1, 1, 1), rotate_angle(0),
	move_speed(SPD8), cut_speed(SPD5), move_z(10)
{
	mirror_xy[0] = false;
	mirror_xy[1] = false;
	program.addCommand(std::auto_ptr<Command>(new CSPEED(SPD8)));
	program.addCommand(std::auto_ptr<Command>(new CGO_ZERO));
	program.addCommand(std::auto_ptr<Command>(new CSCALEX(1, 1)));
	program.addCommand(std::auto_ptr<Command>(new CSCALEY(1, 1)));
	program.addCommand(std::auto_ptr<Command>(new CSCALEZ(1, 1)));
	program.addCommand(std::auto_ptr<Command>(new CTURN(0, 0, 0)));
}

vec3f ProgramWriter::tform(vec3f vec)
{
	matrix2f mtform = matrix2f::rotate(rotate_angle)*matrix2f::scale((mirror_xy[0] ? -1 : 1)*scale.x, (mirror_xy[1] ? -1 : 1)*scale.y);
	return vec3f(mtform*vec2f(vec), vec.z*scale.z);
}

void ProgramWriter::setSpeed(ESpeed spd)
{
	if (spd == SPDDEF || spd == this->speed)
		return;

	program.addCommand(std::auto_ptr<Command>(new CSPEED(spd)));
	speed = spd;
}

ESpeed ProgramWriter::getSpeed(void)
{
	return speed;
}

void ProgramWriter::switchDevice(EDevice dev, bool on)
{
	if (spindle == on)
		return;

	if (on)
		program.addCommand(std::auto_ptr<Command>(new CON(dev)));
	else
		program.addCommand(std::auto_ptr<Command>(new COFF(dev)));
	spindle = on;
}

bool ProgramWriter::getDevice(EDevice dev)
{
	return spindle;
}

void ProgramWriter::setMirror(const bool xy[2])
{
	if (mirror_xy[0] == mirror_xy[0] || mirror_xy[1] == mirror_xy[1])
		return;
	program.addCommand(auto_ptr<Command>(new CTURN(xy[0], xy[1], rotate_angle)));
	mirror_xy[0] = xy[0];
	mirror_xy[1] = xy[1];
}

void ProgramWriter::getMirror(bool xy[2])
{
	xy = mirror_xy;
}

void ProgramWriter::setRotate(float angle)
{
	if (rotate_angle == angle)
		return;
	program.addCommand(auto_ptr<Command>(new CTURN(mirror_xy[0], mirror_xy[1], angle)));
	rotate_angle = angle;
}

float ProgramWriter::getRotate(void)
{
	return rotate_angle;
}

void ProgramWriter::setScale(const float xyz[3])
{
	if (scale[0] != xyz[0])
		program.addCommand(auto_ptr<Command>(new CSCALEX(100, unsigned(100*xyz[0]))));
	if (scale[1] != xyz[1])
		program.addCommand(auto_ptr<Command>(new CSCALEY(100, unsigned(100*xyz[1]))));
	if (scale[2] != xyz[2])
		program.addCommand(auto_ptr<Command>(new CSCALEZ(100, unsigned(100*xyz[2]))));
	scale = xyz;
}

void ProgramWriter::getScale(float xy[2])
{
	xy = scale;
}

void ProgramWriter::displace(const float disp[3], ESpeed spd)
{
	if (disp[0] == 0 && disp[1] == 0 && disp[2] == 0)
		return;
	program.addCommand(std::auto_ptr<Command>(new CLINE(disp[0], disp[1], disp[2], spd != speed ? spd : SPDDEF)));
	pos = pos + tform(disp);
}

void ProgramWriter::arc(float rad, float al, float fi, ESpeed spd)
{
	if (fi == 0)
		return;
	program.addCommand(std::auto_ptr<Command>(new CARC(rad, al, fi, spd != speed ? spd : SPDDEF)));
	arcf arc = arcf::by_pt_rad_ang(pos, rad, al, fi);
	vec2f newpos = arc.calcEndPoint();
	newpos = vec2f(tform(vec3f(newpos - vec2f(pos), 0)));
	pos.x = newpos.x;
	pos.y = newpos.y;
}

void ProgramWriter::moveto(const float xy[2])
{
	if (vec2f(pos) != vec2f(xy))
	{
		if (pos.z < move_z)
			displace(vec3f(0, 0, move_z - pos.z), move_speed);
		displace(vec3f(xy, move_z) - pos, move_speed);
	}
	if (pos.z > 0)
		displace(vec3f(0, 0, -pos.z), move_speed);
}

/*void ProgramWriter::line(const float xyz1[3], const float xyz2[3])
{
	//if ((vec2d<float>(pos_xyz)-vec2d<float>(xyz1)).len() > .1f)
	moveto(xyz1);
	setSpeed(cut_speed);
	displace(vec3f(0, 0, xyz1[2]));
	displace(vec3f(xyz2) - vec3f(xyz1));
}*/

vec3f ProgramWriter::getPos(void)
{
	return pos;
}

void ProgramWriter::begin()
{
}

Program ProgramWriter::end()
{
	return std::move(program);
}
