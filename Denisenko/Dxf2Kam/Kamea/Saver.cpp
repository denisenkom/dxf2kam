#include "StdAfx.h"

using std::string;
using std::ostream;
using std::for_each;

namespace Denisenko
{
namespace Kamea
{
class saver_dispatcher : public dispatcher {
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
	saver_dispatcher(ostream &stream) : stream(stream)
	{
		//memset(_cmdbuf, 0, sizeof(_cmdbuf));
	}
};

void saver_dispatcher::_check_constraints() {
	if (!stream)
		throw std::runtime_error("Файл не открыт или ошибка ввода-вывода.");
}

/*void saver_dispatcher::beginCommands(size_t numcommands)
{
	_check_constraints();
	stream.write(reinterpret_cast<char*>(&numcommands), 2);
}

void saver_dispatcher::beginPoints(size_t numpoints)
{
	_check_constraints();
	pstream->write(reinterpret_cast<char*>(&numpoints), 2);
}*/

inline void saver_dispatcher::_before(ECmdId cmdId) {
	_check_constraints();
	_param_l = 0;
	char cmd_id = cmdId;
	stream.write(&cmd_id, 1);
}

inline void saver_dispatcher::_write_spd(ESpeed spd)
{
	if (spd != SPDDEF)
		_param_l += sprintf(_cmdbuf+_param_l, ",%u", spd);
}

inline void saver_dispatcher::_after() {
	if (_param_l > 30)
		throw std::runtime_error("Слишком длинная строка параметров в команде ТТ ДУГА");
	char cparam_l = static_cast<char>(_param_l);
	stream.write(&cparam_l, 1);
	stream.write(_cmdbuf, 30);
}

void saver_dispatcher::command(CPP_LINE &cmd) {
	_before(PP_LINE);
	_param_l = sprintf(_cmdbuf, "%u,%u,%.1f", cmd.startPoint, cmd.endPoint, cmd.deltaZ);
	_write_spd(cmd.speed);
	_cmdbuf[_param_l++] = static_cast<unsigned char>(0xfd);	// программа UF3-5 непонятно почему требует этого
	_cmdbuf[_param_l++] = (cmd.UpAndDown ? 0 : 1);
	_after();
}

void saver_dispatcher::command(CPP_ARC &cmd) {
	_before(PP_ARC);
	_param_l = sprintf(_cmdbuf, "%u,%u,%u", cmd.startPoint, cmd.midPoint, cmd.endPoint);
	_write_spd(cmd.speed);
	_after();
}

void saver_dispatcher::command(CPR_ARC &cmd) {
	_before(PR_ARC);
	_param_l = sprintf(_cmdbuf, "%u,%u,%.1f", cmd.startPoint, cmd.endPoint, cmd.radius);
	_write_spd(cmd.speed);
	_after();
}

void saver_dispatcher::command(CPZ_ARC &cmd) {
	_before(PZ_ARC);
	_param_l = sprintf(_cmdbuf, "%u,%u,%u,%.1f", cmd.startPoint, cmd.midPoint, cmd.endPoint, cmd.deltaZ);
	_write_spd(cmd.speed);
	_after();
}

void saver_dispatcher::command(CPRZ_ARC &cmd) {
	_before(PRZ_ARC);
	_param_l = sprintf(_cmdbuf, "%u,%u,%.1f,%.1f", cmd.startPoint, cmd.endPoint, cmd.radius, cmd.deltaZ);
	_write_spd(cmd.speed);
	_after();
}

void saver_dispatcher::command(CLINE &cmd) {
	_before(LINE);
	_param_l = sprintf(_cmdbuf, "%.1f,%.1f,%.1f", cmd.dx, cmd.dy, cmd.dz);
	_write_spd(cmd.speed);
	_after();
}

void saver_dispatcher::command(CARC &cmd) {
	_before(ARC);
	_param_l = sprintf(_cmdbuf, "%.1f,%.1f,%.1f", cmd.radius, cmd.al, cmd.fi);
	_write_spd(cmd.speed);
	_after();
}

void saver_dispatcher::command(CREL_ARC &cmd) {
	_before(REL_ARC);
	_param_l = sprintf(_cmdbuf, "%.1f,%.1f,%.1f", cmd.dx, cmd.dy, cmd.radius);
	_write_spd(cmd.speed);
	_after();
}

void saver_dispatcher::command(CSET_PARK &cmd) {
	_simple(SET_PARK);
}

void saver_dispatcher::command(CGO_PARK &cmd) {
	_simple(GO_PARK);
}

void saver_dispatcher::command(CSET_ZERO &cmd) {
	_simple(SET_ZERO);
}

void saver_dispatcher::command(CGO_ZERO &cmd) {
	_simple(GO_ZERO);
}

void saver_dispatcher::command(CON &cmd) {
	_before(ON);
	_param_l = sprintf(_cmdbuf, "%u", cmd.device);
	_after();
}

void saver_dispatcher::command(COFF &cmd) {
	_before(OFF);
	_param_l = sprintf(_cmdbuf, "%u", cmd.device);
	_after();
}

void saver_dispatcher::command(CSPEED &cmd) {
	assert(cmd.speed >= 1 && cmd.speed <= 8);
	_before(SPEED);
	_param_l = sprintf(_cmdbuf, "%u", cmd.speed);
	_after();
}

inline void saver_dispatcher::_scale(ECmdId cmd_id, unsigned oldscale, unsigned newscale, bool relative)
{
	_before(cmd_id);
	_param_l = sprintf(_cmdbuf, "%u,%u", oldscale, newscale);
	_cmdbuf[_param_l++]=static_cast<unsigned char>(0xfd);
	_cmdbuf[_param_l++]=relative ? 1 : 0;
	_after();
}

void saver_dispatcher::command(CSCALEX &cmd) {
	_scale(SCALE_X, cmd.old_scale, cmd.new_scale, cmd.relative);
}

void saver_dispatcher::command(CSCALEY &cmd) {
	_scale(SCALE_Y, cmd.old_scale, cmd.new_scale, cmd.relative);
}

void saver_dispatcher::command(CSCALEZ &cmd) {
	_scale(SCALE_Z, cmd.old_scale, cmd.new_scale, cmd.relative);
}

void saver_dispatcher::command(CTURN &cmd) {
	_before(TURN);
	_param_l = sprintf(_cmdbuf, "%u,%u,%.1f", cmd.mirrorX, cmd.mirrorY, cmd.angle);
	_after();
}

inline void saver_dispatcher::_text(ECmdId cmd_id, string str)
{
	_before(cmd_id);
	strcpy(_cmdbuf, str.c_str());
	_param_l = strlen(_cmdbuf);
	_after();
}

void saver_dispatcher::command(CSUB &cmd) {
	_text(SUB, cmd.name);
}

void saver_dispatcher::command(CCALL &cmd) {
	_text(CALL, cmd.sub_name);
}

inline void saver_dispatcher::_simple(ECmdId cmd_id)
{
	_before(cmd_id);
	_after();
}

void saver_dispatcher::command(CRET &cmd) {
	_simple(RET);
}

void saver_dispatcher::command(CLABEL &cmd) {
	_text(LABEL, cmd.name);
}

void saver_dispatcher::command(CGOTO &cmd) {
	_text(GOTO, cmd.label_name);
}

void saver_dispatcher::command(CLOOP &cmd) {
	_before(LOOP);
	_param_l = sprintf(_cmdbuf, "%u", cmd.n);
	_after();
}

void saver_dispatcher::command(CENDLOOP &cmd) {
	_simple(ENDLOOP);
}

void saver_dispatcher::command(CSTOP &cmd) {
	_simple(STOP);
}

void saver_dispatcher::command(CFINISH &cmd) {
	_simple(FINISH);
}

void saver_dispatcher::command(CPAUSE &cmd) {
	_before(PAUSE);
	_param_l = sprintf(_cmdbuf, "%.1f", cmd.delay);
	_after();
}

void saver_dispatcher::command(CCOMMENT &cmd) {
	_text(COMMENT, cmd.comment);
}

void saver_dispatcher::command(CSPLINE &cmd)
{
	_before(PAUSE);
	_param_l = sprintf(_cmdbuf, "%i,%i,%i,%i", cmd.p1, cmd.p2, cmd.p3, cmd.p4);
	_after();
}

namespace Kamea
{
	class func1 {
		dispatcher &dispatcher;
	public:
		func1(dispatcher &dispatcher) : dispatcher(dispatcher) {}
		void operator () (command *pcommand) {assert(pcommand); pcommand->dispatch(dispatcher);}
	};

	class func2 {
		ostream &stream;
	public:
		func2(ostream &stream) : stream(stream) {}
		void operator () (point &point) {
			short int x=static_cast<int>(point.x*10), y=static_cast<int>(point.y*10);
			stream.write(reinterpret_cast<char*>(&x), 2);
			stream.write(reinterpret_cast<char*>(&y), 2);
		};
	};
}

void save(ostream &stream, program &program)
{
	// saving comands
	size_t commands = program.getCommands().size();
	stream.write(reinterpret_cast<char*>(&commands), 2);
	saver_dispatcher dispatcher(stream);
	func1 func1(dispatcher);
	for_each(program.getCommands().begin(), program.getCommands().end(), func1);
	// saving points
	size_t points = program.points.size();
	stream.write(reinterpret_cast<char*>(&points), 2);
	func2 func2(stream);
	for_each(program.points.begin(), program.points.end(), func2);
}

} // namespace Kamea
} // namespace Denisenko