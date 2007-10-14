#include "stdafx.h"
#include "dxf.h"


namespace Denisenko {
namespace Dxf2Kam {
namespace Dxf {

using namespace std;

const short MAXSHORT = short(0x7fff);
const short MINSHORT = short(0x8000);
const int MAX_LINE_LENGTH = 1024;

void Database::Load(const char *fileName)
{
	_stack.push(this);
	ifstream fs(fileName);
	if (!fs)
		throw ParseError(); // unable to open file

	while (fs)
	{
		char buf[MAX_LINE_LENGTH];
		fs.getline(buf, sizeof(buf));
		int code = atoi(buf);

		fs.getline(buf, sizeof(buf));
		string str(buf);

		if (code == 0)
		{
			if (str == "EOF")
				return;
			else if (str == "ENDSEC")
			{
				while (_stack.size() > 1)
				{
					Node *b = _stack.top();
					_stack.pop();
					_stack.top()->Children.insert(Nodes::value_type(
						b->Attributes.begin()->second.GetValue(), b));
				}
			}
			else if (str == "ENDTAB" || str == "ENDBLK")
			{
				Node *b = _stack.top();
				_stack.pop();
				_stack.top()->Children.insert(Nodes::value_type(
					b->Attributes.begin()->second.GetValue(), b));
			}
			else // new node
			{
				_stack.push(new Node());
				_stack.top()->Attributes.insert(Attributes::value_type(code,
					Attribute(0, str)));
			}
		}
		else
		{
			if (code >= 10 && code <= 39)
			{
				// vector param
				int loDigit = code % 10;
				int hiDigit = code / 10;
				_xyz[loDigit][hiDigit] = str;
				if (hiDigit == 3)
					_stack.top()->Attributes.insert(Attributes::value_type(loDigit,
						Attribute(loDigit, _xyz[loDigit])));
			}
			else
				// scalar param
				_stack.top()->Attributes.insert(
					Attributes::value_type(code, Attribute(code, str)));
		}
	}
}

Dxf::Attribute::Attribute(unsigned c, std::string value)
{
	_sVal[0] = value;
	_code = c;

	// decoding type

	if (c <= 9) // 0-9
		_type = String; // String (with the introduction of extended symbol names in AutoCAD 2000, the 255-character limit has been lifted. There is no explicit limit to the number of bytes per line, although most lines should fall within 2049 bytes)
	else if (c <= 39) // 10-39
		assert(0); // this case should be processed in procedure below
	else if (c <= 59) // 40-59
		_type = Double; // Double-precision floating-point value
	else if (c <= 79) // 60-79
		_type = Short; // 16-bit integer value
	else if (c <= 89) // 80-89
		throw ParseError(); // invalid code
	else if (c <= 99) // 90-79
		_type = Long; // 32-bit integer value
	else if (c == 100)
		_type = String; // String (255-character maximum; less for Unicode strings)
	else if (c == 101)
		throw ParseError(); // invalid code
	else if (c == 102)
		_type = String; // String (255-character maximum; less for Unicode strings)
	else if (c <= 104) // 103-104
		throw ParseError(); // invalid code
	else if (c == 105)
		_type = String; // String representing hexadecimal (hex) handle value
	else if (c <= 109) // 106-109
		throw ParseError(); // invalid code
	else if (c <= 149) // 110-119 120-129 130-139 140-149
		_type = Double;
	else if (c <= 169) // 150-169
		throw ParseError(); // invalid code
	else if (c <= 179) // 170-179
		_type = Short;
	else if (c <= 209) // 180-209
		throw ParseError(); // invalid code
	else if (c <= 239) // 210-239
		_type = Double;
	else if (c <= 269) // 240-269
		throw ParseError(); // invalid code
	else if (c <= 289) // 270-279 280-289
		_type = Short;
	else if (c <= 299) // 290-299
		_type = Boolean; // Boolean flag value
	else if (c <= 309) // 300-309
		_type = String; // Arbitrary text string
	else if (c <= 319) // 310-319
		_type = String; // String representing hex value of binary chunk
	else if (c <= 329) // 320-329
		_type = String; // String representing hex handle value
	else if (c <= 369) // 330-369
		_type = String; // String representing hex object IDs
	else if (c <= 389) // 370-379 380-389
		_type = Short;
	else if (c <= 399) // 390-399
		_type = String; // String representing hex handle value
	else if (c <= 409) // 400-409
		_type = Short;
	else if (c <= 419) // 410-419
		_type = String; // String
	else if (c <= 429) // 420-429
		_type = Long;
	else if (c <= 439) // 430-439
		_type = String;
	else if (c <= 449) // 440-449
		_type = Long;
	else if (c <= 459) // 450-459
		_type = Long; // Long?
	else if (c <= 469) // 460-469
		_type = Double;
	else if (c <= 479) // 470-479
		_type = String;
	else if (c <= 998) // 480-998
		throw ParseError(); // invalid code
	else if (c == 999) // 999
		_type = String; // Comment (string)
	else if (c <= 1009) // 1000-1009
		_type = String; // String (same limits as indicated with 0–9 code range)
	else if (c <= 1059) // 1010-1059
		_type = Double;
	else if (c <= 1070)	// 1060-1070
		_type = Short;
	else if (c == 1071) // 1071
		_type = Long;
	else                // 1072...
		throw ParseError(); // invalid code
	
	// parsing integer and floating point values

	char *end;
	switch (_type)
	{
	case String:
		break; // do nothing

	case Short:
	case Long:
	case Boolean:
		long res;
		res = strtol(_sVal[0].c_str(), &end, 10);
		if (errno == ERANGE)
			throw ParseError(); // overflow converting integer
		switch (_type)
		{
		case Short:
			assert(MINSHORT <= res && res <= MAXSHORT);
			_shVal = short(res);
			break;
		case Long:
			_lVal = res;
			break;
		case Boolean:
			// TODO: check value
			assert(res == 0 || res == 1);
			_bVal = res == 1 ? true : false;
			break;
		default:
			assert(0); // should never get here
			break;
		}
		break;

	case Double:
		_dVal = strtod(_sVal[0].c_str(), &end);
		if (errno == ERANGE)
			throw ParseError(); // overflow converting double
		break;

	default:
		assert(0); // should never get here
		break;
	}
}

Dxf::Attribute::Attribute(int rem, string value[3])
{
	assert(0 <= rem && rem <= 9);

	_code = 10 + rem;
	_type = Vector;

	// converting value

	double *v = _vecVal;
	char *end;
	for (string* s = value; s != value + sizeof(value)/sizeof(*value); s++, v++)
	{
		*v = strtod(s->c_str(), &end);
		if (errno == ERANGE)
			throw ParseError(); // overflow converting 3D vector values
	}
}


const char * Attribute::GetValue()
{
	return _sVal[0].c_str();
}

Constructor& Constructor::operator <<(Manipulator manip)
{
	Node *node;
	switch (manip)
	{
	case Eof:
		break;
	case EndSection:
		while (_stack.size() > 1)
		{
			node = _stack.top();
			_stack.pop();
			_stack.top()->Children.insert(make_pair(node->Attributes.begin()->second.GetValue(), node));
		}
		break;
	case EndBlock:
	case EndTable:
		node = _stack.top();
		_stack.pop();
		_stack.top()->Children.insert(make_pair(node->Attributes.begin()->second.GetValue(), node));
		break;
	case BeginNode:
		_stack.push(new Node());
		break;
	default:
		assert(0/*invalid manip*/);
		break;
	}
	return *this;
}

Constructor& Constructor::operator <<(Attribute &attr)
{
	_stack.top()->Attributes.insert(Attributes::value_type(attr.GetCode(), attr));
	return *this;
}

void Parser::Parse(istream &stream, Constructor &constructor)
{
	char buf[MAX_LINE_LENGTH];
	string xyz[3];
	while (stream)
	{
		// Getting code
		//
		stream.getline(buf, sizeof(buf));
		int code = atoi(buf);

		// Getting string
		//
		stream.getline(buf, sizeof(buf));
		string str = buf;

		if (code == 0) // 0 code for reserved words and nodes 
		{
			if (str == "EOF")
			{
				constructor << Eof;
				break;
			}
			else if (str == "ENDSEC")
				constructor << EndSection;
			else if (str == "ENDTAB")
				constructor << EndTable;
			else if (str == "ENDBLK")
				constructor << EndBlock;
			else // new node like SECTION ENTITY TABLE BLOCK etc.
			{
				constructor << BeginNode;
				constructor << Attribute(code, str);
			}
		}
		else // other codes is attributes
		{
			if (code >= 10 && code <= 39) // 3d vector attribute
			{
				int rem = code % 10;
				int fix = code / 10;
				xyz[fix] = str;
				if (fix == 3) // got last 3rd value
					constructor << Attribute(rem, xyz[rem]);
			}
			else // scalar attribute	
				constructor << Attribute(code, str);
		}
	}
}

const char * ParseError::GetMessageA()
{
	return "";
}

} // namespace Dxf
} // namespace Dxf2Kam
} // namespace Denisenko