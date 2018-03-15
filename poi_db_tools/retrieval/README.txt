=========================
How to use ServerCall.cpp and its test files:
=========================

1. ServerCall requires the MySQL/C++ connector to be set up: https://dev.mysql.com/doc/connector-cpp/en/

2. To obtain the API functionalities, simply include
	#include "ServerCall.h"
	
	in your C++ program.
	
3. To compile a program, such as ServerUserInput.cpp, run the following command on the AWS server at the location of the script
	sudo g++ -std=c++0x -Wall -I/usr/include/cppconn -o TestUserInput ServerCall.cpp ServerUserInput.cpp -L/usr/lib -lmysqlcppconn
	
4. Modifiable parameters:
	In ServerCall.h, there are two relevant modifiable global variables:
		1) rangeIncrement
		2) maxIncrementAttempts
		
	These two variables are used for incremental search area expansion in the case that the user specified range in the function parameters does not
	provide enough locations to satisfy the desired number of locations.
	
	For example, if a user specifies a range of 100 meters and desires 10 locations, if there are only 5 locations within 100 meters of the coordinate,
	then the function will automatically now search in a range of (100 + rangeIncrement). This range expansion will repeat up to maxIncrementAttempts times,
	and will stop early if 10 locations are found in the new range.

	
5. Helper Functions Overview
	Place Struct:
		Includes the name, latitude, longitude, and types of a POI
		
	Conversion Functions:
		These are used to allow the user to specify ranges in meters, which later need to be converted to degrees in order to query the database.
		
	Split:
		This allows a string to be split into a vector based on a delimiter. This is used to parse database outputs, which are first stored as delimited strings.
		(e.g. "Location Name, Latitude, Longitude, Type1, Type2, Type3, ...")
	
	ServerCall Constructor:
		Initializes random seed, which is used for randomly selecting from a list of locations.
		Sets up a connection to the MySQL database in the AWS database.
	
	queryDatabase:
		Uses the coordinate and specified range to create a MySQL query string.
		Converts each of the results into a vector (essentially a list) of Place objects
		
	filterPlaces:
		Selects a subset of the input Places that follow the filter rules specified by the input string.
		For an overview of the filter string format, refer to Section 7 of this document.
			
	randomSelectPlaces:
		This function will randomly select a subset of the input Places.
		
		
6. Main/Important Functions Overview
	SearchByCoordinate (double latitude, double longitude, double maxRange, int numPlaces, string filters, double maxRangeIncr = maxRangeIncrement):
		This funtion will return at most numPlaces within a square of side length 2*maxRange centered around the specified coordinate
		(where maxRange is specified in meters)
		And will return results that follow the filtering rules, which are to be specified by the filters string

		maxRangeIncr is an optional parameter in case you want to specify the maximum range expansion when searching for POIs
		(maximum range expansion will increase the search range to a larger area if the smaller area found fewer than numPlaces POIs)
		
		
	SearchByLine (double initLatitude, double initLongitude, double endLatitude, double endLongitude, double maxRange, int numPlaces, string filters):
		SearchByLine will find numPlaces that are in a line between the initial coordinate and the end coordinate
		Additionally, only places that satisfy the filters will be accepted in the output
		This function will return a vector of Places that meet the requirements
		The parameter types are identical to that of SearchByCoordinate.
		
		
7. Using Filter Strings
	Filter strings are used as parameters for SearchByCoordinate and SearchByLine.
	The format for the filter string is as follows:
	"type1,type2,type3|type4|type5,type6"
	This filter string makes it so that the Places returned by this function will have
	(type1, type2, and type 3) OR type 4 OR (type 5 and type 6) in the Place's type string
	
	Examples:
	Ex 1) "I want locations that are restaurants"
			=> filterString = "restaurant"
			
	Ex 2) "I want locations that are restaurants or bars
			=> filterString = "restaurant|bar"
			
	Ex 3) "I want locations that are both bars and clubs
			=> filterString = "bar,club"
	
	Ex 4) "I want locations that are (restaurants and points of interest) or (clubs and bars)
			=> filterString = "restaurant,point_of_interest|club,bar"
		