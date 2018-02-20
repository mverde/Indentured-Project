import googlemaps
import MySQLdb
import math
gmaps = googlemaps.Client(key='AIzaSyBmvB1gGLH0cujfkhQylJu6St3BIqLcvwU')

###places_dict = gmaps.places('', (34.0537136,-118.24265330000003), 10000)
# places_dict = gmaps.places_nearby(location=(34.0537136,-118.24265330000003), radius=1)
# print places_dict['results']

def getLocations(coordinates, rad):
    # input: coordinates (latitude,longitude), radius
    # output: dict of locations
    return  gmaps.places_nearby(location=coordinates, radius=rad)

gmaps = googlemaps.Client(key='AIzaSyBmvB1gGLH0cujfkhQylJu6St3BIqLcvwU')

def milesToMeters(miles):
    return miles*1609.34

def metersToLatitude(meters):
    return meters/110540

def metersToLongitude(meters, latitude):
    return 111320*math.cos(math.radians(latitude))*meters

# Globals
defaultSearchRadiusInMiles = 0.25 
defaultSearchRadius = milesToMeters(defaultSearchRadiusInMiles)
degreeDelta = 10

def searchAreaAlgorithm(latitude, longitude, radius):
    # input: starting & ending coordinates & radius of circle
    startLat = latitude + metersToLatitude(milesToMeters(radius))
    startLong = longitude + metersToLongitude(milesToMeters(radius))
    
    currentLat = startLat + metersToLatitude(defaultSearchRadius)
    currentLong = startLong - metersToLatitude(defaultSearchRadius)
    searchResults = []

    while(currentLong < endLong):  
        while(currentLat < endLat):
            searchResults.append((gmaps.places_nearby(location=(currentLat, currentLong), radius=defaultSearchRadius))['results'])
            currentLat += metersToLatitude(defaultSearchRadius)*2
        currentLong += metersToLatitude(defaultSearchRadius)*2
        currentLat = startLat + metersToLongitude(defaultSearchRadius, metersToLatitude(currentLat))

    return searchResults

def addToDB(array):
    #input: locations array - add to db all at once    
    # db columns: index, place, coordinate(lat, longitude), type

    # ================= Connect to DB ================= #
    db = MySQLdb.connect(host= "localhost",
                    user="root",
                    passwd="password")
    cursor = db.cursor()
    # cursor.execute("SET sql_notes = 0; ")       # Suppress warning

    # ================= Default database and table set up ================= #
    cursor.execute('CREATE DATABASE IF NOT EXISTS pois');
    cursor.execute('USE pois')
    cursor.execute("DROP TABLE IF EXISTS test")
    cursor.execute("CREATE TABLE test (place VARCHAR(70), lat DECIMAL(10, 8) NOT NULL, lng DECIMAL(11, 8) NOT NULL)")

    # ================= Parse array ================= #
    for entry in array:
            # print (entry)
            # print ('\n')

            # print ("*** NAME OF THING ***" + entry['name'])
            placeName = entry['name']
            lat = entry['geometry']['location']['lat']
            lng = entry['geometry']['location']['lng']

            try:
                params = (placeName, lat, lng)
                cursor.execute("""
                    INSERT INTO test 
                    VALUES
                        (%s, %s, %s)
                        """, params)

                db.commit()
            except:
                db.rollback()
    db.close()
    return

def main():
    addToDB(getLocations((34.0537136,-118.24265330000003), 1)['results'])
    # print searchAreaAlgorithm(34, -118 , 1000)

main()


