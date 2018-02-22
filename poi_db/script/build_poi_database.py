import googlemaps
# import MySQLdb
import pymysql.cursors
import math

'''
Google Maps Client interface object. In production, the API key should not be
stored locally.
'''
gmaps = googlemaps.Client(key='AIzaSyBmvB1gGLH0cujfkhQylJu6St3BIqLcvwU')

def milesToMeters(miles):
    return miles*1609.34

'''
Global variables.
'''
DEFAULT_GRID_SQUARE_LENGTH_METERS = milesToMeters(0.13)
DEFAULT_SEARCH_RADIUS_MILES = 0.13
DEFAULT_SEARCH_RADIUS_METERS = milesToMeters(DEFAULT_SEARCH_RADIUS_MILES)
EARTH_RADIUS_METERS = 6371000


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
        
def getResults(lat, long, searchRadius):
    # helper function to return the list of places; there can be overlap. 

    places = []
    places1 = gmaps.places_nearby(location=(lat,long), radius=searchRadius)

    for place in places1['results']:
        places.append(place)
    
    x=0
    y=0
    
    if('next_page_token' in places1):
        print ("Yes, there is a next page token for places 1")
        while(x < 50):
            try:
                next_page = "" + places1['next_page_token'].encode('ascii','ignore')
                places2 = gmaps.places_nearby(location=(lat,long), radius=searchRadius, page_token = next_page)
                
                for place in places2['results']:
                    places.append(place)
                
                x += 1

                if('next_page_token' in places2):
                    print ("Yes, there is a next page token for places 2")
                    while(y < 50):
                        try:
                            next_page = "" + places2['next_page_token'].encode('ascii','ignore')
                            places3 = gmaps.places_nearby(location=(lat,long), radius=searchRadius, page_token = next_page)
                            
                            for place in places3['results']:
                                places.append(place)
                            
                            return places
                        except (KeyError, googlemaps.exceptions.ApiError) as e:
                            #no next page token
                            print ("Error with fetching results: ", e)
                            y += 1
                            continue
                else:
                    print ("There is no next page token for places 2, end results")
                    return places

            except (KeyError, googlemaps.exceptions.ApiError) as e:
                # no next page token
                print ("Error with fetching results: ", e)
                x += 1
                continue
    else:
        print ("There is no next page token for places 1, end results.")
        return places

    return places

def searchArea(latitude, longitude, radius=DEFAULT_SEARCH_RADIUS_METERS, gridSquareLength=DEFAULT_GRID_SQUARE_LENGTH_METERS):
    # returns array of places
    if radius < gridSquareLength:
        return []

    gridSquareSearchRadius = math.sqrt(2.0 * math.pow(gridSquareLength, 2)) / 2.0
    
    searchGrid = createSearchGrid(latitude, longitude, radius, gridSquareLength)
    locations = []
    
    for gridCenter in searchGrid:
        print ("Grid Center: ", str(gridCenter[0]) + ',' + str(gridCenter[1]))
        locations += getResults(gridCenter[0], gridCenter[1], gridSquareSearchRadius)
    
    return locations

def filterResults(results):
    # return results of type x,y,z 
    return 0

def addToDB(array):
    #input: locations array - add to db all at once    
    # db columns: index, place, coordinate(lat, longitude), type

    # ================= Connect to DB ================= #
    db = pymysql.connect(host= "escality-db-instance.cykpeyjjej2m.us-west-1.rds.amazonaws.com",
                    user="escality_user",
                    passwd="12345678")
    cursor = db.cursor()
    # cursor.execute("SET sql_notes = 0; ")       # Suppress warning

    # ================= Default database and table set up ================= #
    cursor.execute('CREATE DATABASE IF NOT EXISTS pois');
    cursor.execute('USE pois')
    cursor.execute("DROP TABLE IF EXISTS test")
    cursor.execute("CREATE TABLE test (place VARCHAR(70), lat DECIMAL(10, 8) NOT NULL, lng DECIMAL(11, 8) NOT NULL, types TEXT, PRIMARY KEY (place, lat, lng))")

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
                    INSERT INTO test 
                    VALUES
                        (%s, %s, %s, %s)
                    ON DUPLICATE KEY UPDATE
                    types=GREATEST(types,VALUES(types))
                        """, params)

                db.commit()
            except:
                db.rollback()
    db.close()
    return

def main():
    addToDB(searchArea(34.0537136, -118.24265330000003))
    #print searchArea(34, -118 , 1000)
    # searchArea(34.0537136, -118.24265330000003)

main()
