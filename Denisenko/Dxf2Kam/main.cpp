#include "stdafx.h"
#include "Dxf2Kam.h"

using namespace std;
using namespace Denisenko;
using namespace Denisenko::Dxf;
using namespace Denisenko::Dxf2Kam;
using namespace Denisenko::Kamea;

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		cout << "Not enough parameters" << endl;
		cout << "use: Dxf2Kam [dxfFile [kamFile]]" << endl;
		return 0;
	}
	string out;
	if (argc < 3)
	{
		out = argv[1];
		size_t pos = out.find_last_of('.');
		out = string(out.begin(), out.begin() + pos);
		out += ".kam";
	}
	else
		out = argv[2];

	Kamea::Program prog;
	prog.LoadKam("test2.kam");

	Database dxf;
	try
	{
		dxf.Load(argv[1]);
		for (Nodes::iterator i = dxf.Children.begin(); i != dxf.Children.end(); i++)
		{
			for (Attributes::iterator j = (*i)->Attributes.begin(); j != (*i)->Attributes.end(); j++)
				cout << (*j)->GetValue() << ' ';
			cout << endl;
			for (Nodes::iterator j = (*i)->Children.begin(); j != (*i)->Children.end(); j++)
				cout << ' ' << (*(*j)->Attributes.begin())->GetValue() << endl;
		}
		/*Convertor conv;
		Program prog = conv.Convert(argv[1], out);
		if (string(out).substr(strlen(out) - 3, 3) == "kam")
			prog.SaveKam(out);
		else
			prog.SaveTxt(out);*/
	}
	catch (Dxf::ParseError &ex)
	{
		cout << "Dxf parsing error " << ex.GetMessage() << endl;
		return -1;
	}
	/*catch (Kamea::SaveError &ex)
	{
		cout << "Saving error " << ex.GetMessage() << endl;
		return -1;
	}*/
	cout << "Done" << endl;
	int x;
	cin >> x;
	return 0;
}