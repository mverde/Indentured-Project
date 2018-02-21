// ServerCall.cpp
// Overview: ServerCall.cpp will be used to connect to the AWS server and retrieve desired points of interests stored in the database

#include <string>
#include <sstream>
#include <vector>
#include <iterator>

using namespace std;

// A structure to define a point of interest
struct Place
{
	string name;
	double latitude;
	double longitude;
	string type;
};

// Split by delimiter function obtained from: https://stackoverflow.com/questions/236129/the-most-elegant-way-to-iterate-the-words-of-a-string

template<typename Out>
void split(const string &s, char delim, Out result) {
    stringstream ss(s);
    string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, back_inserter(elems));
    return elems;
}

// This class handles server connections and the querying of the server database
class ServerCall
{
	// Constructor; Parameters not specified yet
	ServerCall(int options = 0);

	// This function will find all locations within a square of side length 2*maxRange centered around the specified coordinate
	vector<Place> queryDatabase(double latitude, double longitude, double maxRange);

	// This funtion will return at most numPlaces within a square of side length 2*maxRange centered around the specified coordinate
	// And will return results that follow the filtering rules, which are to be specified in CNF format:
	// Filter array: [[filter1,filter2,filter3][filter4][filter5,filter6]] will return places that has 
	// [filter1 OR filter2 OR filter3] AND [filter4] AND [filter5 OR filter6]
	vector<Place> SearchByCoordinate(double latitude, double longitude, double maxRange, int numPlaces, string filters);
	
	// SearchByLine will find numPlaces that are in a line between the initial coordinate and the end coordinate
	// Additionally, only places that satisfy the filters will be accepted in the output
	// This function will return a vector of Places that meet the requirements, which are specified in CNF format
	vector<Place> SearchByLine(double initLatitude, double initLongitude, double endLatitude, double endLongitude, int numPlaces, string filters);
};

// The constructor will set up a connection with the AWS server to allow for database querying
ServerCall::ServerCall(int options)
{
	// To be implemented once the AWS is running
}

// This function will get the locations in a square around the specified coordinate
vector<Place> ServerCall::queryDatabase(double latitude, double longitude, double maxRange)
{
	// serverResult will contain strings that correspond to rows from the point of interest tables in the AWS server
	vector<string> serverResult;

	// We will assume that serverResult has meaningful items for now, since the server is not set up yet
	// For now, we insert dummy test data to serverResult
	serverResult.push_back("Los Angeles,34.05223420,-118.24368490,locality,political");
	serverResult.push_back("Los Angeles City Hall,34.05352670,-118.24293160,city_hall,premise,local_government_office,point_of_interest,establishment");

	// dbPlaceVec will hold Places that were returned by the database query (filtering will happen later)
	vector<Place> dbPlaceVec;

	// Iterate through every string in the database results
	// This assumes that each string in serverResults follows the following format:
	// "Place name,Latitude,Longitude,Type1,Type2,Type3,..."

	vector<string>::iterator resultIterator, resultEnd = serverResult.end();
	for(resultIterator = serverResult.begin(); resultIterator != resultEnd; ++resultIterator)
	{
		// Create a new Place
		Place tempPlace;
		tempPlace.type = "";

		// Split the string by using the comma as a delimiter
		vector<string> splitString = split(*resultIterator, ',');

		// Go through the split string and fill out the tempPlace attributes
		int i = 0;
		vector<string>::iterator stringIterator, stringEnd = splitString.end();
		for(stringIterator = splitString.begin(); stringIterator != stringEnd; stringIterator++, i++)
		{
			if(i == 0)
				tempPlace.name = *stringIterator;
			else if (i == 1)
				tempPlace.latitude = stod(*stringIterator);
			else if (i == 2)
				tempPlace.longitude = stod(*stringIterator);
			else if (tempPlace.type == "")
				tempPlace.type = *stringIterator;
			else
				tempPlace.type += "," + *stringIterator;
		}

		// Add the Place to the vector
		dbPlaceVec.push_back(tempPlace);
	}

	return dbPlaceVec;
}

// SearchByCoordinate will find at most numPlaces that are in maxRange radius from the specified coordinate
// Additionally, only places that satisfy the filters will be accepted in the output
// This function will return a vector of Places that meet the requirements
vector<Place> ServerCall::SearchByCoordinate(double latitude, double longitude, double maxRange, int numPlaces, string filters)
{
	// This is where we will store places that meet the required criteria
	vector<Place> outputPlaces;

	vector<Place> dbPlaceVec = queryDatabase(latitude, longitude, maxRange);

	// Now we need to filter the results in dbPlaceVec based on the input filters


	return outputPlaces;
}

// SearchByLine will find numPlaces that are in a line between the initial coordinate and the end coordinate
// Additionally, only places that satisfy the filters will be accepted in the output
// This function will return a vector of Places that meet the requirements
vector<Place> ServerCall::SearchByLine(double initLatitude, double initLongitude, double endLatitude, double endLongitude, int numPlaces, string filters)
{
	vector<Place> outputPlaces;

	// To be implemented

	return outputPlaces;
}

int main()
{

}