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
    