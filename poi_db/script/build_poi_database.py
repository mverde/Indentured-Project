import googlemaps
# import mysql.connector

gmaps = googlemaps.Client(key='AIzaSyBmvB1gGLH0cujfkhQylJu6St3BIqLcvwU')

###places_dict = gmaps.places('', (34.0537136,-118.24265330000003), 10000)
# places_dict = gmaps.places_nearby(location=(34.0537136,-118.24265330000003), radius=1)
# print places_dict['results']

#dummy data
# dummyPlace = {
#     place: 'UCLA',
#     latitude: 34.05,
#     longitude: -118,
#     typeOfPlace: 'School' 
# }
 
def getLocations(coordinates, rad):
    # input: coordinates (latitude,longitude), radius
    # output: dict of locations
    return  gmaps.places_nearby(location=coordinates, radius=rad)

def searchAreaAlgorithm(start):
    # input: start coordinate 
    # only returns 60 places.. then gotta go on to next place ... how far should we spread each search??
    print 'hi'
    


def addToDB(place, latitude, longitude, typeOfPlace):
    #input: locations array - add to db all at once    
    # db columns: index, place, coordinate(lat, longitude), type
    print (place, latitude, longitude, typeOfPlace)


def main():
    print getLocations((34.0537136,-118.24265330000003), 1)['results'];
    # addToDB(dummyPlace.place, dummyPlace.latitude, dummyPlace.longitude, dummyPlace.typeOfPlace)


main()


