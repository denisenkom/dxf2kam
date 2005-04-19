#include "stdafx.h"
#include "Kamea.h"

using namespace std;

namespace Denisenko {
namespace Dxf2Kam {
namespace Kamea {

void Program::LoadKam(const char *fileName)
{
	ifstream stream(fileName);
	stream.exceptions(ios::eofbit | ios::badbit | ios::failbit);
	unsigned short num;
	stream.read(reinterpret_cast<char*>(&num), 2);
	while (num--)
	{
		char id = stream.get();
		if (id > 0x1f)
			throw InvalidOpcode(); //("", i, id);
		// безопасно т.к. на предыдущем шаге значение было проверено
		Statement::OpcodeEnum cmdId;
		cmdId = static_cast<Statement::OpcodeEnum>(id);

		unsigned len = stream.get();
		if (len > 30)
			throw InvalidCommandLength(); //"", i, cmdId, param_l);

		char cmdbuf[31];
		stream.read(cmdbuf, 30);
		cmdbuf[len] = 0;

		unsigned spd = 0, device, mirrorX, mirrorY, n, oldScale, newScale;
		unsigned points[4];
		int res;
		float dx, dy, dz, radius, al, fi, angle, delay;
		bool updown, relative;

		switch (cmdId) {
		case Statement::PPLine:
			updown = (cmdbuf[len - 1] == 0);
			for (char *ptr = cmdbuf; ptr != cmdbuf + len; ptr++)
				if (*ptr < ' ') *ptr = ' ';
			res = sscanf(cmdbuf, "%u,%u,%f,%u", &points[0], &points[1], &dz, &spd);
			if (res < 3)
				throw InvalidCommandFormat(); //"", i, cmdId);
			if (spd > 8 || spd < 0)
				throw InvalidSpeedValue(); //"", i, cmdId, spd);
			Statements.push_back(new PPLine(points, dz, updown, spd));
			break;

		case Statement::PPArc:
			res = sscanf(cmdbuf, "%u,%u,%u,%u", &points[0], &points[1], &points[2], &spd);	
			if (res < 3)
				throw InvalidCommandFormat(); //"", i, cmdId);
			if (spd > 8 || spd < 0)
				throw InvalidSpeedValue(); //"", i, cmdId, spd);
			Statements.push_back(new PPArc(points, spd));
			break;

		case Statement::PRArc:
			res = sscanf(cmdbuf, "%u,%u,%f,%u", &points[0], &points[1], &radius, &spd);	
			if (res < 3)
				throw InvalidCommandFormat(); //"", i, cmdId);
			if (spd > 8 || spd < 0)
				throw InvalidSpeedValue(); //"", i, cmdId, spd);
			Statements.push_back(new PRArc(points, radius, spd));
			break;

		case Statement::PZArc:
			res = sscanf(cmdbuf, "%u,%u,%u,%f,%u", &points[0], &points[1], &points[2], &dz, &spd);	
			if (res < 4)
				throw InvalidCommandFormat(); //"", i, cmdId);
			if (spd > 8 || spd < 0)
				throw InvalidSpeedValue(); //"", i, cmdId, spd);
			Statements.push_back(new PZArc(points, dz, spd));
			break;

		case Statement::PRZArc:
			res = sscanf(cmdbuf, "%u,%u,%u,%f,%u", &points[0], &points[1], &radius, &dz, &spd);	
			if (res < 4)
				throw InvalidCommandFormat(); //"", i, cmdId);
			if (spd > 8 || spd < 0)
				throw InvalidSpeedValue(); //"", i, cmdId, spd);
			Statements.push_back(new PRZArc(points, radius, dz, spd));
			break;

		case Statement::Line:
			res = sscanf(cmdbuf, "%f,%f,%f,%u", &dx, &dy, &dz, &spd);	
			if (res < 3)
				throw InvalidCommandFormat(); //"", i, cmdId);
			if (spd > 8 || spd < 0)
				throw InvalidSpeedValue(); //"", i, cmdId, spd);
			Statements.push_back(new Line(dx, dy, dz, spd));
			break;

		case Statement::Arc:
			res = sscanf(cmdbuf, "%f,%f,%f,%u", &radius, &al, &fi, &spd);	
			if (res < 3)
				throw InvalidCommandFormat(); //"", i, cmdId);
			if (spd > 8 || spd < 0)
				throw InvalidSpeedValue(); //"", i, cmdId, spd);
			Statements.push_back(new Arc(radius, al, fi, spd));
			break;

		case Statement::RelArc:
			res = sscanf(cmdbuf, "%f,%f,%f,%u", &dx, &dy, &radius, &spd);	
			if (res < 3)
				throw InvalidCommandFormat(); //"", i, cmdId);
			if (spd > 8 || spd < 0)
				throw InvalidSpeedValue(); //"", i, cmdId, spd);
			Statements.push_back(new RelArc(dx, dy, radius, spd));
			break;

		case Statement::On:
		case Statement::Off:
			res = sscanf(cmdbuf, "%u", &device);
			if (res < 1)
				throw InvalidCommandFormat(); //"", i, cmdId);
			Statements.push_back(new OnOff(cmdId, device));
			break;

		case Statement::Speed:
			res = sscanf(cmdbuf, "%u", &spd);
			if (res < 1)
				throw InvalidCommandFormat(); //"", i, cmdId);
			if (spd < 1 || spd > 8)
				throw InvalidSpeedValue(); //"", i, cmdId, spd);
			Statements.push_back(new Speed(spd));
			break;

		case Statement::ScaleX:
		case Statement::ScaleY:
		case Statement::ScaleZ:
			relative = (cmdbuf[len - 1] == 1);
			for (char *ptr = cmdbuf; ptr != cmdbuf + len; ptr++)
				if (*ptr < ' ') *ptr = ' ';
			res = sscanf(cmdbuf, "%u,%u", &oldScale, &newScale);
			if (res < 2)
				throw InvalidCommandFormat(); //"", i, cmdId);
			Statements.push_back(new Scale(cmdId, oldScale, newScale, relative));
			break;

		case Statement::Turn:
			res = sscanf(cmdbuf, "%u,%u,%f", &mirrorX, &mirrorY, &angle);
			if (res < 3)
				throw InvalidCommandFormat(); //"", i, cmdId);
			Statements.push_back(new Turn(mirrorX > 0 ? true : false, mirrorY > 0 ? true : false, angle));
			break;

		case Statement::Sub:
		case Statement::Call:
		case Statement::Label:
		case Statement::Goto:
		case Statement::Comment:
			Statements.push_back(new Label(cmdbuf, cmdId));
			break;

		case Statement::Loop:
			res = sscanf(cmdbuf, "%u", &n);
			if (res < 1)
				throw InvalidCommandFormat(); //"", i, cmdId);
			Statements.push_back(new Loop(n));
			break;

		case Statement::Pause:
			res = sscanf(cmdbuf, "%f", &delay);
			if (res < 1)
				throw InvalidCommandFormat(); //"", i, cmdId);
			Statements.push_back(new Pause(delay));
			break;


		case Statement::Spline:
			res = sscanf(cmdbuf, "%i%i%i%i", &points[0], &points[1], &points[2], &points[3]);
			if (res < 4)
				throw InvalidCommandFormat(); //"", i, cmdId);
			Statements.push_back(new Spline(points));
			break;
		
		default: // SetPark, GoPark, SetZero, GoZero, Ret, EndLoop, Stop, Finish
			Statements.push_back(new Statement(cmdId));
			break;
		}
	}

	stream.read(reinterpret_cast<char*>(&num), 2);
	while (num--)
	{
		short xy[2];
		stream.read(reinterpret_cast<char*>(xy), 4);
		Points.push_back(Point(xy[0]/10.0f, xy[1]/10.0f));
	}
}

void Program::SaveKam(const char *fileName)
{
	ofstream stream(fileName);

	for (Statements::iterator i = Statements.begin(); i != Statements.end(); i++)
	{
		Statement &s = **i;
		char cmdbuf[31];
		char len;
		bool updownRelat = false;
		bool updownRelatVal;
		switch (s.Opcode)
		{
		case Statement::PPLine:{
			PPLine &s = *(PPLine*)*i;
			sprintf(cmdbuf, "%u,%u,%f,%u", s.PointRefs[0], s.PointRefs[1], s.DeltaZ,
				s.Speed);
			updownRelat = true;
			updownRelatVal = s.UpDown;}
			break;
		case Statement::PPArc:{
			PPArc &s = *(PPArc*)*i;
			sprintf(cmdbuf, "%u,%u,%u,%u", s.PointRefs[0], s.PointRefs[1], s.PointRefs[2],
				s.Speed);}
			break;
		case Statement::PRArc:{
			PRArc &s = *(PRArc*)*i;
			sprintf(cmdbuf, "%u,%u,%f,%u", s.PointRefs[0], s.PointRefs[1], s.Radius,
				s.Speed);}			
			break;
		case Statement::PZArc:{
			PZArc &s = *(PZArc*)*i;
			sprintf(cmdbuf, "%u,%u,%u,%f,%u", s.PointRefs[0], s.PointRefs[1],
				s.PointRefs[2], s.DeltaZ, s.Speed);}
			break;
		case Statement::PRZArc:{
			PRZArc &s = *(PRZArc*)*i;
			sprintf(cmdbuf, "%u,%u,%u,%f,%u", s.PointRefs[0], s.PointRefs[1], s.Radius,
				s.DeltaZ, s.Speed);}
			break;
		case Statement::Line:{
			Line &s = *(Line*)*i;
			sprintf(cmdbuf, "%f,%f,%f,%u", s.DeltaX, s.DeltaY, s.DeltaZ, s.Speed);}
			break;
		case Statement::Arc:{
			Arc &s = *(Arc*)*i;
			sprintf(cmdbuf, "%f,%f,%f,%u", s.Radius, s.StartAngle, s.AngleSpan, s.Speed);}
			break;
		case Statement::RelArc:{
			RelArc &s = *(RelArc*)*i;
			sprintf(cmdbuf, "%f,%f,%f,%u", s.DeltaX, s.DeltaY, s.Radius, s.Speed);}
			break;
		case Statement::On:
		case Statement::Off:{
			OnOff &s = *(OnOff*)*i;
			sprintf(cmdbuf, "%u", s.Device);}
			break;
		case Statement::Speed:{
			Speed &s = *(Speed*)*i;
			sprintf(cmdbuf, "%u", s.Value);}
			break;
		case Statement::ScaleX:
		case Statement::ScaleY:
		case Statement::ScaleZ:{
			Scale &s = *(Scale*)*i;
			sprintf(cmdbuf, "%u,%u", s.Old, s.New);
			updownRelat = true;
			updownRelatVal = s.Relative;}
			break;
		case Statement::Turn:{
			Turn &s = *(Turn*)*i;
			sprintf(cmdbuf, "%u,%u,%f", s.MirrorX, s.MirrorY, s.Angle);}
			break;
		case Statement::Sub:
		case Statement::Call:
		case Statement::Label:
		case Statement::Goto:
		case Statement::Comment:{
			Label &s = *(Label*)*i;
			strcpy(cmdbuf, s.Value.c_str());}
			break;
		case Statement::Loop:{
			Loop &s = *(Loop*)*i;
			sprintf(cmdbuf, "%u", s.Iterations);}
			break;
		case Statement::Pause:{
			Pause &s = *(Pause*)*i;
			sprintf(cmdbuf, "%f", s.Delay);}
			break;
		case Statement::Spline:{
			Spline &s = *(Spline*)*i;
			sprintf(cmdbuf, "%i%i%i%i", s.PointRefs[0], s.PointRefs[1], s.PointRefs[2],
				s.PointRefs[3]);}
			break;
		default:
			assert(0);
		}
		len = char(strlen(cmdbuf));
		if (updownRelat)
		{
			cmdbuf[len] = char(!updownRelatVal);
			len++;
		}
		stream.put(char(s.Opcode));
		stream.put(len);
		stream.write(cmdbuf, 30);
	}
}

} // namespace Kamea
} // namespace Dxf2Kam
} // namespace Denisenko