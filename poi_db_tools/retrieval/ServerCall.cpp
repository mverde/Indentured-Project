// ServerCall.cpp
// Overview: ServerCall.cpp will be used to connect to the AWS server and retrieve desired points of interests stored in the database

#include "ServerCall.h"
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <math.h>

using namespace std;

// Constants used to convert degrees to meters
const int EARTH_RADIUS_METERS = 6371000;
const double PI = 3.14159265358979;
const double ONE_EIGHTY_DIV_PI = 180.0 / PI;
const double PI_DIV_ONE_EIGHTY = PI / 180.0;


// Algorithm borrowed from our Python script creators

// The following two functions will convert meters to latitude/longitude
double metersToLat(double meters)
{
	return (meters / EARTH_RADIUS_METERS) * ONE_EIGHTY_DIV_PI;
}

double metersToLong(double latitude, double meters)
{
	return (meters / EARTH_RADIUS_METERS) * ONE_EIGHTY_DIV_PI / cos(latitude * PI_DIV_ONE_EIGHTY);
}

// The following function will add two degree values
double addDegrees(double d1, double d2)
{
	double newDegree = d1 + d2;

	if (newDegree > 180.0)
        return -180.0 + (newDegree - 180.0);
    else if (newDegree < -180.0)
        return 180.0 + (newDegree + 180.0);
    
    return newDegree;
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



// The constructor will set up a connection with the AWS server to allow for database querying
ServerCall::ServerCall(int options)
{
	// Initialize srand for randomly selecting locations when requested
	srand(time(NULL));

	// Set up the connection to the sql database
	
	try {
		// Create a connection 
		/////////////////////////
		driver = get_driver_instance();
		//set the variables for connection to the database : endpoint, username, password
		con = driver->connect("escality-db-instance.cykpeyjjej2m.us-west-1.rds.amazonaws.com", "escality_user", "12345678");
		// Connect to the MySQL test database 
		con->setSchema("escality_location_db");

	} catch (sql::SQLException &e) {
	  cout << "# ERR: SQLException in " << __FILE__;
	  cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
	  cout << "# ERR: " << e.what();
	  cout << " (MySQL error code: " << e.getErrorCode();
	  cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	} 
	
}

vector<string> dummyServerResults()
{
	vector<string> serverResult;
	serverResult.push_back("Place1,10.0,100.0,type1,type2,type8");
	serverResult.push_back("Place2,12.0,102.0,type1,type3,type4,type5,type6");
	serverResult.push_back("Place3,14.0,124.0,type1,type7,type9");
	serverResult.push_back("Place4,16.0,126.0,type6,type3,type10,type8,type2");
	serverResult.push_back("Place5,18.0,128.0,type9,type4,type3");
	serverResult.push_back("Place6,20.0,130.0,type1,type6,type8");
	serverResult.push_back("Place7,22.0,132.0,type12,type3,type4,type8,type10");

	return serverResult;
}

// This function will get the locations in a square around the specified coordinate
vector<Place> ServerCall::queryDatabase(double latitude, double longitude, double maxRange)
{
	// First need to get the longitude and latitude range for the search
	double deltaLat = metersToLat(maxRange);
	double lowLat = addDegrees(latitude, -deltaLat);
	double highLat = addDegrees(latitude, deltaLat);

	double deltaLong = metersToLong(latitude, maxRange);
	double lowLong = addDegrees(longitude, -deltaLong);
	double highLong = addDegrees(longitude, deltaLong);

	// Create the rds query string
	string query = "select * from pois where lat > " + to_string(lowLat) + 
					" and lat < " +	to_string(highLat) + 
					" and lng > " +	to_string(lowLong) + 
					" and lng < " +	to_string(highLong);

	//cout << query << endl;
	// serverResult will contain strings that correspond to rows from the point of interest tables in the AWS server
	vector<string> serverResult;

	// Run the query on the server
	sql::PreparedStatement *prep_stmt;
	sql::ResultSet *res;
	prep_stmt = con->prepareStatement(query);
	res = prep_stmt->executeQuery();

	// For every server result, turn it into a comma separated string and add it to the serverResult vector
	while (res->next()) {
		//for each tuple result, we use getString("attribute") to retrieve the data corrosponding to the attribute name
		string currResult = res->getString("Place") + "," + 
							res->getString("Lat") + "," + 
							res->getString("Lng") + "," + 
							res->getString("Types");

		serverResult.push_back(currResult);
		// cout << res->getString("Place") << "," << res->getString("Lat") << "," << res->getString("Lng") << "," << res->getString("Types") << endl;
	}

	// clean up
	delete res;
	delete prep_stmt;

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

	// For each place in places
	vector<Place>::iterator placesIterator, placesEnd = places.end();
	for(placesIterator = places.begin(); placesIterator != placesEnd; placesIterator++)
	{
		string currPlaceTypes = (*placesIterator).type;

		// Go through each OR statement until one is satisfied
		vector<vector<string>>::iterator orStatementIterator, orStatementEnd = filterVector.end();
		for(orStatementIterator = filterVector.begin(); orStatementIterator != orStatementEnd; orStatementIterator++)
		{
			passesFilters = true;
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
				//cout << "PASSED FILTER" << endl;
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
