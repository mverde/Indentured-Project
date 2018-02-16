import googlemaps
import math

'''
Global variables.
'''
DEFAULT_SEARCH_RADIUS_MILES = 0.25
DEFAULT_SEARCH_RADIUS_METERS = milesToMeters(DEFAULT_SEARCH_RADIUS_MILES)
EARTH_RADIUS_METERS = 6371000

'''
Google Maps Client interface object. In production, the API key should be
stored locally.
'''
gmaps = googlemaps.Client(key='AIzaSyBmvB1gGLH0cujfkhQylJu6St3BIqLcvwU')

def milesToMeters(miles):
    return miles*1609.34

def metersToLatitude(meters):
    return meters/110540

def metersToLongitude(meters, latitude):
    return 111320*math.cos(math.radians(latitude))*meters

'''
Functions to get new latitude and longitude coordinates by adding meters to 
old coordinates. This necessitates scaling and wrap-around handling for
longitude conversions. Following established convention, we treat negative
meters as going South/West and vice versa. Note that latPlusMeters does not
handle situations where the new latitude would cross one of the poles.
'''
def latPlusMeters(latitude, meters):
    return latitude + (meters / EARTH_RADIUS_METERS) * (180.0 / math.pi);

def longPlusMeters(longitude, latitude, meters):
    longToAdd = (meters / EARTH_RADIUS_METERS) * (180.0 / math.pi) / math.cos(latitude * math.pi / 180.0)
    newLong = longitude + longToAdd

    if newLong > 180.0:
        return -180.0 + (newLong - 180.0)
    elif newLong < -180.0:
        return 180.0 - (newLong + 180.0)
    
    return newLong

def searchAreaAlgorithm(latitude, longitude, radius):
    # input: starting & ending coordinates & radius of circle
    startLat = latitude + metersToLatitude(milesToMeters(radius))
    startLong = longitude + metersToLongitude(milesToMeters(radius))
    
    currentLat = startLat + metersToLatitude(DEFAULT_SEARCH_RADIUS_METERS)
    currentLong = startLong - metersToLatitude(DEFAULT_SEARCH_RADIUS_METERS)
    searchResults = []

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
    print searchAreaAlgorithm(34, -118 , 1000)
    

main()


