import build_poi_database as bpdb
import pymysql.cursors
import pytest

LAT_LONG_ERROR_MARGIN = 0.0075
DB_ARRAY = [{'icon': 'https://maps.gstatic.com/mapfiles/place_api/icons/geocode-71.png', 'vicinity': 'Los Angeles', 'place_id': 'ChIJE9on3F3HwoAR9AhGJW_fL-I', 'geometry': {'viewport': {'northeast': {'lng': -118.1552891, 'lat': 34.3373061}, 'southwest': {'lng': -118.6681759, 'lat': 33.7036519}}, 'location': {'lng': -118.2436849, 'lat': 34.0522342}}, 'reference': 'CmRbAAAA9T2rJeT8qVbj8G2PjK4K_fs5Z0xZTFRiKlN11jKBY3-ZtitDqXUZJsVaS8mfzisKd30wEOkQ2EnQTggCTS10Z9wMRHoAN06J6omf0qodYtPesJhsd5jxiWggJo3DuRbUEhBm8Ffbf4HcxxEjG3l1_OueGhSpBHaXpXXcKgvGidEJyxKHnPfyxw', 'name': 'Los Angeles', 'scope': 'GOOGLE', 'types': ['locality', 'political'], 'photos': [{'width': 1728, 'html_attributions': ['<a href="https://maps.google.com/maps/contrib/103375578854730315251/photos">Antonina Reina</a>'], 'height': 1152, 'photo_reference': 'CmRaAAAAIqV53K_b0_hD_D2jGxWSQvHj4DZYs6qOwPNrZXc_XPs1vHvJRrkC0q0C3o6R0Td7gsCHTwhJ_umc1KpnInayZH0oTpDqc6PDSd8hD9gV-UD_IJvFSravam-1lAVsPavvEhAolOO-G958O0h6kXEjHMvVGhTj26kJkcT9AuMHf7i_ap_g4DI5gA'}], 'id': '7f7b7d8118ae8db8ed3f541159ac928c484d12ad'}, {'icon': 'https://maps.gstatic.com/mapfiles/place_api/icons/civic_building-71.png', 'vicinity': 'Los Angeles', 'place_id': 'ChIJ8U1fOU_GwoAR2t6y9AjKEKI', 'geometry': {'viewport': {'northeast': {'lng': -118.2418364697085, 'lat': 34.05513463029151}, 'southwest': {'lng': -118.2445344302915, 'lat': 34.05243666970851}}, 'location': {'lng': -118.2429316, 'lat': 34.05352670000001}}, 'reference': 'CmRbAAAAMAptsfp8BJXAHBDbIPlvJRxnALlSitObCvVYdFla0cSWBSwZneHco2pKvAWRFUDphQIYl9cbUCcW5juxlE6j_vtI7zdcLAIkFEZgLfsy_w2D_XsnxAbm71jQfftK7DfnEhDT9pI8zcXIa4zetqI-tkFKGhQBS493XW1vf14dqdW_1QPxJSv8Ag', 'name': 'Los Angeles City Hall', 'scope': 'GOOGLE', 'types': ['city_hall', 'premise', 'local_government_office', 'point_of_interest', 'establishment'], 'photos': [{'width': 6016, 'html_attributions': ['<a href="https://maps.google.com/maps/contrib/107993465247707575743/photos">petr bousek</a>'], 'height': 4000, 'photo_reference': 'CmRaAAAAy5TrvlEHfM1lwp93q3Pg62Utn1Mp-ddS2Xb3V2259FcmmX0xSDfdby4Mb4tA61AVC1qhaUD2C_lOpdJovwItHzUvlqbrlMQg_n4414-QgJ6obKMIbIQj-A04MBWCjsCUEhCVOnaXs7v7dpzvTJaGfT4CGhRfAlQjZL6N_nvMeX25XO4x_qtNDQ'}], 'id': '7f75c7158a6b7ceac9a8e54d1a28bacf6575a080'}, {'icon': 'https://maps.gstatic.com/mapfiles/place_api/icons/civic_building-71.png', 'rating': 4.5, 'reference': 'CmRRAAAAeNBpwuBL9f32R9ZDMO6xzPUKN43CLgP4ZrhV79tWpDwsHktsObdCn_I_Li0DBWeKrfXK1ueHYFmwjSKdqM2o_XVmYXD2XkVlvoqLST44kWgq7yIF7pgWDTZnvdUdVvEDEhBWkmdx8Vf5bjz1--YJ0zzMGhRikLMWtbyN0_exQsyZVvGSgGkioQ', 'name': 'Los Angeles City Hall', 'id': '88ad570c9fe952eddb3d694b01c5adc6fccb3041', 'place_id': 'ChIJjzUBO0_GwoARrO_fJmtVI1c', 'geometry': {'viewport': {'northeast': {'lng': -118.2414976197085, 'lat': 34.0550804802915}, 'southwest': {'lng': -118.2441955802915, 'lat': 34.0523825197085}}, 'location': {'lng': -118.2426533, 'lat': 34.0537136}}, 'scope': 'GOOGLE', 'types': ['city_hall', 'local_government_office', 'point_of_interest', 'establishment'], 'opening_hours': {'open_now': True, 'weekday_text': []}, 'photos': [{'width': 3840, 'html_attributions': ['<a href="https://maps.google.com/maps/contrib/115176561477727359334/photos">Seen Enkidu</a>'], 'height': 2160, 'photo_reference': 'CmRaAAAAGdlbUGTPdiJ9A3VQClEkbOqpF8R1VOVe4lyouMohOjCN3LYtvvfBU03k8v-TTBK6CI_ZhZiZ1aybZgsSBmPte-bnuTaxYkSFNAXnP8vvoVgqEYlIrZy431KGYju9cpjCEhD4tIuPniDgiXGKmAN4lFadGhRDHP2FgLul0QkeSaL8TKm5y8hrCQ'}], 'vicinity': '200 North Spring Street, Los Angeles'}, {'icon': 'https://maps.gstatic.com/mapfiles/place_api/icons/geocode-71.png', 'vicinity': 'Los Angeles', 'place_id': 'ChIJz-A_k1nHwoARloiyHDKVAm8', 'geometry': {'viewport': {'northeast': {'lng': -118.1913053, 'lat': 34.156512}, 'southwest': {'lng': -118.3978721, 'lat': 34.0127129}}, 'location': {'lng': -118.3228165, 'lat': 34.0686748}}, 'reference': 'CmRbAAAAaBj8zC4jh38gMnjusT5wMaB5ldNfV3yW5NcPtN0uRnhDP8lZ27j4MoAOPCcGEjND7Tj-JeN0ZaVIXYmj6vCp9eZhgWF0_NJX9yBB1HbxNudyU9AHQywIqOeUdDCrjBVoEhBLZN3ZvLv8XXp_WMrqSDR6GhTOywyAF8-JmBB5GqXryvmeM4k78A', 'name': 'Central LA', 'scope': 'GOOGLE', 'types': ['neighborhood', 'political'], 'photos': [{'width': 4160, 'html_attributions': ['<a href="https://maps.google.com/maps/contrib/103618845056792977404/photos">Milca Orozco Torres</a>'], 'height': 3120, 'photo_reference': 'CmRaAAAASUnpzprza_bWoRj05yolwa9QxLm6csgVuc0WMloz2SecqsUyXUrz51VDyd5CuykNaasZjxWeDyDCN5a2Lf1LldO1QW3sqNF5tlZAEe6UpQIJI1ugcanrTaorVb2ZE02rEhC2eQqc4LSQHaEfQ1oliXGdGhQLM2GgicqyxpIilxaH2CaKybCV7g'}], 'id': 'dcc0b3d9033a9588a15062916d310643ec8d56a1'}]

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
    
def test_addToDB_connect():
    #  ================= For Albert  ================= #

    db = pymysql.connect(host= "escality-db-instance.cykpeyjjej2m.us-west-1.rds.amazonaws.com",
                user="escality_user",
                passwd="12345678")

    #  ================= For Melissa  ================= #
    # db = pymysql.connect(host= "localhost",
    #                 user="root",
    #                 passwd="password")

    #  ================================================= #      

    assert(db.open)    

    db.close()

    assert(db.open == 0)            

   #  ================================================= #     
def test_addToDB_add_data():
    bpdb.addToDB(DB_ARRAY)
    #  ================= For Albert  ================= #

    # db = pymysql.connect(host= "escality-db-instance.cykpeyjjej2m.us-west-1.rds.amazonaws.com",
    #             user="escality_user",
    #             passwd="12345678")

    #  ================= For Melissa  ================= #
    db = pymysql.connect(host= "localhost",
                    user="root",
                    passwd="password")

   #  ================================================= #       
    cursor = db.cursor()
    cursor.execute("SET sql_notes = 0; ")       # Suppress warnings   
    cursor.execute('USE escality_location_db')
    cursor.execute('SELECT * from pois')
    
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

    