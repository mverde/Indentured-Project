import build_poi_database as bpdb
import pytest

LAT_LONG_ERROR_MARGIN = 0.0075

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
    