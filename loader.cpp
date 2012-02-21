#include "stdafx.h"
#include "file.h"
#include "opcodes.h"
#include "operations.h"
#include "errors.h"

using std::istream;
using std::ios;
using std::auto_ptr;

Kamea::program Kamea::load(istream &stream)
{
	program program;
	stream.exceptions(ios::eofbit | ios::badbit | ios::failbit);
	unsigned short cmds_num;
	stream.read(reinterpret_cast<char*>(&cmds_num), 2);
	for (unsigned i = 0; i < cmds_num; i++)
	{
		char cmd_id = stream.get();
		if (cmd_id > 0x1f)
			throw invalid_command_id("", i, cmd_id);
		// безопасно т.к. на предыдущем шаге значение было проверено
		ECmdId cmdId;
		cmdId = static_cast<ECmdId>(cmd_id);

		unsigned param_l = stream.get();
		if (param_l > 30)
			throw invalid_command_length("", i, cmdId, param_l);

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
				throw invalid_command_format("", i, cmdId);
			if (spd > SPD8 || spd < SPDDEF)
				throw invalid_speed_value("", i, cmdId, spd);
			program.addCommand(auto_ptr<command>(new CPP_LINE(startPoint, endPoint, dz, updown, static_cast<ESpeed>(spd))));
			break;

		case PP_ARC:
			res = sscanf(cmdbuf, "%u,%u,%u,%u", &startPoint, &midPoint, &endPoint, &spd);	
			if (res < 3)
				throw invalid_command_format("", i, cmdId);
			if (spd > SPD8 || spd < SPDDEF)
				throw invalid_speed_value("", i, cmdId, spd);
			program.addCommand(auto_ptr<command>(new CPP_ARC(startPoint, midPoint, endPoint, static_cast<ESpeed>(spd))));
			break;

		case PR_ARC:
			res = sscanf(cmdbuf, "%u,%u,%f,%u", &startPoint, &endPoint, &radius, &spd);	
			if (res < 3)
				throw invalid_command_format("", i, cmdId);
			if (spd > SPD8 || spd < SPDDEF)
				throw invalid_speed_value("", i, cmdId, spd);
			program.addCommand(auto_ptr<command>(new CPR_ARC(startPoint, endPoint, radius, static_cast<ESpeed>(spd))));
			break;

		case PZ_ARC:
			res = sscanf(cmdbuf, "%u,%u,%u,%f,%u", &startPoint, &midPoint, &endPoint, &dz, &spd);	
			if (res < 4)
				throw invalid_command_format("", i, cmdId);
			if (spd > SPD8 || spd < SPDDEF)
				throw invalid_speed_value("", i, cmdId, spd);
			program.addCommand(auto_ptr<command>(new CPZ_ARC(startPoint, midPoint, endPoint, dz, static_cast<ESpeed>(spd))));
			break;

		case PRZ_ARC:
			res = sscanf(cmdbuf, "%u,%u,%u,%f,%u", &startPoint, &endPoint, &radius, &dz, &spd);	
			if (res < 4)
				throw invalid_command_format("", i, cmdId);
			if (spd > SPD8 || spd < SPDDEF)
				throw invalid_speed_value("", i, cmdId, spd);
			program.addCommand(auto_ptr<command>(new CPRZ_ARC(startPoint, endPoint, radius, dz, static_cast<ESpeed>(spd))));
			break;

		case LINE:
			res = sscanf(cmdbuf, "%f,%f,%f,%u", &dx, &dy, &dz, &spd);	
			if (res < 3)
				throw invalid_command_format("", i, cmdId);
			if (spd > SPD8 || spd < SPDDEF)
				throw invalid_speed_value("", i, cmdId, spd);
			program.addCommand(auto_ptr<command>(new CLINE(dx, dy, dz, static_cast<ESpeed>(spd))));
			break;

		case ARC:
			res = sscanf(cmdbuf, "%f,%f,%f,%u", &radius, &al, &fi, &spd);	
			if (res < 3)
				throw invalid_command_format("", i, cmdId);
			if (spd > SPD8 || spd < SPDDEF)
				throw invalid_speed_value("", i, cmdId, spd);
			program.addCommand(auto_ptr<command>(new CARC(radius, al, fi, static_cast<ESpeed>(spd))));
			break;

		case REL_ARC:
			res = sscanf(cmdbuf, "%f,%f,%f,%u", &dx, &dy, &radius, &spd);	
			if (res < 3)
				throw invalid_command_format("", i, cmdId);
			if (spd > SPD8 || spd < SPDDEF)
				throw invalid_speed_value("", i, cmdId, spd);
			program.addCommand(auto_ptr<command>(new CREL_ARC(dx, dy, radius, static_cast<ESpeed>(spd))));
			break;

		case SET_PARK: program.addCommand(auto_ptr<command>(new CSET_PARK)); break;
		case GO_PARK: program.addCommand(auto_ptr<command>(new CGO_PARK)); break;
		case SET_ZERO: program.addCommand(auto_ptr<command>(new CSET_ZERO)); break;
		case GO_ZERO: program.addCommand(auto_ptr<command>(new CGO_ZERO)); break;

		case ON:
			res = sscanf(cmdbuf, "%u", &device);
			if (res < 1)
				throw invalid_command_format("", i, cmdId);
			if (device != SPINDEL)
				throw invalid_device("", i, cmdId, device);
			program.addCommand(auto_ptr<command>(new CON(static_cast<EDevice>(device))));
			break;

		case OFF:
			res = sscanf(cmdbuf, "%u", &device);
			if (res < 1)
				throw invalid_command_format("", i, cmdId);
			if (device != SPINDEL)
				throw invalid_device("", i, cmdId, device);
			program.addCommand(auto_ptr<command>(new COFF(static_cast<EDevice>(device))));
			break;

		case SPEED:
			res = sscanf(cmdbuf, "%u", &spd);
			if (res < 1)
				throw invalid_command_format("", i, cmdId);
			if (spd < SPD1 || spd > SPD8)
				throw invalid_speed_value("", i, cmdId, spd);
			program.addCommand(auto_ptr<command>(new CSPEED(static_cast<ESpeed>(spd))));
			break;

		case SCALE_X:
			relative = (cmdbuf[param_l-1] == 1);
			for (char *ptr = cmdbuf; ptr != cmdbuf+param_l; ptr++)
				if (*ptr < ' ') *ptr = ' ';
			res = sscanf(cmdbuf, "%u,%u", &old_scale, &new_scale);
			if (res < 2)
				throw invalid_command_format("", i, cmdId);
			program.addCommand(auto_ptr<command>(new CSCALEX(old_scale, new_scale, relative)));
			break;

		case SCALE_Y:
			relative = (cmdbuf[param_l-1] == 1);
			for (char *ptr = cmdbuf; ptr != cmdbuf+param_l; ptr++)
				if (*ptr < ' ') *ptr = ' ';
			res = sscanf(cmdbuf, "%u,%u", &old_scale, &new_scale);
			if (res < 2)
				throw invalid_command_format("", i, cmdId);
			program.addCommand(auto_ptr<command>(new CSCALEY(old_scale, new_scale, relative)));
			break;

		case SCALE_Z:
			relative = (cmdbuf[param_l-1] == 1);
			for (char *ptr = cmdbuf; ptr != cmdbuf+param_l; ptr++)
				if (*ptr < ' ') *ptr = ' ';
			res = sscanf(cmdbuf, "%u,%u", &old_scale, &new_scale);
			if (res < 2)
				throw invalid_command_format("", i, cmdId);
			program.addCommand(auto_ptr<command>(new CSCALEZ(old_scale, new_scale, relative)));
			break;

		case TURN:
			res = sscanf(cmdbuf, "%u,%u,%f", &mirrorX, &mirrorY, &angle);
			if (res < 3)
				throw invalid_command_format("", i, cmdId);
			program.addCommand(auto_ptr<command>(new CTURN(mirrorX > 0 ? true : false, mirrorY > 0 ? true : false, angle)));
			break;

		case SUB: program.addCommand(auto_ptr<command>(new CSUB(cmdbuf))); break;
		case CALL: program.addCommand(auto_ptr<command>(new CCALL(cmdbuf))); break;
		case RET: program.addCommand(auto_ptr<command>(new CRET)); break;
		case LABEL: program.addCommand(auto_ptr<command>(new CLABEL(cmdbuf))); break;
		case GOTO: program.addCommand(auto_ptr<command>(new CGOTO(cmdbuf))); break;

		case LOOP:
			res = sscanf(cmdbuf, "%u", &n);
			if (res < 1)
				throw invalid_command_format("", i, cmdId);
			program.addCommand(auto_ptr<command>(new CLOOP(n)));
			break;

		case ENDLOOP: program.addCommand(auto_ptr<command>(new CENDLOOP)); break;
		case STOP: program.addCommand(auto_ptr<command>(new CSTOP)); break;
		case FINISH: program.addCommand(auto_ptr<command>(new CFINISH)); break;

		case PAUSE:
			res = sscanf(cmdbuf, "%f", &delay);
			if (res < 1)
				throw invalid_command_format("", i, cmdId);
			program.addCommand(auto_ptr<command>(new CPAUSE(delay)));
			break;

		case COMMENT: program.addCommand(auto_ptr<command>(new CCOMMENT(cmdbuf))); break;

		case SPLINE:
			res = sscanf(cmdbuf, "%i%i%i%i", &p1, &p2, &p3, &p4);
			if (res < 4)
				throw invalid_command_format("", i, cmdId);
			program.addCommand(auto_ptr<command>(new CSPLINE(p1, p2, p3, p4)));
			break;
		}
	}

	unsigned short points_num;
	stream.read(reinterpret_cast<char*>(&points_num), 2);
	program.points.resize(points_num);
	while (points_num--) {
		short xy[2];
		stream.read(reinterpret_cast<char*>(xy), 4);
		program.points.push_back(point(xy[0]/10.0f, xy[1]/10.0f));
	}

	return program;
}