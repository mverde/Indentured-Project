import googlemaps

gmaps = googlemaps.Client(key='AIzaSyBmvB1gGLH0cujfkhQylJu6St3BIqLcvwU')

###places_dict = gmaps.places('', (34.0537136,-118.24265330000003), 10000)
# places_dict = gmaps.places_nearby(location=(34.0537136,-118.24265330000003), radius=1)
# print places_dict['results']

 
def getLocations(coordinates, rad):
    # input: coordinates (latitude,longitude), radius
    # output: dict of locations
    return  gmaps.places_nearby(location=coordinates, radius=rad)

def searchAreaAlgorithm(start):
    # input: start coordinate 


def addToDB(place, coordinate, type):
    #input: locations array - add to db all at once    
    # db columns: index, place, coordinate(lat, longitude), type


def main():
    print queryGoogle((34.0537136,-118.24265330000003), 1)['results'];

main()


