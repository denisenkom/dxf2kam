#include <stdexcept>
#include "opcodes.h"
#include "operations.h"
#include "geometry.h"

namespace Kamea {

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
}	// namespace Kamea