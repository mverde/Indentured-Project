import build_poi_database as bpdb
import pymysql.cursors
import pytest

LAT_LONG_ERROR_MARGIN = 0.0075
DB_ARRAY = {'types': ['locality', 'political'], 'photos': [{'height': 2322, 'html_attributions': ['<a href="https://maps.google.com/maps/contrib/102571719816690763141/photos">SKY RHEE</a>'], 'width': 4128, 'photo_reference': 'CmRaAAAAJHNQWZ_i9BHWPOszF1rR4jnckSXbcwrUVzVRWWzvmEgDPal5jb-7Mi9_lPf0E3bm7xY5Wv-dPp8Me57oI77V3pWykss47Qdw7h48xnZ52YGxCGcN2o87_YLem58-rbySEhA6RFLEsO3kgygLIYxHfKn9GhSrXu29BtE5ZXZd_sdCeAu-68iFrw'}], 'reference': 'CmRbAAAAAICUcT5M2BK2sBJJ4Ey4czciT0aoW6N7R2KcaqRpTR2ZgE30jmi0ve7BTdlNC9YMOsGonHh__90Orm_ScjGkXLPS0hf9t6IjmIXg1q3mf_DG9ZKywyWyOgmzkCZ5byNrEhCrj6zQmjGNAW-y_XSoxd4BGhRG2vGpXSsrw3dTxqw_ssRDqp8p9g', 'name': 'Los Angeles', 'id': '7f7b7d8118ae8db8ed3f541159ac928c484d12ad', 'geometry': {'viewport': {'northeast': {'lat': 34.3373061, 'lng': -118.1552891}, 'southwest': {'lat': 33.7036519, 'lng': -118.6681759}}, 'location': {'lat': 34.0522342, 'lng': -118.2436849}}, 'icon': 'https://maps.gstatic.com/mapfiles/place_api/icons/geocode-71.png', 'scope': 'GOOGLE', 'place_id': 'ChIJE9on3F3HwoAR9AhGJW_fL-I', 'vicinity': 'Los Angeles'}

def test_latPlusMeters_positive_correctness():
    assert abs(bpdb.latPlusMeters(0.0, 111000) - 1.0) <= LAT_LONG_ERROR_MARGIN

def test_latPlusMeters_negative_correctness():
    assert abs(bpdb.latPlusMeters(0.0, -111000) - (-1.0)) <= LAT_LONG_ERROR_MARGIN

def test_longPlusMeters_positive_correctness():
    assert abs(bpdb.longPlusMeters(0.0, 0.0, 110567) - 1.0) <= LAT_LONG_ERROR_MARGIN

def test_longPlusMeters_negative_correctness():
    assert abs(bpdb.longPlusMeters(0.0, 0.0, -110567) - (-1.0)) <= LAT_LONG_ERROR_MARGIN

def test_longPlusMeters_positive_wraparound():
    assert abs(bpdb.longPlusMeters(180.0, 0.0, 110567) - (-179.0)) <= LAT_LONG_ERROR_MARGIN

def test_longPlusMeters_negative_wraparound():
    assert abs(bpdb.longPlusMeters(-180.0, 0.0, -110567) - 179.0) <= LAT_LONG_ERROR_MARGIN

def test_createSearchGrid_invalid_radius():
    # Test that no grid is returned if radius < gridSquareRadius
    assert bpdb.createSearchGrid(0.0, 0.0, 1000, 10000) == []

def test_createSearchGrid_creates_correct_grid():
    # Test that grid centers are correctly spaced from each other and cover the correct area
    dummyGridCenters = [(0.006744912044390478, -0.006744912139593897), \
                        (0.006744912044390478, 0.0022483039819083716), \
                        (-0.0022483040147968265, -0.006744912139593897), \
                        (-0.0022483040147968265, 0.0022483039265172927)]
    assert bpdb.createSearchGrid(0.0, 0.0, 1000, 1000) == dummyGridCenters
    
def test_addToDB():
    #  ================= For Albert  ================= #

    # db = pymysql.connect(host= "escality-db-instance.cykpeyjjej2m.us-west-1.rds.amazonaws.com",
    #             user="escality_user",
    #             passwd="12345678")
    
    #  ================= For Melissa  ================= #
    db = pymysql.connect(host= "localhost",
                    user="root",
                    passwd="password")
    cursor = db.cursor()
    cursor.execute('USE escality_location_db')
    cursor.execute('SELECT * from test')
    
    output = []
    for row in cursor:
        row_data = []
        for data in row:
            if type(data) is str:
                row_data.append(str(data))
            else:
                row_data.append(float(data))
        output.append(row_data)

    assert all([a == b for a, b in zip(output, [['Central LA', 34.0686748, -118.3228165, 'neighborhood,political'], ['Los Angeles', 34.0522342, -118.2436849, 'locality,political'], ['Los Angeles City Hall', 34.0535267, -118.2429316, 'city_hall,premise,local_government_office,point_of_interest,establishment'], ['Los Angeles City Hall', 34.0537136, -118.2426533, 'city_hall,local_government_office,point_of_interest,establishment']])])