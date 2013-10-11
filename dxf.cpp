// dxf.cpp : Defines the entry point for the console application.
//
#include <stdexcept>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <stack>
#include <cassert>
#include <cstdlib>
#include <cerrno>
#include "dxf.h"
#include "geometry.h"

using namespace std;
using namespace dxf;

using MyGeometryTools::grad2rad;

class EConvertError : public std::runtime_error
{
public:
	EConvertError(const char *cstr)
		: std::runtime_error(cstr)
	{
	}
};

/*dxf::database::~database(void)
{
	for (t_entities::iterator i = entities.begin(); i != entities.end(); i++)
		delete *i;
}

void dxf::database::add_entity(auto_ptr<entity> pentity)
{
	assert(pentity.get());
	entities.push_back(pentity.release());
}

entity& dxf::database::by_index(size_t index)
{
	assert(index < entities.size());
	assert(entities[index]);
	return *entities[index];
}*/

namespace dxf {
	/*class entity {
	public:
		int handle;
		int color;
		double thickness;
		entity(int handle, int color=0, double thickness=0)
			: handle(handle), color(color), thickness(thickness)
		{
		}
		virtual ~entity(void) {};
	};

	class line : public entity {
	public:
		double x1, y1, z1, x2, y2, z2;
		line(int handle, double x1, double y1, double z1,
			double x2, double y2, double z2, double thickness=0, int color=0)
			: entity(handle, color, thickness), x1(x1), y1(y1), z1(z1), x2(x2), y2(y2), z2(z2)
		{
		}
		virtual ~line(void) {};
	};

	class arc : public entity {
	public:
		double x, y, z, radius, start_angle, end_angle;
		arc(int handle, double x, double y, double z,
			double radius, double start_angle, double end_angle,
			double thickness=0, int color=0)
			: entity(handle, color, thickness), x(x), y(y), z(z),
			radius(radius), start_angle(start_angle), end_angle(end_angle)
		{
		}
		virtual ~arc(void) {};
	};

	class circle : public entity {
	public:
		double x, y, z, radius;
		circle(int handle, double x, double y, double z,
			double radius, double thickness=0, int color=0)
			: entity(handle, color, thickness), x(x), y(y), z(z),
			radius(radius)
		{
		}
		virtual ~circle(void) {};
	};

	class ellipse : public entity {
	public:
		double cx, cy, cz, mx, my, mz, ratio, start_angle, end_angle;
		ellipse(int handle, double cx, double cy, double cz,
			double mx, double my, double mz,
			double ratio, double start_angle, double end_angle,
			double thickness=0, int color=0)
			: entity(handle, color, thickness), cx(cx), cy(cy), cz(cz),
			mx(mx), my(my), mz(mz), ratio(ratio),
			start_angle(start_angle), end_angle(end_angle)
		{
		}
		virtual ~ellipse(void) {};
	};

	class database {
		typedef std::vector<entity*> t_entities_vector;
		typedef t_entities_vector t_entities;
		t_entities entities;
	public:
		~database(void);
		void add_entity(std::auto_ptr<entity>);
		entity& by_index(size_t index);
	};*/

	class Reader
	{
		bool next_code_string;
		int code;
	public:
		std::istream &stream;
		
		Reader(std::istream &str)
			:stream(str), next_code_string(true)
		{
		}

		int get_code(void);

		string get_string(void);
		int get_int(void);
		float get_float(void);
	};

	class Parser
	{
		Reader reader;
		//database database;
		Factory &factory;
		void unknown_entity(string entity_name);
		void unknown_section(string section_name);
		void parseParams(int &par5, float &par10, float &par20, float &par30,
			float &par11, float &par21, float &par31, float par39, float &par40,
			float &par41, float &par42, float &par50, float &par51,
			int &par62);
		void parseLayerTable();
		void parseTable();
		void parseTablesSec();
		void parseEntitiesSec();

		//void circle(int handle, double x, double y, double z, double radius, double thickness=0, int color=0);
		//void arc(int handle, double x, double y, double z, double radius, double start_angle, double end_angle, double thickness=0, int color=0);
		//void line(int handle, double x1, double y1, double z1, double x2, double y2, double z2, double thickness=0, int color=0);
		//void ellipse(int handle, double cx, double cy, double cz, double mx, double my, double mz, double ratio, double start_angle, double end_angle, double thickness=0, int color=0);
	public:
		Parser(istream &stream, dxf::Factory &factory);
		void parse(void);
	};
}

int dxf::Reader::get_code(void)
{
	if (!next_code_string)
		return code;
	char buf[1024];
	stream.getline(buf, sizeof(buf));
	next_code_string = false;
	code = atoi(buf);
	return code;
}

string dxf::Reader::get_string(void)
{
	assert(!next_code_string);
	char buf[1024];
	stream.getline(buf, sizeof(buf));
	next_code_string = true;
	return string(buf);
}

float dxf::Reader::get_float(void)
{
	assert(!next_code_string);
	char buf[1024];
	stream.getline(buf, sizeof(buf), '\n');
	next_code_string = true;
	char *stopchar;
	float retval = float(strtod(buf, &stopchar));
	if (stopchar == 0)
		throw EConvertError(buf);
	return retval;
}

int dxf::Reader::get_int(void)
{
	assert(!next_code_string);
	char buf[1024];
	stream.getline(buf, sizeof(buf), '\n');
	next_code_string = true;
	return atoi(buf);
}

dxf::Parser::Parser(istream &stream, dxf::Factory &factory)
	: reader(stream), factory(factory)
{
}

void dxf::Parser::unknown_entity(string entity_name)
{
	cerr << "skip entity - " << entity_name << endl;
	int code;
	string string;
	while(!reader.stream.eof())
	{
		code = reader.get_code();
		if (code == 0)
			break;
		string = reader.get_string();
	}
}

void dxf::Parser::unknown_section(string section_name)
{
	//cerr << "skip section - " << section_name << endl;
	int code;
	string string;
	while(1)
	{
		code = reader.get_code();
		string = reader.get_string();
		if (code == 0 && string == "ENDSEC")
			break;
	}
}

/*void dxf::Parser::parseLayerTable()
{
}

class AcDbSymbolTableRecord {
public:
	static AcDbSymbolTableRecord* from_dxf(dxf::Reader & reader)
	{
		int code = reader.get_code();
		if (code == 100)
		{
			if (reader.get_string() == "AcDbLayerTableRecord")
				return AcDbLayerTableRecord::from_dxf(reader);
		}
		else
			reader.get_string();
	}
};

class AcDbLayerTableRecord : public AcDbSymbolTableRecord
{
public:
	string name;
	char flags;
	int color;
	bool plot_flag;
	int line_weight;
	int hPlotStyleName;
	string line_type_name;

	static AcDbLayerTableRecord * from_dxf(dxf::Reader &reader)
	{
		while (1)
		{
			AcDbLayerTableRecord *rec = new AcDbLayerTableRecord();
			int code = reader.get_code();
			if (code == 2)
				rec->name = reader.get_string();
			else if (code == 70)
				rec->flags = reader.get_int();
			else if (code == 62)
				rec->color = reader.get_int();
			else if (code == 6)
				rec->line_type_name = reader.get_string();
			else if (code == 290)
				rec->plot_flag = reader.get_int();
			else if (code == 370)
				rec->line_weight = reader.get_int();
			else if (code == 390)
				rec->hPlotStyleName = reader.get_int();
			else
				break;
		}
		return rec;
	}
};

class AcDbSymbolTable {
public:
	int max_number_entries;
	static AcDbSymbolTable from_dxf(dxf::Reader &reader)
	{
		AcDbSymbolTable table;
		while (1)
		{
			int code = reader.get_code();
			if (code == 0)
				break;
			else if (code == 70)
				table.max_number_entries = reader.get_int();
			else if (code == 100)
			{
				if (reader.get_string() == "AcDbSymbolTable")
					AcDbSymbolTable::from_dxf(reader);
			}
			else
				reader.get_string();
		}
		return table;
	}
};

void dxf::Parser::parseTable()
{
	string table_name;
	int handle;
	AcDbSymbolTable table;
	while (1)
	{
		int code = reader.get_code();
		if (code == 0)
		{
			if (reader.get_string() == "ENDTAB")
				break;
		}
		else if (code == 2)
			table_name = reader.get_string();
		else if (code == 5)
			handle = reader.get_int();
		else if (code == 100)
		{
			if (reader.get_string() == "AcDbSymbolTable")
				table = AcDbSymbolTable::from_dxf(reader);
			//break;
		}
		else
			reader.get_string();

	}
	//if (table_name == "LAYER")
	//	parseLayerTable();
}

void dxf::Parser::parseTablesSec()
{
	while (1)
	{
		int code = reader.get_code();
		if (code == 0)
		{
			string string = reader.get_string();
			if (string == "TABLE")
				parseTable();
			else if (string == "ENDSEC")
				break;
		}
		else
			reader.get_string();
			//break;
	}
}*/

void dxf::Parser::parseParams(int &par5, float &par10, float &par20, float &par30,
		   float &par11, float &par21, float &par31, float par39, float &par40, float &par41, float &par42,
		   float &par50, float &par51,
		   int &par62)
{
	while (1)
	{
		int code = reader.get_code();
		if (code == 0)
			break;
		else if (code == 5)
			par5 = reader.get_int();
		else if (code == 10)
			par10 = reader.get_float();
		else if (code == 20)
			par20 = reader.get_float();
		else if (code == 30)
			par30 = reader.get_float();
		else if (code == 11)
			par11 = reader.get_float();
		else if (code == 21)
			par21 = reader.get_float();
		else if (code == 31)
			par31 = reader.get_float();
		else if (code == 39)
			par39 = reader.get_float();
		else if (code == 40)
			par40 = reader.get_float();
		else if (code == 41)
			par41 = reader.get_float();
		else if (code == 42)
			par42 = reader.get_float();
		else if (code == 50)
			par50 = reader.get_float();
		else if (code == 51)
			par51 = reader.get_float();
		else if (code == 62)
			par62 = reader.get_int();
		else
			reader.get_string();	// skip
	}
}

void dxf::Parser::parseEntitiesSec()
{
	while(!reader.stream.eof())
	{
		int code = reader.get_code();
		string string = reader.get_string();
		int handle = 0, color = 0;
		float p1[3] = {0, 0, 0}, p2[3] = {0, 0, 0};
		float ratio = 0, start_angle = 0, end_angle = 0, thickness = 0, start_par=0, end_par=0;
		parseParams(handle, p1[0], p1[1], p1[2], p2[0], p2[1], p2[2], thickness, ratio, start_par, end_par, start_angle, end_angle, color);
		if (string == "CIRCLE")
			factory.circle(handle, p1, ratio, thickness, color);
		else if (string == "ARC")
			factory.arc(handle, p1, ratio, grad2rad(start_angle), grad2rad(end_angle), thickness, color);
		else if (string == "LINE")
			factory.line(handle, p1, p2, thickness, color);
		else if (string == "ELLIPSE")
			factory.ellipse(handle, p1, p2, ratio, start_par, end_par, thickness, color);
		else if (string == "POINT")
			factory.point(handle, p1, thickness, color);
		else if (string == "SPLINE")
			factory.spline(handle, thickness, color);
		else if (string == "ENDSEC")
			break;
		else
			unknown_entity(string);
	}
}

void dxf::Parser::parse()
{
	while (!reader.stream.eof())
	{
		int code = reader.get_code();
		string string = reader.get_string();
		if (string == "EOF")
			break;
		else if (string == "SECTION")
		{
			code = reader.get_code();
			string = reader.get_string();
			/*if (string == "TABLES")
				parseTablesSec();
			else*/ if (string == "ENTITIES")
				parseEntitiesSec();
			else
				unknown_section(string);
		}
	}
}

void dxf::parse(istream &reader, Factory &factory)
{
	Parser parser(reader, factory);
	parser.parse();
}

namespace _newloader
{


enum Manipulator {
	Eof,
	EndSection,
	EndTable,
	EndBlock,
	BeginNode
};

class ParseError
{
public:
	const char *GetMessageA();
};

typedef std::map<std::string, class Node*> Nodes;
typedef std::map<unsigned, class Attribute> attributes_t;

class Node
{
public:
	Nodes Children;
	attributes_t Attributes;
};

class Attribute : public Node
{
public:
	enum Type {String, Short, Long, Double, Boolean, Vector};

	const char *GetName();

	//explicit operator (const char *());
	operator int();
	operator float();
	operator float*();

	const char *GetValue();

	Type        GetType() const {return _type;};

	unsigned GetCode() const {return _code;};

	Attribute(unsigned c, std::string value);
	Attribute(int rem, std::string value[3]);

private:
	unsigned _code;
	Type _type;
	std::string _sVal[3];
	union {
		long _lVal;
		short _shVal;
		double _dVal;
		bool _bVal;
		double _vecVal[3];
	};
};

class Constructor
{
	std::string _xyz[10][3];
	std::stack<Node*> _stack;

public:
	Constructor& operator << (Manipulator);
	Constructor& operator << (const Attribute &);
};

class Parser
{
public:
	static void Parse(std::istream &, Constructor &);
};

class Entity : public Node
{
public:
	Entity(Node &source);

	Attribute& Type();          // 0 Entity type

	Attribute& Handle();        // 5 Handle

	Attribute& LayerName();     // 8 Layer name

	Attribute& LinetypeName();  // 6 Linetype name (present if not BYLAYER). The special name BYBLOCK indicates a floating linetype (optional)

	// <summary>
	// Code: 62
	// Color number (present if not BYLAYER); zero indicates the BYBLOCK (floating) color;
	// 256 indicates BYLAYER; a negative value indicates that the layer is turned off (optional) 
	// </summary>
	Attribute& ColorNumber(); 

	Attribute& Lineweight();    // 370 Lineweight enum value. Stored and moved around as a 16-bit integer.

	Attribute& LinetypeScale(); // 48 Linetype scale (optional) (1.0)

	Attribute& Visibility();    // 60 Object visibility (optional): 0 = Visible; 1 = Invisible (0)
};

typedef std::vector<Entity*> Entities;
typedef Entities::iterator EntitiesIt;

class Line : public Entity
{
public:
	Attribute& Thickness();  // 39 Thickness (optional; default = 0)

	Attribute& StartPoint(); // 10 Start point (in WCS)

	Attribute& EndPoint();   // 11 Endpoint (in WCS)
};

class Point : public Entity
{
public:
	Attribute& Location();   // 10 Point location (in WCS)

	Attribute& Thickness();  // 39 Thickness (optional; default = 0)
};

class Circle : public Entity
{
public:
	Attribute& Thickness();  // 39 Thickness (optional; default = 0)

	Attribute& Center();     // 10 Center point (in OCS)

	Attribute& Radius();     // 40 Radius
};

class Arc : public Circle
{
public:
	Attribute& StartAngle(); // 50 Start angle

	Attribute& EndAngle();   // 51 End angle
};

class Ellipse : public Entity
{
public:
	Attribute& Center();   // 10 Center point (in WCS)

	Attribute& MajorEnd(); // 11 Endpoint of major axis, relative to the center (in WCS)

	Attribute& Ratio();    // 40 Ratio of minor axis to major axis

	Attribute& Start();    // 41 Start parameter (this value is 0.0 for a full ellipse)

	Attribute& End();      // 42 End parameter (this value is 2pi for a full ellipse)
};

class Database : public Node
{
	std::string _xyz[10][3];
	std::stack<Node*> _stack;
    
public:
	Nodes Sections;

	Nodes::iterator FindSection(const char *name);

	Entities GetEntities() const;

    void Load(const char *fileName);

	void Save(const char *fileName);
};

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
				_stack.top()->Attributes.insert(attributes_t::value_type(code,
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
					_stack.top()->Attributes.insert(attributes_t::value_type(loDigit,
						Attribute(loDigit, _xyz[loDigit])));
			}
			else
				// scalar param
				_stack.top()->Attributes.insert(
					attributes_t::value_type(code, Attribute(code, str)));
		}
	}
}

Attribute::Attribute(unsigned c, std::string value)
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

Attribute::Attribute(int rem, string value[3])
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

Constructor& Constructor::operator <<(const Attribute &attr)
{
	_stack.top()->Attributes.insert(attributes_t::value_type(attr.GetCode(), attr));
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
		unsigned code = atoi(buf);

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
}
