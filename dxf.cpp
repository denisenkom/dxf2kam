// dxf.cpp : Defines the entry point for the console application.
//
#include <stdexcept>
#include <iostream>
#include <cassert>
#include <cstdlib>
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
