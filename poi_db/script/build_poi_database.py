import googlemaps
# import mysql.connector

gmaps = googlemaps.Client(key='AIzaSyBmvB1gGLH0cujfkhQylJu6St3BIqLcvwU')

def milesToMeters(miles):
    return miles*1609.34

# Globals
defaultSearchRadiusInMiles = 0.25 
defaultSearchRadius = milesToMeters(defaultSearchRadiusInMiles)
degreeDelta = 10

def searchAreaAlgorithm(startLat, startLong, radius):
    # input: start coordinate 
    # only returns 60 places.. then gotta go on to next place ... how far should we spread each search??
    # lets do every 1 mile for now ...
    # when to stop??? well when ur out of radius
    # so given start is the center.. think of unit circle.. 

    count = 0
    degree = 0
    searchResults = []
    rad = milesToMeters(radius)

    # 360 degrees in a circle, move by 1 degree each time
    while(degree < 360):
        currentPoint = 0
        while(currentPoint < rad):
            #search spot
            searchResults.append((gmaps.places_nearby(location=(startLat, startLong), radius=defaultSearchRadius))['results'])
            currentPoint = currentPoint + defaultSearchRadius*2
            print ("current point: ", currentPoint)
        degree += degreeDelta
        print ("degree: ", degree)
    
    return searchResults

def addToDB(place, latitude, longitude, typeOfPlace):
    #input: locations array - add to db all at once    
    # db columns: index, place, coordinate(lat, longitude), type
    print (place, latitude, longitude, typeOfPlace)


def main():
    # print getLocations((34.0537136,-118.24265330000003), 1)['results'];
    print searchAreaAlgorithm(34, -118, 1)
    

main()


