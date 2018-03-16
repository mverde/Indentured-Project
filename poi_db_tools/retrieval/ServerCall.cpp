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

// Conversion algorithms borrowed from our Python script creators
// The following functions will convert meters to latitude/longitude and vice versa
double metersToLat(double meters)
{
	return (meters / EARTH_RADIUS_METERS) * ONE_EIGHTY_DIV_PI;
}

double metersToLong(double latitude, double meters)
{
	return (meters / EARTH_RADIUS_METERS) * ONE_EIGHTY_DIV_PI / cos(latitude * PI_DIV_ONE_EIGHTY);
}

double latToMeters(double lat)
{
	return (lat * EARTH_RADIUS_METERS * PI_DIV_ONE_EIGHTY);
}

double longToMeters(double latitude, double longitude)
{
	return (longitude * EARTH_RADIUS_METERS * PI_DIV_ONE_EIGHTY * cos(latitude * PI_DIV_ONE_EIGHTY));
}

// The following function will add two degree values and adjust for degree overflow
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
ServerCall::ServerCall()
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

// This function will get the locations in a square (edge length 2 * maxRange) around the specified coordinate
vector<Place> ServerCall::queryDatabase(double latitude, double longitude, double maxRange)
{
	if(maxRange < 1)
		maxRange = 1;

	// First need to get the longitude and latitude range for the search
	double deltaLat = metersToLat(maxRange);
	double lowLat = addDegrees(latitude, -deltaLat);
	double highLat = addDegrees(latitude, deltaLat);

	double deltaLong = metersToLong(latitude, maxRange);
	double lowLong = addDegrees(longitude, -deltaLong);
	double highLong = addDegrees(longitude, deltaLong);

	// Create the rds query string
	string query = "select * from pois where lat >= " + to_string(lowLat) + 
					" and lat <= " +	to_string(highLat) + 
					" and lng >= " +	to_string(lowLong) + 
					" and lng <= " +	to_string(highLong);

	// Run the query on the server
	sql::PreparedStatement *prep_stmt;
	sql::ResultSet *res;
	prep_stmt = con->prepareStatement(query);
	res = prep_stmt->executeQuery();
	
	// dbPlaceVec will hold Places that were returned by the database query (filtering will happen later)
	vector<Place> dbPlaceVec;

	// For every server result, create a Place object and fill it with the result data
	while (res->next()) {
		Place tempPlace;
		tempPlace.type = "";

		//for each tuple result, we use getString("attribute") to retrieve the data corrosponding to the attribute name
		tempPlace.name = res->getString("Place"); 
		tempPlace.latitude = stod(res->getString("Lat")); 

		tempPlace.longitude = stod(res->getString("Lng"));
		tempPlace.type = res->getString("Types");

		dbPlaceVec.push_back(tempPlace);
		// cout << res->getString("Place") << "," << res->getString("Lat") << "," << res->getString("Lng") << "," << res->getString("Types") << endl;
	}

	// clean up
	delete res;
	delete prep_stmt;

	return dbPlaceVec;
}

// This function will filter a vector of Places based on the filters string
// The format for the filter string is as follows:
// "type1,type2,type3|type4|type5,type6"
// This filter string makes it so that the Places returned by this function will have
// (type1, type2, and type 3) OR type 4 OR (type 5 and type 6) in the Place's type string
vector<Place> ServerCall::filterPlaces(vector<Place> places, string filters)
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

	// For each place in places, check if it passes the filters
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
vector<Place> ServerCall::randomSelectPlaces(vector<Place> places, int numPlaces)
{
	int placesSize = places.size();
	// If we request more places than exist in our input vector, simply return the input vector
	if(numPlaces >= placesSize)
		return places;

	vector<Place> selectedPlaces;

	// Creating a copy is important to avoid modifying the input vector
	vector<Place> places_copy (places);

	// On each iteration, select a Place, add it to the output vector, and remove it from the places_copy
	for(int i = 0; i < numPlaces; i++)
	{
		int currIndex = rand() % places_copy.size();

		selectedPlaces.push_back(places_copy[currIndex]);
		places_copy.erase(places_copy.begin() + currIndex);

	}

	/*
	int selectedPlaceIndices [numPlaces];
	// Fill with -1 to avoid issues when checking if an index already exists in the array
	for(int i = 0; i < numPlaces; i++)
	selectedPlaceIndices[i] = -1;

	// On each iteration, we will randomly select one of the indexes of places
	// If we had already selected this index, we try again
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
	*/

	return selectedPlaces;

}


// SearchByCoordinate will find at most numPlaces that are within a square of side length 2*maxRange from the specified coordinate
// Additionally, only places that satisfy the filters will be accepted in the output
// This function will return a vector of Places that meet the requirements
// If filters is empty (filters == ""), then filtering is disabled
// maxRangeIncr is used if you want to override the maximum extra range that the function will look in
// maxRangeIncr is optional and default set to maxRangeIncrement = rangeIncrement * maxIncrementAttempts
vector<Place> ServerCall::SearchByCoordinate(double latitude, double longitude, double maxRange, int numPlaces, string filters, double maxRangeIncr)
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


	// We will now attempt to find more than numPlaces if there were fewer than that number after filtering
	double currExtraRange = rangeIncrement;
	unsigned int uint_numPlaces = (unsigned int) numPlaces;

	// Will try up to get enough POIs to satisfy numPlaces (if there weren't enough found by the initial search)
	for(int i = 0; i < maxIncrementAttempts; i++, currExtraRange += rangeIncrement)
	{
		// If we have enough POIs or if we hit the maximum range extension, stop the loop
		if(outputPlaces.size() >= uint_numPlaces || currExtraRange > maxRangeIncr)
			break;

		// If we reach this line, then there were not enough POIs in the original range, so we clear our vectors and try again
		dbPlaceVec.clear();
		outputPlaces.clear();

		// Query the database with the increased range
		dbPlaceVec = queryDatabase(latitude, longitude, maxRange + currExtraRange);

		// Now we need to filter the results in dbPlaceVec based on the input filters
		// If a filter is specified, filter our Places
		if(filters != "")
			outputPlaces = filterPlaces(dbPlaceVec, filters);
		else
		//Otherwise, retain all Places
			outputPlaces = dbPlaceVec;
	}
	
	// Out of the remaining places that satisfy the filter, select up to numPlaces to return
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
	// If only one place is requested, just return a place near the init coordinate, since we can't make a line
	else if(numPlaces == 1)
		return SearchByCoordinate(initLatitude, initLongitude, maxRange, numPlaces, filters);

	// Otherwise, we need to split the line to have numPlaces locations
	// Pretend latitude is a y value and longitude is an x value in a 2D coordinate plane
	double longitudeChange = endLongitude - initLongitude;
	double slope;
	if(longitudeChange == 0)
		slope = 0;
	else
		slope = (endLatitude - initLatitude) / longitudeChange;

	double currLat = initLatitude;
	double currLong = initLongitude;
	// Partition the longitude into (numPlaces - 1) segments since the initial coordinate will be used as the first location
	double longitudeStep = longitudeChange / (numPlaces - 1);
	double latitudeStep = longitudeStep * slope;

	// This variable is used in the case that the first locations didn't have enough POIs
	// So it will try to make up by finding more POIs in future locations
	int backlogSize = 0;

	// Will store how many locations we use from each index and how much backlog was cleared by each index
	unsigned int numPOIs[numPlaces];
	unsigned  int backlogCleared[numPlaces];
	for(int i = 0; i < numPlaces; i++){
		numPOIs[i] = 0;
		backlogCleared[i] = 0;
	}

	// We do a search at every point in the line
	for(int i = 0; i < numPlaces; i++, currLong += longitudeStep, currLat += latitudeStep)
	{
		// Find a location at the current coordinate (finding extra if the backlog is non-zero)
		vector<Place> currPlace = SearchByCoordinate(currLat, currLong, maxRange, 1 + backlogSize, filters);
		//cout << "BackLog: " << backlogSize << ", Lat: " << currLat << ", Long: " << currLong << endl;

		// If a POI was found in this area
		if(!currPlace.empty())
		{
			unsigned int numFound = currPlace.size();
			// State how many POIs were found in this current location
			numPOIs[i] = numFound;
			backlogCleared[i] = (numFound - 1);
			// Update the backlogSize
			backlogSize -= (numFound - 1);

			// Add the POI(s) to the output list
			outputPlaces.insert(outputPlaces.end(), currPlace.begin(), currPlace.end());
		}
		// If a POI was not found in this area, we will attempt to search previous areas
		// It will only do this if the backlogSize is 0, since if it is > 0, then it means we already failed
		// to find extra locations in the previous areas
		else if (backlogSize == 0)
		{
			int j;
			// We will attempt to find more POIs in the previous areas
			for(j = i - 1; j >= 0; j--)
			{
				// If the area j found a POI, try to find an extra POI there
				unsigned int numFound = numPOIs[j];

				if(numFound != 0)
				{
					// Calculate the coordinates of the old location
					double prevLat = initLatitude + latitudeStep * j;
					double prevLong = initLongitude + longitudeStep * j;

					// We will search for an extra POI here
					currPlace = SearchByCoordinate(prevLat, prevLong, maxRange, numFound + 1, filters);

					// If we successfully found an extra POI at this location
					if(currPlace.size() == numFound + 1)
					{
						int numCleared = backlogCleared[j];
						int currIndexStart = j - numCleared;
						// To avoid adding the same location multiple times, we remove POIs found in this old area
						for(unsigned int k = 0; k < numFound; k++)
							outputPlaces.erase(outputPlaces.begin() + currIndexStart);

						outputPlaces.insert(outputPlaces.begin() + currIndexStart, currPlace.begin(), currPlace.end());
						numPOIs[j]++;

						// We now leave the loop since we found an extra POI here
						break;
					}
					// Otherwise, we go to the following previous location and try again
				}
			}

			// If we couldn't find any extra POIs in the previous locations, increment our backlog to search
			// for extra POIs in future locations
			if(j == -1)
				backlogSize++;
		}
		// If backlogSize was non-zero, there is no point in searching previous areas
		else
			backlogSize++;
	}

	return outputPlaces;
}
