#ifndef __OPCODES_H_INCLUDED__
#define __OPCODES_H_INCLUDED__

// symbols for kamea programs
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

}	// namespace Kamea

#endif	// __OPCODES_H_INCLUDED__