// To compile locally: g++ ServerCall.cpp ServerCallTest.cpp -static-libstdc++ -o Test.exe
// To compile on AWS: sudo g++ -std=c++0x -Wall -I/usr/include/cppconn -o TestCall ServerCall.cpp ServerCallTest.cpp -L/usr/lib -lmysqlcppconn

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
		cout << placeToString(*i) << endl;
	}
	cout << endl;
}

void runRandomSelectionTests(ServerCall sc, bool printOutputs = true)
{
	cout << "Beginning Random Selection Tests:" << endl;
	vector<Place> poi;

	poi.clear();
	poi = sc.SearchByCoordinate(34.0507,-118.24,500,15,"");

	
	cout << "Dataset used for random selection tests:" << endl;
	if(printOutputs)
		printPlaceVector(poi);
	

	cout << "Select 3 random locations (5 times):" << endl;
	if(printOutputs)
	{
		for(int i = 0; i < 5; i++)
		{
			cout << "Selection " << i << ":" << endl;
			printPlaceVector(sc.randomSelectPlaces(poi, 3));
		}
	}
}

Place makePlace(string n, double lat, double lng, string t)
{
	Place newPlace;
	newPlace.name = n;
	newPlace.latitude = lat;
	newPlace.longitude = lng;
	newPlace.type = t;

	return newPlace;
}

void runFilterTests(ServerCall sc, bool printOutputs = true)
{
	vector<Place> dummyPlaces;
	dummyPlaces.push_back(makePlace("Place1",10.0,100.0,"type1,type2,type8"));
	dummyPlaces.push_back(makePlace("Place2",12.0,102.0,"type1,type3,type4,type5,type6"));
	dummyPlaces.push_back(makePlace("Place3",14.0,124.0,"type1,type7,type9"));
	dummyPlaces.push_back(makePlace("Place4",16.0,126.0,"type6,type3,type10,type8,type2"));
	dummyPlaces.push_back(makePlace("Place5",18.0,128.0,"type9,type4,type3"));
	dummyPlaces.push_back(makePlace("Place6",20.0,130.0,"type1,type6,type8"));
	dummyPlaces.push_back(makePlace("Place7",22.0,132.0,"type12,type3,type4,type8,type10"));

	cout << "Beginning Filtering Tests:" << endl;
	cout << "Data set used for filtering tests:" << endl;
	if(printOutputs)
		printPlaceVector(dummyPlaces);

	vector<Place> poi;

	cout << "Return Places of type2:" << endl;
	poi.clear();
	poi = sc.filterPlaces(dummyPlaces,"type2");
	if(printOutputs)
		printPlaceVector(poi);

	cout << "Return Places of type2|type8:" << endl;
	poi.clear();
	poi = sc.filterPlaces(dummyPlaces,"type2|type8");
	if(printOutputs)
		printPlaceVector(poi);

	cout << "Return Places of type2,type8:" << endl;
	poi.clear();
	poi = sc.filterPlaces(dummyPlaces,"type2,type10");
	if(printOutputs)
		printPlaceVector(poi);

	cout << "Return Places of type50: (should be none)" << endl;
	poi.clear();
	poi = sc.filterPlaces(dummyPlaces,"type50");
	if(printOutputs)
		printPlaceVector(poi);
}

void runCoordSearchTests(ServerCall sc, bool printOutputs = true)
{
	cout << "Beginning Coordinate Search Tests:" << endl;
	vector<Place> poi;

	cout << "Return JiST cafe: " << endl;
	poi.clear();
	poi = sc.SearchByCoordinate(34.05077310,-118.24037730,1,1,"cafe,restaurant,food,point_of_interest,establishment");
	if(printOutputs)
		printPlaceVector(poi);

	cout << "Return 5 Locations:" << endl;
	poi.clear();
	poi = sc.SearchByCoordinate(34.05,-118.24,20,5,"");
	if(printOutputs)
		printPlaceVector(poi);

	cout << "Filter for 5 food places:" << endl;
	poi.clear();
	poi = sc.SearchByCoordinate(34.05,-118.24,20,5,"food");
	if(printOutputs)
		printPlaceVector(poi);
	
	cout << "Filter for 10 food or parking:" << endl;
	poi.clear();
	poi = sc.SearchByCoordinate(34.05,-118.24,20,10,"food|parking");
	if(printOutputs)
		printPlaceVector(poi);

	cout << "Filter for 10 food and cafe:" << endl;
	poi.clear();
	poi = sc.SearchByCoordinate(34.05,-118.24,20,10,"food,cafe");
	if(printOutputs)
		printPlaceVector(poi);

	//return "";
}

void runLineSearchTests(ServerCall sc, bool printOutputs = true)
{
	cout << "Beginning Line Search Tests:" << endl;
	vector<Place> poi;

	cout << "Return Line of 5 Locations:" << endl;
	poi.clear();
	poi = sc.SearchByLine(34.05073240,-118.24543790,34.05648110,-118.24060620,50,5,"");
	if(printOutputs)
		printPlaceVector(poi);

	cout << "Create Line Where End Coord is Far Away:" << endl;
	poi.clear();
	poi = sc.SearchByLine(34.05073240,-118.24543790,4.05648110,-18.24060620,500,5,"");
	if(printOutputs)
		printPlaceVector(poi);

	cout << "Create Line Where Beginning Coord is Far Away:" << endl;
	poi.clear();
	poi = sc.SearchByLine(4.05073240,-18.24543790,34.05648110,-118.24060620,500,5,"");
	if(printOutputs)
		printPlaceVector(poi);
}

int main()
{
	ServerCall sc = ServerCall();
	cout << endl << "Beginning of Tests:" << endl << endl;

	runRandomSelectionTests(sc);
	runFilterTests(sc);
	runCoordSearchTests(sc);
	runLineSearchTests(sc);

	// Test results used for testing with the dummy server results
	/*
	// Should return all places returned by the database
	cout << "Test Return All Locations:" << endl;
	vector<Place> poi = test.SearchByCoordinate(1.0,1.0,0,7,"");
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
	*/
}