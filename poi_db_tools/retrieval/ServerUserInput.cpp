// To compile on AWS: sudo g++ -std=c++0x -Wall -I/usr/include/cppconn -o UserInput ServerCall.cpp ServerUserInput.cpp -L/usr/lib -lmysqlcppconn

#include "ServerCall.h"

string placeToString(Place p)
{
	return "Name: " + p.name + "\nLatitude, Longitude: " + to_string(p.latitude) + ", " + to_string(p.longitude) + "\nTypes list: " + p.type;
}

void printPlaceVector(vector<Place> v)
{
	vector<Place>::iterator i, end = v.end();
	for(i = v.begin(); i != end; i++)
	{
		cout << placeToString(*i) << endl << endl;
	}
}


// Copy pasted from https://stackoverflow.com/questions/17750872/validating-a-double-in-c
double getDouble () {
    double x;
    cin >> x;
    // ver1: while( cin.fail() ) // or !cin.good()   no trailing char check.
    while( cin.fail() || (cin.peek() != '\r' && cin.peek() != '\n'))
    {
        cout << "Invalid Input! Please input a numerical value." << endl;
        cin.clear();
        while( cin.get() != '\n' ); // or cin.ignore(1000, '\n');
        cin >> x;
    }
    return x;
}

int getInt () {
    int x;
    cin >> x;
    // ver1: while( cin.fail() ) // or !cin.good()   no trailing char check.
    while( cin.fail() || (cin.peek() != '\r' && cin.peek() != '\n'))
    {
        cout << "Invalid Input! Please input an integer." << endl;
        cin.clear();
        while( cin.get() != '\n' ); // or cin.ignore(1000, '\n');
        cin >> x;
    }
    return x;
}

int main()
{
	ServerCall sc = ServerCall();
	vector<Place> poi;
	string userInputString;
	double iLat, iLong, eLat, eLong, sRange;
	int numPOIs;
	vector<string> splitString;

	while(true)
	{
		poi.clear();
		cout << endl << "Please select your search type:" << endl;
		cout << "\tFor point search, type \'P\'" << endl;
		cout << "\tFor line search, type \'L\'" << endl;
		cout << "\tTo quit, type \'Q\'" << endl;
		cin >> userInputString;
		if(userInputString == "P")
		{
			cout << "Enter the Latitude of your coordinate:" << endl;
			iLat = getDouble();

			cout << "Enter the Longitude of your coordinate:" << endl;
			iLong = getDouble();

			cout << "Enter the range of your search:" << endl;
			sRange = getDouble();

			cout << "Enter the number of POIs desired:" << endl;
			numPOIs = getInt();

			cout << "Enter filter string (for no filter, type \'.\'):" << endl;
			cin >> userInputString;
			if(userInputString == ".")
				userInputString = "";

			poi = sc.SearchByCoordinate(iLat,iLong,sRange,numPOIs,userInputString);

			cout << endl << "Here are your results:" << endl;
			printPlaceVector(poi);

		}
		else if (userInputString == "L")
		{
			cout << "Enter the latitude of your initial coordinate:" << endl;
			iLat = getDouble();

			cout << "Enter the longitude of your initial coordinate:" << endl;
			iLong = getDouble();

			cout << "Enter the latitude of your end coordinate:" << endl;
			eLat = getDouble();

			cout << "Enter the longitude of your end coordinate:" << endl;
			eLong = getDouble();

			cout << "Enter the range of your search:" << endl;
			sRange = getDouble();

			cout << "Enter the number of POIs desired:" << endl;
			numPOIs = getInt();

			cout << "Enter filter string (for no filter, type \'.\')" << endl;
			cin >> userInputString;
			if(userInputString == ".")
				userInputString = "";

			poi = sc.SearchByLine(iLat,iLong,eLat,eLong,sRange,numPOIs,userInputString);

			cout << endl << "Here are your results:" << endl;
			printPlaceVector(poi);
		}
		else if (userInputString == "Q")
		{
			break;
		}
		else
		{
			cout << "Invalid input. Please try again." << endl;
		}

	}
}