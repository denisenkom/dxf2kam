#include "stdafx.h"
#include "modeler.h"
#include "arc.h"

using namespace Kamea;

namespace mgt = MyGeometryTools;
using mgt::vec2f;
using mgt::vec3f;
using mgt::matrix2f;
using std::auto_ptr;

Kamea::ProgramWriter::ProgramWriter(void)
	: speed(SPD8), pos(0, 0, 0), scale(1, 1, 1), rotate_angle(0),
	move_speed(SPD8), cut_speed(SPD5), move_z(10)
{
	mirror_xy[0] = false;
	mirror_xy[1] = false;
	program.addCommand(std::auto_ptr<command>(new CSPEED(SPD8)));
	program.addCommand(std::auto_ptr<command>(new CGO_ZERO));
	program.addCommand(std::auto_ptr<command>(new CSCALEX(1, 1)));
	program.addCommand(std::auto_ptr<command>(new CSCALEY(1, 1)));
	program.addCommand(std::auto_ptr<command>(new CSCALEZ(1, 1)));
	program.addCommand(std::auto_ptr<command>(new CTURN(0, 0, 0)));
}

vec3f Kamea::ProgramWriter::tform(vec3f vec)
{
	matrix2f mtform = matrix2f::rotate(rotate_angle)*matrix2f::scale((mirror_xy[0] ? -1 : 1)*scale.x, (mirror_xy[1] ? -1 : 1)*scale.y);
	return vec3f(mtform*vec2f(vec), vec.z*scale.z);
}

void Kamea::ProgramWriter::setSpeed(ESpeed spd)
{
	if (spd == SPDDEF || spd == this->speed)
		return;

	program.addCommand(std::auto_ptr<command>(new CSPEED(spd)));
	speed = spd;
}

ESpeed Kamea::ProgramWriter::getSpeed(void)
{
	return speed;
}

void Kamea::ProgramWriter::switchDevice(EDevice dev, bool on)
{
	if (spindle == on)
		return;

	if (on)
		program.addCommand(std::auto_ptr<command>(new CON(dev)));
	else
		program.addCommand(std::auto_ptr<command>(new COFF(dev)));
	spindle = on;
}

bool Kamea::ProgramWriter::getDevice(EDevice dev)
{
	return spindle;
}

void Kamea::ProgramWriter::setMirror(const bool xy[2])
{
	if (mirror_xy[0] == mirror_xy[0] || mirror_xy[1] == mirror_xy[1])
		return;
	program.addCommand(auto_ptr<command>(new CTURN(xy[0], xy[1], rotate_angle)));
	mirror_xy[0] = xy[0];
	mirror_xy[1] = xy[1];
}

void Kamea::ProgramWriter::getMirror(bool xy[2])
{
	xy = mirror_xy;
}

void Kamea::ProgramWriter::setRotate(float angle)
{
	if (rotate_angle == angle)
		return;
	program.addCommand(auto_ptr<command>(new CTURN(mirror_xy[0], mirror_xy[1], angle)));
	rotate_angle = angle;
}

float Kamea::ProgramWriter::getRotate(void)
{
	return rotate_angle;
}

void Kamea::ProgramWriter::setScale(const float xyz[3])
{
	if (scale[0] != xyz[0])
		program.addCommand(auto_ptr<command>(new CSCALEX(100, unsigned(100*xyz[0]))));
	if (scale[1] != xyz[1])
		program.addCommand(auto_ptr<command>(new CSCALEY(100, unsigned(100*xyz[1]))));
	if (scale[2] != xyz[2])
		program.addCommand(auto_ptr<command>(new CSCALEZ(100, unsigned(100*xyz[2]))));
	scale = xyz;
}

void Kamea::ProgramWriter::getScale(float xy[2])
{
	xy = scale;
}

void Kamea::ProgramWriter::displace(const float disp[3], ESpeed spd)
{
	if (disp[0] == 0 && disp[1] == 0 && disp[2] == 0)
		return;
	program.addCommand(std::auto_ptr<command>(new CLINE(disp[0], disp[1], disp[2], spd != speed ? spd : SPDDEF)));
	pos = pos + tform(disp);
}

void Kamea::ProgramWriter::arc(float rad, float al, float fi, ESpeed spd)
{
	if (fi == 0)
		return;
	program.addCommand(std::auto_ptr<command>(new CARC(rad, al, fi, spd != speed ? spd : SPDDEF)));
	mgt::arcf arc = mgt::arcf::by_pt_rad_ang(pos, rad, al, fi);
	vec2f newpos = arc.calcEndPoint();
	newpos = vec2f(tform(vec3f(newpos - vec2f(pos), 0)));
	pos.x = newpos.x;
	pos.y = newpos.y;
}

void Kamea::ProgramWriter::moveto(const float xy[2])
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

/*void Kamea::ProgramWriter::line(const float xyz1[3], const float xyz2[3])
{
	//if ((vec2d<float>(pos_xyz)-vec2d<float>(xyz1)).len() > .1f)
	moveto(xyz1);
	setSpeed(cut_speed);
	displace(vec3f(0, 0, xyz1[2]));
	displace(vec3f(xyz2) - vec3f(xyz1));
}*/

mgt::vec3f Kamea::ProgramWriter::getPos(void)
{
	return pos;
}

void Kamea::ProgramWriter::begin()
{
}

Kamea::Program Kamea::ProgramWriter::end()
{
	return std::move(program);
}
