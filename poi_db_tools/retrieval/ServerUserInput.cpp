// To compile on AWS: sudo g++ -std=c++0x -Wall -I/usr/include/cppconn -o TestUserInput ServerCall.cpp ServerUserInput.cpp -L/usr/lib -lmysqlcppconn

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

// Split by delimiter function obtained from: https://stackoverflow.com/questions/236129/the-most-elegant-way-to-iterate-the-words-of-a-string
template<typename Out>
void split2(const string &s, char delim, Out result) {
    stringstream ss(s);
    string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

vector<string> split2(const string &s, char delim) {
    vector<string> elems;
    split2(s, delim, back_inserter(elems));
    return elems;
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
		cin >> userInputString;
		if(userInputString == "P")
		{
			cout << "Enter the Latitude,Longitude of your coordinate:" << endl;
			cin >> userInputString;
			splitString = split2(userInputString,',');
			iLat = stod(splitString[0]);
			iLong = stod(splitString[1]);

			cout << "Enter the range of your search:" << endl;
			cin >> userInputString;
			sRange = stod(userInputString);

			cout << "Enter the number of POIs desired:" << endl;
			cin >> userInputString;
			numPOIs = stoi(userInputString);

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
			cout << "Enter the initial \"Latitude,Longitude\" of your coordinate:" << endl;
			cin >> userInputString;
			splitString = split2(userInputString,',');
			iLat = stod(splitString[0]);
			iLong = stod(splitString[1]);

			cout << "Enter the end \"Latitude,Longitude\" of your coordinate:" << endl;
			cin >> userInputString;
			splitString = split2(userInputString,',');
			eLat = stod(splitString[0]);
			eLong = stod(splitString[1]);

			cout << "Enter the range of your search:" << endl;
			cin >> userInputString;
			sRange = stod(userInputString);

			cout << "Enter the number of POIs desired:" << endl;
			cin >> userInputString;
			numPOIs = stoi(userInputString);

			cout << "Enter filter string (for no filter, type \'.\')" << endl;
			cin >> userInputString;
			if(userInputString == ".")
				userInputString = "";

			poi = sc.SearchByLine(iLat,iLong,eLat,eLong,sRange,numPOIs,userInputString);

			cout << endl << "Here are your results:" << endl;
			printPlaceVector(poi);
		}
		else
		{
			cout << "Invalid input. Please try again." << endl;
		}

	}
}