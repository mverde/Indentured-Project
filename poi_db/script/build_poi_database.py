import googlemaps

gmaps = googlemaps.Client(key='AIzaSyBmvB1gGLH0cujfkhQylJu6St3BIqLcvwU')

###places_dict = gmaps.places('', (34.0537136,-118.24265330000003), 10000)

places_dict = gmaps.places_nearby(location=(34.0537136,-118.24265330000003), radius=1)

print places_dict['results']
