// To compile: g++ ServerCall.cpp ServerCallTest.cpp -static-libstdc++ -o Test.exe

#include "ServerCall.h"


string placeToString(Place p)
{
	return p.name + ", " + to_string(p.latitude) + ", " + to_string(p.longitude) + ", " + p.type;
}

void printStringVector(vector<string> v)
{
	vector<string>::iterator i, end = v.end();
	for(i = v.begin(); i != end; i++)
	{
		cout << *i << " ";
	}
	cout << endl;
}

void printPlaceVector(vector<Place> v)
{
	vector<Place>::iterator i, end = v.end();
	for(i = v.begin(); i != end; i++)
	{
		cout << placeToString(*i) << " ";
	}
	cout << endl;
}

int main()
{
	ServerCall test = ServerCall();

	// Should return all places returned by the database
	cout << "Test Return All Locations:" << endl;
	vector<Place> poi = test.SearchByCoordinate(1.0,1.0,1.0,7,"");
	for(int i = 0; i < poi.size(); i++)
	{
		string str = placeToString(poi[i]);
		cout << str << endl;
	}

	// Should only return places that contain specific types
	cout << endl << "Filter Test 1: type8" << endl;
	poi = test.SearchByCoordinate(1.0,1.0,1.0,4,"type8");

	for(int i = 0; i < poi.size(); i++)
	{
		string str = placeToString(poi[i]);
		cout << str << endl;
	}

	cout << endl << "Filter Test 2: type3,type4" << endl;
	poi = test.SearchByCoordinate(1.0,1.0,1.0,4,"type3,type4");

	for(int i = 0; i < poi.size(); i++)
	{
		string str = placeToString(poi[i]);
		cout << str << endl;
	}

	cout << endl << "Filter Test 3: type3,type4|type8" << endl;
	poi = test.SearchByCoordinate(1.0,1.0,1.0,6,"type3,type4|type8");

	for(int i = 0; i < poi.size(); i++)
	{
		string str = placeToString(poi[i]);
		cout << str << endl;
	}

	cout << endl << "Filter Test 4: type8|type4,type3" << endl;
	poi = test.SearchByCoordinate(1.0,1.0,1.0,6,"type8|type4,type3");

	for(int i = 0; i < poi.size(); i++)
	{
		string str = placeToString(poi[i]);
		cout << str << endl;
	}

	// Should only return one place
	cout << endl << "Number of Places Test 1: Return 1 Place" << endl;
	poi = test.SearchByCoordinate(1.0,1.0,1.0,1,"");

	for(int i = 0; i < poi.size(); i++)
	{
		string str = placeToString(poi[i]);
		cout << str << endl;
	}

	cout << endl << "Number of Places Test 2: Return 3 Places" << endl;
	poi = test.SearchByCoordinate(1.0,1.0,1.0,3,"");

	for(int i = 0; i < poi.size(); i++)
	{
		string str = placeToString(poi[i]);
		cout << str << endl;
	}
}