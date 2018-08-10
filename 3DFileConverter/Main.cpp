#include <iostream>
#include <string>
#include <time.h>
#include <vector>
#include "3DData.h"
#include "ObjLoader.h"
#include "StlWriter.h"
using namespace std;
using namespace FormatConverter;

int main()
{
	cout << "Testing converter with an obj of a cube made from rectangles" << endl;
	try
	{
		clock_t t1, t2, t3;
		t1 = clock();
		CheckedObjLoader ol = CheckedObjLoader();
		D3Data inmem = ol.load(std::string("cube.obj"),true,false,true);
		cout << "Face count: " << inmem.faces.size() << endl;
		t2 = clock();
		float diff((float)t2 - (float)t1);
		cout << "Loading time: " << diff / CLOCKS_PER_SEC << endl;

		cout << "Calculated statistics:" << endl;
		cout << "Surface area of base 2 cube: " << Utils::calculate_surface_area(inmem) << endl;
		cout << "Volume of base 2 cube: " << Utils::calculate_volume(inmem) << endl;

		StlWriter sw = StlWriter();
		sw.write(std::string("mycube.stl"), inmem);
		t3 = clock();
		cout << "Writing done." << endl;
		float diff2((float)t3 - (float)t2);
		cout << "Writing time: " << diff2 / CLOCKS_PER_SEC << endl;

	}
	catch (const std::exception& ex)
	{
		cout << ex.what() << endl;
	}

	//system("pause");
	return 0;
}