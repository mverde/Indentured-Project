// ServerCall.cpp
// Overview: ServerCall.cpp will be used to connect to the AWS server and retrieve desired points of interests stored in the database

// To compile: g++ ServerCall.cpp -static-libstdc++ -o executableName.exe

#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <iostream>
#include <ctime>
#include <algorithm>

using namespace std;

// A structure to define a point of interest
struct Place
{
	string name;
	double latitude;
	double longitude;
	string type;
};

string placeToString(Place p)
{
	return p.name + ", " + to_string(p.latitude) + ", " + to_string(p.longitude) + ", " + p.type;
}

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
public:
	// Constructor; Parameters not specified yet
	ServerCall(int options);

	// This function will find all locations within a square of side length 2*maxRange centered around the specified coordinate
	vector<Place> queryDatabase(double latitude, double longitude, double maxRange);

	// This funtion will return at most numPlaces within a square of side length 2*maxRange centered around the specified coordinate
	// And will return results that follow the filtering rules, which are to be specified in CNF format:
	// Filter array: [[filter1,filter2,filter3][filter4][filter5,filter6]] will return places that has 
	// [filter1 OR filter2 OR filter3] AND [filter4] AND [filter5 OR filter6]
	vector<Place> SearchByCoordinate(double latitude, double longitude, double maxRange, int numPlaces, string filters);
	
	// SearchByLine will find numPlaces that are in a line between the initial coordinate and the end coordinate
	// MaxRange will specify how far away from the line a place can be
	// Additionally, only places that satisfy the filters will be accepted in the output
	// This function will return a vector of Places that meet the requirements, which are specified in CNF format
	vector<Place> SearchByLine(double initLatitude, double initLongitude, double endLatitude, double endLongitude, double maxRange, int numPlaces, string filters);
};


// The constructor will set up a connection with the AWS server to allow for database querying
ServerCall::ServerCall(int options)
{
	// To be implemented once the AWS is running
	srand(time(NULL));
}

vector<string> dummyServerResults()
{
	vector<string> serverResult;
	serverResult.push_back("Los Angeles,34.05223420,-118.24368490,locality,political");
	serverResult.push_back("Los Angeles City Hall,34.05352670,-118.24293160,city_hall,premise,local_government_office,point_of_interest,establishment");

	return serverResult;
}

// This function will get the locations in a square around the specified coordinate
vector<Place> ServerCall::queryDatabase(double latitude, double longitude, double maxRange)
{
	// serverResult will contain strings that correspond to rows from the point of interest tables in the AWS server

	// We will assume that serverResult has meaningful items for now, since the server is not set up yet
	// For now, we insert dummy test data to serverResult
	vector<string> serverResult = dummyServerResults();


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

// This function will filter a vector of Places based on the filters string
// The format for the filter string is as follows:
// "type1,type2,type3|type4|type5,type6"
// This filter string makes it so that the Places returned by this function will have
// (type1, type2, and type 3) OR type 4 OR (type 5 and type 6) in the Place's type string
vector<Place> filterPlaces(vector<Place> places, string filters)
{
	vector<Place> outputVector;

	// Split the filter into the OR filter types, where each row in the vector is a string of AND filter types
	vector<string> filterGroupVector = split(filters, '|');
	vector<vector<string>> filterVector;

	// Now we make a vector, where each row in that vector is a string vector where each string is an individual filter type
	vector<string>::iterator filterGroupIterator, filterGroupEnd = filterGroupVector.end();
	for(filterGroupIterator = filterGroupVector.begin(); filterGroupIterator != filterGroupEnd; filterGroupIterator++)
	{
		filterVector.push_back(split(*filterGroupIterator, ','));
	}

	bool passesFilters;	
	// Now we use the vector of vectors to filter the input places
	vector<Place>::iterator placesIterator, placesEnd = places.end();
	for(placesIterator = places.begin(); placesIterator != placesEnd; placesIterator++)
	{
		passesFilters = true;
		string currPlaceTypes = (*placesIterator).type;

		// Go through each OR statement until one is satisfied
		vector<vector<string>>::iterator orStatementIterator, orStatementEnd = filterVector.end();
		for(orStatementIterator = filterVector.begin(); orStatementIterator != orStatementEnd; orStatementIterator++)
		{
			// Go through each AND statement
			vector<string> currAndStatement = (*orStatementIterator);
			vector<string>::iterator andStatementIterator, andStatementEnd = currAndStatement.end();
			for(andStatementIterator = currAndStatement.begin(); andStatementIterator != andStatementEnd; andStatementIterator++)
			{
				// If it fails one of these filters, then break out of this loop and go to the next set of AND statements
				if(currPlaceTypes.find(*andStatementIterator) == string::npos)
				{
					passesFilters = false;
					break;
				}
			}

			// If it passed all of the filters in a set of AND statements, then add this place to our output vector and stop going through OR statements
			if(passesFilters)
			{
				outputVector.push_back(*placesIterator);
				break;
			}
			// Otherwise, check if the current Place satisfies the next OR statement
		}
	}

	return outputVector;
}

// This function will select numPlaces for a Place vector randomly
vector<Place> randomSelectPlaces(vector<Place> places, int numPlaces)
{
	int selectedPlaceIndices [numPlaces];
	
	int vectorSize = places.size();
	// If we request more places than exist in our input vector, simply return the input vector
	if(numPlaces >= vectorSize)
		return places;

	// Fill with -1 to avoid issues when checking if an index already exists in the array
	for(int i = 0; i < numPlaces; i++)
		selectedPlaceIndices[i] = -1;

	vector<Place> selectedPlaces;

	for(int i = 0; i < numPlaces; i++)
	{
		// Will select one of the indices of our place vector
		int currIndex = rand() % vectorSize;
		//cout << vectorSize << " " << currIndex << endl;

		// Check if this index was already chosen to be in our array
		bool indexExists = false;
		for(int j = 0; j < numPlaces; j++)
		{	
			// If the selected index is already in our array, set the bool and exit the loop
			if(selectedPlaceIndices[j] == currIndex)
			{
				indexExists = true;
				break;
			}
		}

		// If the index is new, add it to our array
		if (!indexExists)
			selectedPlaceIndices[i] = currIndex;
		else
		// Otherwise, try again
			i--;
	}

	for(int i = 0; i < numPlaces; i++)
		selectedPlaces.push_back(places[selectedPlaceIndices[i]]);

	return selectedPlaces;

}

// SearchByCoordinate will find at most numPlaces that are in maxRange radius from the specified coordinate
// Additionally, only places that satisfy the filters will be accepted in the output
// This function will return a vector of Places that meet the requirements
// If filters is empty (filters == ""), then filtering is not required
vector<Place> ServerCall::SearchByCoordinate(double latitude, double longitude, double maxRange, int numPlaces, string filters)
{
	// This is where we will store places that meet the required criteria
	vector<Place> outputPlaces;

	// If 0 or a negative number of places is requested, return an empty vector
	if(numPlaces < 1)
		return outputPlaces;

	vector<Place> dbPlaceVec = queryDatabase(latitude, longitude, maxRange);

	// Now we need to filter the results in dbPlaceVec based on the input filters

	// If a filter is specified, filter our Places
	if(filters != "")
		outputPlaces = filterPlaces(dbPlaceVec, filters);
	else
	//Otherwise, retain all Places
		outputPlaces = dbPlaceVec;

	// Out of the remaining places, select up to numPlaces to return
	outputPlaces = randomSelectPlaces(outputPlaces, numPlaces);

	return outputPlaces;
}

// SearchByLine will find numPlaces that are in a line between the initial coordinate and the end coordinate
// Additionally, only places that satisfy the filters will be accepted in the output
// This function will return a vector of Places that meet the requirements
vector<Place> ServerCall::SearchByLine(double initLatitude, double initLongitude, double endLatitude, double endLongitude, double maxRange, int numPlaces, string filters)
{
	vector<Place> outputPlaces;


	if(numPlaces < 1)
		return outputPlaces;
	// If only one place is requested, just return a place near the init coordinate
	else if(numPlaces == 1)
		return SearchByCoordinate(initLatitude, initLongitude, maxRange, numPlaces, filters);

	// Otherwise, we need to split the line to have numPlaces locations
	// Pretend latitude is a y value and longitude is an x value in a 2D coordinate plane
	double longitudeChange = endLongitude - initLongitude;
	double slope = (endLatitude - initLatitude) / longitudeChange;

	double currLat = initLatitude;
	double currLong = initLongitude;
	// Partition the longitude into (numPlaces - 1) since the initial coordinate will be used as the first location
	double longitudeStep = longitudeChange / (numPlaces - 1);
	double latitudeStep = longitudeStep * slope;

	for(int i = 0; i < numPlaces; i++)
	{
		// Find a location at the current coordinate
		vector<Place> currPlace = SearchByCoordinate(currLat, currLong, maxRange, 1, filters);
		outputPlaces.push_back(currPlace[0]);

		// Move to the next coordinate in the line
		currLong += longitudeStep;
		currLat += latitudeStep;
	}

	return outputPlaces;
}


int main()
{
	ServerCall test = ServerCall(1);
	vector<Place> poi = test.SearchByCoordinate(1.0,1.0,1.0,2,"");

	// Should return all places returned by the database
	cout << "Test 1:" << endl;
	for(int i = 0; i < poi.size(); i++)
	{
		string str = placeToString(poi[i]);
		cout << str << endl;
	}

	// Should only return places that contain "political" as one of its types
	cout << endl << "Test 2:" << endl;
	poi = test.SearchByCoordinate(1.0,1.0,1.0,1,"political");

	for(int i = 0; i < poi.size(); i++)
	{
		string str = placeToString(poi[i]);
		cout << str << endl;
	}

	// Should only return one place
	cout << endl << "Test 3:" << endl;
	poi = test.SearchByCoordinate(1.0,1.0,1.0,1,"");

	for(int i = 0; i < poi.size(); i++)
	{
		string str = placeToString(poi[i]);
		cout << str << endl;
	}

	// Should return random locations from the database
	cout << endl << "Test 4:" << endl;
	poi = test.SearchByCoordinate(1.0,1.0,1.0,2,"");
	for(int i = 0; i < 10; i++)
	{
		vector<Place> curr = randomSelectPlaces(poi, 1);
		cout << placeToString(curr[0]) << endl;
	}

	// Should return 2 places
	cout << endl << "Test 5:" << endl;
	poi = test.SearchByLine(1.0,1.0,1.0,1.0,1.0,2,"");

	for(int i = 0; i < poi.size(); i++)
	{
		string str = placeToString(poi[i]);
		cout << str << endl;
	}


}