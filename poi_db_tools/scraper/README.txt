==========
How to use build_poi_database.py:
==========

1. Make sure your Python environment has googlemaps and pymysql installed ($> pip install <package name>)

2. Run '$> python build_poi_database.py <lat> <long> <radius>'
   - lat and long refer to the coordinates of the center of the area you wish to search for Points of Interest.
   - radius refers to the radius in meters of the area you wish to search. This radius should not be less than 250 meters. 

==========
How to run tests manually:
==========

1. Make sure your Python environment has pytest installed ($> pip install pytest)

2. cd into the directory containing build_poi_database_test.py and run '$> py.test'