import googlemaps
import math

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
    startLat = latitude - metersToLatitude(milesToMeters(radius))
    startLong = longitude + metersToLongitude(milesToMeters(radius), startLat)
    endLat = latitude + metersToLatitude(milesToMeters(radius))
    endLong = longitude - metersToLongitude(milesToMeters(radius), startLat)

    print ("start lat: ", startLat, " startLong: ", startLong, "endLat: ", endLat, " endLong: ", endLong)

    currentLat = startLat + metersToLatitude(defaultSearchRadius)
    currentLong = startLong - metersToLongitude(defaultSearchRadius, currentLat)
    searchResults = []

    print ("current lat: ", currentLat, " currentLong: ", currentLong)

    while(currentLong < endLong):  
        while(currentLat < endLat):
            searchResults.append((gmaps.places_nearby(location=(currentLat, currentLong), radius=defaultSearchRadius))['results'])
            currentLat += metersToLatitude(defaultSearchRadius)*2
        currentLong += metersToLatitude(defaultSearchRadius)*2
        currentLat = startLat + metersToLongitude(defaultSearchRadius, metersToLatitude(currentLat))

    return searchResults

def addToDB(place, latitude, longitude, typeOfPlace):
    #input: locations array - add to db all at once    
    # db columns: index, place, coordinate(lat, longitude), type
    print (place, latitude, longitude, typeOfPlace)


def main():
    # print getLocations((34.0537136,-118.24265330000003), 1)['results'];
    # print gmaps.places_nearby(location=(34.0537136,-118.24265330000003), radius=defaultSearchRadius)['results']
    print searchAreaAlgorithm(34, -118, 1000)
    

main()


