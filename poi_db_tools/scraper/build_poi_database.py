import sys
import googlemaps
# import MySQLdb
import pymysql.cursors
import math

'''
Google Maps Client interface object. In production, the API key should not be
stored locally.
'''
gmaps = googlemaps.Client(key='AIzaSyBmvB1gGLH0cujfkhQylJu6St3BIqLcvwU')

# def getLocations(coordinates, rad):
#     # input: coordinates (latitude,longitude), radius
#     # output: dict of locations
#     return  gmaps.places_nearby(location=coordinates, radius=rad)

def milesToMeters(miles):
    return miles*1609.34

'''
Global variables.
'''
# Approximate radius that returns ~60 results per query in Downtown LA
DEFAULT_GRID_SQUARE_LENGTH_METERS = milesToMeters(0.13)
# Approximate radius of LA City
DEFAULT_SEARCH_RADIUS_MILES = 12.427424
DEFAULT_SEARCH_RADIUS_METERS = milesToMeters(DEFAULT_SEARCH_RADIUS_MILES)
EARTH_RADIUS_METERS = 6371000
TYPES_OF_PLACES = ['bar', 'beauty_salon', 'bicycle_store', 'book_store', 'cafe', 'cemetery', 'gym', 'museum', 'parking']

'''
Functions to get new latitude and longitude coordinates by adding meters to 
old coordinates. This necessitates scaling and wrap-around handling for
longitude conversions. Following established convention, we treat negative
meters as going South/West and vice versa. Note that latPlusMeters does not
handle situations where the new latitude would cross one of the poles.
'''
def latPlusMeters(latitude, meters):
    return latitude + (meters * 1.0 / EARTH_RADIUS_METERS) * (180.0 / math.pi);

def longPlusMeters(longitude, latitude, meters):
    longToAdd = (meters * 1.0 / EARTH_RADIUS_METERS) * (180.0 / math.pi) / math.cos(latitude * math.pi / 180.0)
    newLong = longitude + longToAdd

    if newLong > 180.0:
        return -180.0 + (newLong - 180.0)
    elif newLong < -180.0:
        return 180.0 + (newLong + 180.0)
    
    return newLong

def createSearchGrid(centerLat, centerLong, radius, gridSquareLength):
    if radius < gridSquareLength:
        return []
    startLat = latPlusMeters(centerLat, radius)
    startLong = longPlusMeters(centerLong, startLat, -radius)
    squaresPerRow = int(math.ceil((2.0 * radius) / gridSquareLength))
    startLat = latPlusMeters(startLat, (radius * -1.0) / (squaresPerRow * 2.0))
    startLong = longPlusMeters(startLong, startLat, (radius * 1.0) / (squaresPerRow * 2.0))
    gridCenters = []
    
    nextGridCenter = (startLat, startLong)
    for i in range(0, squaresPerRow):
        for j in range(0, squaresPerRow):
            gridCenters.append(nextGridCenter)
            nextLong = longPlusMeters(nextGridCenter[1], nextGridCenter[0], gridSquareLength)
            nextGridCenter = (nextGridCenter[0], nextLong)
        
        nextLat = latPlusMeters(nextGridCenter[0], -gridSquareLength)
        nextGridCenter = (nextLat, startLong)
    
    return gridCenters
        
def getCertainTypesOfResults(lat, long, searchRadius):
    
    returnPlaces = []

    for placeType in TYPES_OF_PLACES:
        tempResults = getResults(lat, long, searchRadius, placeType)
        if len(tempResults) > 0:
            returnPlaces.append(getResults(lat, long, searchRadius, placeType))

    return returnPlaces


def getResults(lat, long, searchRadius, typeOfPlace):
    # helper function to return the list of places; there can be overlap. 


    places = []
    places1 = gmaps.places_nearby(location=(lat,long), radius=searchRadius, type=typeOfPlace)

    for place in places1['results']:
        places.append(place)
    
    x=0
    y=0
    
    if('next_page_token' in places1):
        # print ("Yes, there is a next page token for places 1")
        while(x < 50):
            try:
                next_page = places1['next_page_token'].encode('ascii','ignore')
                places2 = gmaps.places_nearby(location=(lat,long), radius=searchRadius, type=typeOfPlace, page_token = next_page)
                
                for place in places2['results']:
                    places.append(place)

                x += 1

                if('next_page_token' in places2):
                    # print ("Yes, there is a next page token for places 2")
                    while(y < 50):
                        try:
                            next_page = places2['next_page_token'].encode('ascii','ignore')
                            places3 = gmaps.places_nearby(location=(lat,long), radius=searchRadius, type=typeOfPlace, page_token = next_page)
                            
                            for place in places3['results']:
                                places.append(place)
                            
                            return places
                        except (KeyError, googlemaps.exceptions.ApiError) as e:
                            #no next page token
                            #print ("Error with fetching results: ", e)
                            y += 1
                            continue
                else:
                    #print ("There is no next page token for places 2, end results")
                    return places

            except (KeyError, googlemaps.exceptions.ApiError) as e:
                # no next page token
                #print ("Error with fetching results: ", e)
                x += 1
                continue
    else:
        #print ("There is no next page token for places 1, end results.")
        # for item in places:
        #     print("places results for type: ", typeOfPlace, "\n", item)
        return places

    # for item in places:
    #     print("places results for type: ", typeOfPlace, "\n", item)

    return places

def searchArea(latitude, longitude, radius=DEFAULT_SEARCH_RADIUS_METERS, gridSquareLength=DEFAULT_GRID_SQUARE_LENGTH_METERS):
    # returns array of places
    if radius < gridSquareLength:
        return []

    gridSquareSearchRadius = math.sqrt(2.0 * math.pow(gridSquareLength, 2)) / 2.0
    
    searchGrid = createSearchGrid(latitude, longitude, radius, gridSquareLength)
    locations = []
    
    for gridCenter in searchGrid:
        # print ("Grid Center: ", str(gridCenter[0]) + ',' + str(gridCenter[1]))
        locations += getCertainTypesOfResults(gridCenter[0], gridCenter[1], gridSquareSearchRadius)
    
    results = []

    for item in locations:
        for loc in item:
            results.append(loc)

    # for x in results:
    #     print(x['name'])

    return results


def addToDB(array):
    #input: locations array - add to db all at once    
    # db columns: index, place, coordinate(lat, longitude), type

    # ==================== Connect to DB ===================== #


    #  ========== For Testing with AWS  ========== #
    # Uncomment the code below if running the script on AWS
    db = pymysql.connect(host= "escality-db-instance.cykpeyjjej2m.us-west-1.rds.amazonaws.com",
                    user="escality_user",
                    passwd="12345678")

    #  ============ For Local Testing  =========== #
    # Uncomment the code below if testing on a local machine with MySQL.

    # db = pymysql.connect(host= "localhost",
    #             user="root",
    #             passwd="password")

    #  ================= End Connect to DB  ================= #

    cursor = db.cursor()
    cursor.execute("SET sql_notes = 0; ")       # Suppress warning

    # ================= Default database and table set up ================= #
    cursor.execute('CREATE DATABASE IF NOT EXISTS escality_location_db');
    cursor.execute('USE escality_location_db')
    cursor.execute("DROP TABLE IF EXISTS pois")
    cursor.execute("CREATE TABLE pois (place VARCHAR(100), lat DECIMAL(10, 8) NOT NULL, lng DECIMAL(11, 8) NOT NULL, types TEXT, PRIMARY KEY (place, lat, lng))")

    # ================= Parse array ================= #
    for entry in array:
            # print (entry)
            # print ('\n')

            # print ("*** NAME OF THING ***" + entry['name'])
            placeName = entry['name']
            lat = entry['geometry']['location']['lat']
            lng = entry['geometry']['location']['lng']
            types_list = entry['types']
            types = ",".join(types_list)

            try:
                params = (placeName, lat, lng, types)
                cursor.execute("""
                    INSERT INTO pois
                    VALUES
                        (%s, %s, %s, %s)
                    ON DUPLICATE KEY UPDATE
                    types=GREATEST(types,VALUES(types))
                        """, params)

                db.commit()
            except:
                db.rollback()
                
     # ================= Close connection to DB  ================= #
    cursor.close() 
    db.close()
 
    return

def isNumber(s):
    try:
        float(s)
        return True
    except ValueError:
        return False

def main():
    if len(sys.argv) < 4 or sys.argv[1] == 'help' or not isNumber(sys.argv[1]) or not isNumber(sys.argv[2]) or not isNumber(sys.argv[3]):
        print ('Usage: python build_poi_database.py <center latitude> <center longitude> <search radius in meters>')
        return
        
    centerLat = float(sys.argv[1])
    centerLong = float(sys.argv[2])
    searchRadius = float(sys.argv[3])

    searchArea(centerLat, centerLong, searchRadius)
    
    if searchRadius >= 250:
        addToDB(searchArea(centerLat, centerLong, searchRadius))
    else:
        print ("Usage: search radius must be >= 250 meters")

main()
