#ifndef SERVERCALL_H_
#define SERVERCALL_H_

#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <iostream>
#include <ctime>
#include <algorithm>

#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

using namespace std;

// Constants used in area range expansion when not enough POIs are found
const double rangeIncrement = 805; // 805 meters = 0.5 miles
const int maxIncrementAttempts = 4;
const double maxRangeIncrement = rangeIncrement * maxIncrementAttempts;

// A structure to define a point of interest
// The type string will be a comma separated string, where each value is one of the location's types
struct Place
{
	string name;
	double latitude;
	double longitude;
	string type;
};

// This class handles server connections and the querying of the server database
class ServerCall
{
public:
	// Constructor
	// Sets up connection to the RDS on the AWS server
	ServerCall();	

	// This funtion will return at most numPlaces within a square of side length 2*maxRange centered around the specified coordinate
	// (where maxRange is specified in meters)
	// And will return results that follow the filtering rules, which are to be specified by the filters string
	// The format for the filters string is as follows:
	// "type1,type2,type3|type4|type5,type6"
	// This filter string makes it so that the Places returned by this function will have
	// (type1, type2, and type 3) OR type 4 OR (type 5 and type 6) in the Place's types
	// maxRangeIncr is an optional parameter in case you want to specify the maximum range expansion when searching for POIs
	// (maximum range expansion will increase the search range to a larger area if the smaller area found fewer than numPlaces POIs)
	vector<Place> SearchByCoordinate(double latitude, double longitude, double maxRange, int numPlaces, string filters, double maxRangeIncr = maxRangeIncrement);
	
	// SearchByLine will find numPlaces that are in a line between the initial coordinate and the end coordinate
	// MaxRange will specify how far away from the line a place can be
	// Additionally, only places that satisfy the filters will be accepted in the output
	// This function will return a vector of Places that meet the requirements, which are specified in the aforementioned format for the filters string
	vector<Place> SearchByLine(double initLatitude, double initLongitude, double endLatitude, double endLongitude, double maxRange, int numPlaces, string filters);


	//**********************************************************//
	// Helper functions for SearchByCoordinate
	// Set to public functions for the sake of testing
	vector<Place> filterPlaces(vector<Place> places, string filters);
	vector<Place> randomSelectPlaces(vector<Place> places, int numPlaces);	

private:
	// This function will return all locations within a square of side length 2*maxRange centered around the specified coordinate
	// Mainly used as a helper function for SearchByCoordinate
	vector<Place> queryDatabase(double latitude, double longitude, double maxRange);
	sql::Driver *driver;
  	sql::Connection *con;
};


#endif // SERVERCALL_H_