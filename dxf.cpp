// dxf.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "dxf.h"
#include "dxf2kam.h"
#include "file.h"
#include "arc.h"

using namespace std;
using namespace dxf;

using MyGeometryTools::grad2rad;

class invalid_format : public exception
{
};

class convert_error : public exception
{
public:
	convert_error(const char *cstr)
		: exception(cstr)
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

	class stream
	{
		std::istream &_stream;
		bool next_code_string;
		int code;
	public:
		stream(std::istream &str)
			:_stream(str), next_code_string(true)
		{
		}

		int get_code(void);

		string get_string(void);
		int get_int(void);
		float get_float(void);
	};

	class parser
	{
		stream stream;
		//database database;
		factory &factory;
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
		parser(istream &stream, dxf::factory &factory);
		void parse(void);
	};
}

int dxf::stream::get_code(void)
{
	if (!next_code_string)
		return code;
	char buf[1024];
	_stream.getline(buf, sizeof(buf), '\n');
	next_code_string = false;
	code = atoi(buf);
	return code;
}

string dxf::stream::get_string(void)
{
	assert(!next_code_string);
	char buf[1024];
	_stream.getline(buf, sizeof(buf), '\n');
	next_code_string = true;
	return string(buf);
}

float dxf::stream::get_float(void)
{
	assert(!next_code_string);
	char buf[1024];
	_stream.getline(buf, sizeof(buf), '\n');
	next_code_string = true;
	char *stopchar;
	float retval = float(strtod(buf, &stopchar));
	if (stopchar == 0)
		throw convert_error(buf);
	return retval;
}

int dxf::stream::get_int(void)
{
	assert(!next_code_string);
	char buf[1024];
	_stream.getline(buf, sizeof(buf), '\n');
	next_code_string = true;
	return atoi(buf);
}

dxf::parser::parser(istream &stream, dxf::factory &factory)
	: stream(stream), factory(factory)
{
}

void dxf::parser::unknown_entity(string entity_name)
{
	cout << "skip entity - " << entity_name << endl;
	int code;
	string string;
	while(1)
	{
		code = stream.get_code();
		string = stream.get_string();
		if (code == 0)
			break;
	}
}

void dxf::parser::unknown_section(string section_name)
{
	cout << "skip section - " << section_name << endl;
	int code;
	string string;
	while(1)
	{
		code = stream.get_code();
		string = stream.get_string();
		if (code == 0 && string == "ENDSEC")
			break;
	}
}

/*void dxf::parser::parseLayerTable()
{
}

class AcDbSymbolTableRecord {
public:
	static AcDbSymbolTableRecord* from_dxf(dxf::stream & stream)
	{
		int code = stream.get_code();
		if (code == 100)
		{
			if (stream.get_string() == "AcDbLayerTableRecord")
				return AcDbLayerTableRecord::from_dxf(stream);
		}
		else
			stream.get_string();
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

	static AcDbLayerTableRecord * from_dxf(dxf::stream &stream)
	{
		while (1)
		{
			AcDbLayerTableRecord *rec = new AcDbLayerTableRecord();
			int code = stream.get_code();
			if (code == 2)
				rec->name = stream.get_string();
			else if (code == 70)
				rec->flags = stream.get_int();
			else if (code == 62)
				rec->color = stream.get_int();
			else if (code == 6)
				rec->line_type_name = stream.get_string();
			else if (code == 290)
				rec->plot_flag = stream.get_int();
			else if (code == 370)
				rec->line_weight = stream.get_int();
			else if (code == 390)
				rec->hPlotStyleName = stream.get_int();
			else
				break;
		}
		return rec;
	}
};

class AcDbSymbolTable {
public:
	int max_number_entries;
	static AcDbSymbolTable from_dxf(dxf::stream &stream)
	{
		AcDbSymbolTable table;
		while (1)
		{
			int code = stream.get_code();
			if (code == 0)
				break;
			else if (code == 70)
				table.max_number_entries = stream.get_int();
			else if (code == 100)
			{
				if (stream.get_string() == "AcDbSymbolTable")
					AcDbSymbolTable::from_dxf(stream);
			}
			else
				stream.get_string();
		}
		return table;
	}
};

void dxf::parser::parseTable()
{
	string table_name;
	int handle;
	AcDbSymbolTable table;
	while (1)
	{
		int code = stream.get_code();
		if (code == 0)
		{
			if (stream.get_string() == "ENDTAB")
				break;
		}
		else if (code == 2)
			table_name = stream.get_string();
		else if (code == 5)
			handle = stream.get_int();
		else if (code == 100)
		{
			if (stream.get_string() == "AcDbSymbolTable")
				table = AcDbSymbolTable::from_dxf(stream);
			//break;
		}
		else
			stream.get_string();

	}
	//if (table_name == "LAYER")
	//	parseLayerTable();
}

void dxf::parser::parseTablesSec()
{
	while (1)
	{
		int code = stream.get_code();
		if (code == 0)
		{
			string string = stream.get_string();
			if (string == "TABLE")
				parseTable();
			else if (string == "ENDSEC")
				break;
		}
		else
			stream.get_string();
			//break;
	}
}*/

void dxf::parser::parseParams(int &par5, float &par10, float &par20, float &par30,
		   float &par11, float &par21, float &par31, float par39, float &par40, float &par41, float &par42,
		   float &par50, float &par51,
		   int &par62)
{
	while (1)
	{
		int code = stream.get_code();
		if (code == 0)
			break;
		else if (code == 5)
			par5 = stream.get_int();
		else if (code == 10)
			par10 = stream.get_float();
		else if (code == 20)
			par20 = stream.get_float();
		else if (code == 30)
			par30 = stream.get_float();
		else if (code == 11)
			par11 = stream.get_float();
		else if (code == 21)
			par21 = stream.get_float();
		else if (code == 31)
			par31 = stream.get_float();
		else if (code == 39)
			par39 = stream.get_float();
		else if (code == 40)
			par40 = stream.get_float();
		else if (code == 41)
			par41 = stream.get_float();
		else if (code == 42)
			par42 = stream.get_float();
		else if (code == 50)
			par50 = stream.get_float();
		else if (code == 51)
			par51 = stream.get_float();
		else if (code == 62)
			par62 = stream.get_int();
		else
			stream.get_string();	// skip
	}
}

void dxf::parser::parseEntitiesSec()
{
	while(1)
	{
		int code = stream.get_code();
		string string = stream.get_string();
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

void dxf::parser::parse()
{
	while (1)
	{
		int code = stream.get_code();
		string string = stream.get_string();
		if (string == "EOF")
			break;
		else if (string == "SECTION")
		{
			code = stream.get_code();
			string = stream.get_string();
			/*if (string == "TABLES")
				parseTablesSec();
			else*/ if (string == "ENTITIES")
				parseEntitiesSec();
			else
				unknown_section(string);
		}
	}
}

void dxf::parse(istream &stream, factory &factory)
{
	parser parser(stream, factory);
	parser.parse();
}

LRESULT CALLBACK proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;
	string str = "WWW";
	RECT rect = {0, 0, 100, 100};
	POINT pts[] = {{0,0},{100, 100},{100, 200},{200, 200}};
	switch (msg)
	{
	case WM_PAINT:

		BeginPaint(hwnd, &ps);
		DrawText(ps.hdc, str.c_str(), str.size(), &rect, DT_CENTER);
		PolyBezier(ps.hdc, pts, 4);
		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
}

void testing_font_antialiasing(void)
{
	const char *class_name = "myclass", *window_name = "My window";
	WNDCLASS wndcls = {0};
	wndcls.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wndcls.hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));
	wndcls.hInstance = GetModuleHandle(NULL);
	wndcls.lpfnWndProc = proc;
	wndcls.lpszClassName = class_name;
	RegisterClass(&wndcls);
	HWND hwnd = CreateWindow(class_name, window_name, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, GetModuleHandle(NULL), NULL);
	CreateWindow("Button", "&да", WS_CHILDWINDOW | WS_VISIBLE | BS_AUTORADIOBUTTON, 10, 200, 100, 30, hwnd, 0, 0, 0);
	ShowWindow(hwnd, SW_SHOW);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		DispatchMessage(&msg);
	}
}

void test1(void)
{
	//ifstream str("D:\\Work\\source\\SEMIL\\DXF2KAM\\circle.dxf");
	ifstream str("c:\\Данила-Мастер\\drawing1.dxf");
	if (!str)
		throw exception("not opened");
	Kamea::program prog(dxf2kam::convert(str));
	ofstream ostr("circle.kam");
	if (!ostr)
		throw exception("not opened");
	Kamea::save(ostr, prog);
	//parse(str, factory);
}

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		testing_font_antialiasing();
	}
	catch (exception &ex)
	{
		cout << ex.what();
	}
	return 0;
}

