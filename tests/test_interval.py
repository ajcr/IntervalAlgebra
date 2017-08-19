from datetime import datetime as dt
from datetime import timedelta

import pytest

from interval import interval

# test construction and attribute access/setting

def test_data_attribute_access():
    x = interval(0, 5)
    assert x.data is None
    y = interval(0, 5, 'some data')
    assert y.data == 'some data'

@pytest.mark.parametrize('left,right', [
    (  8, 6),
    (  0, 0),
    (3.1, 0),
])
def test_right_le_left_raises(left, right):
    with pytest.raises(ValueError):
        interval(left, right)

@pytest.mark.parametrize('left,right', [
    (     8,    'qqq'),
    ( 'qqq',   3 - 7j),
    (3.1, ['cat', 77]),
])
def test_incomparable_left_to_right_raises(left, right):
    with pytest.raises(TypeError):
        interval(left, right)

@pytest.mark.parametrize('left,right,data', [
    (3, 6, 'data'),
    (3, 6, 'more data'),
    (dt(2000, 1, 1), dt(2000, 6, 30), 'six months'),
])
def test_access_attributes(left, right, data):
    x = interval(left, right, data)
    assert x.left == left and x.right == right and x.data == data

@pytest.mark.parametrize('left,right,data', [
    ( 1, 6, None),
    (-1, 0, 'ab'),
])
def test_readonly_attr(left, right, data):
    x = interval(1, 5, 'some data')
    with pytest.raises(AttributeError):
        x.left = 33
        x.right = 0
        x.data = 'should fail'


# test __contains__

@pytest.mark.parametrize('left,right,thing', [
    ( 3,   6,   3),
    ( 3,   6,   6),
    ( 3, 6.6, 4.7),
    (-1, 5.8,   0),
    ( dt(2000, 1, 1), dt(2000, 6, 1), dt(2000, 2, 29)),
])
def test_contains(left, right, thing):
    x = interval(left, right)
    assert thing in x

@pytest.mark.parametrize('left,right,thing', [
    ( 3,   6,   -1),
    ( 3,   6,   77),
    ( 3, 6.6, 10.7),
    (-1, 5.8,    6),
    ( dt(2000, 1, 1), dt(2000, 6, 1), dt(2000, 7, 2)),
])
def test_not_contains(left, right, thing):
    x = interval(left, right)
    assert thing not in x


# test span

@pytest.mark.parametrize('left,right,span', [
    ( 3,   6,   3),
    (-1, 5.8, 6.8),
    ( dt(2000, 1, 1), dt(2000, 6, 1), timedelta(152)),
])
def test_span(left, right, span):
    x = interval(left, right)
    assert x.span() == span

# test richcompare

@pytest.mark.parametrize('l1,r1,l2,r2', [
    ( 1, 5,  1, 5),
    (-2, 5, -2, 5),
])
def test_eq(l1, r1, l2, r2):
    a = interval(l1, r1)
    b = interval(l2, r2)
    assert a == b

@pytest.mark.parametrize('l1,r1,l2,r2', [
    ( 1, 5,  1, 2),
    (-2, 5, -3, 5),
])
def test_ne(l1, r1, l2, r2):
    a = interval(l1, r1)
    b = interval(l2, r2)
    assert a != b

@pytest.mark.parametrize('l1,r1,l2,r2', [
    ( 1, 2,  1, 5),
    ( 1, 2,  0, 5),
    (-1, 7, -3.3, 10),
    ( dt(2000, 1, 1), dt(2000, 6, 1), dt(1999, 3, 2), dt(2007, 3, 7)),
])
def test_lt(l1, r1, l2, r2):
    a = interval(l1, r1)
    b = interval(l2, r2)
    assert a < b

@pytest.mark.parametrize('l1,r1,l2,r2', [
    ( 1, 5,  2, 5),
    ( 0, 7,  0, 5),
    (-1, 7,  1, 7),
    ( dt(2000, 1, 1), dt(2000, 6, 1), dt(2000, 3, 2), dt(2000, 3, 7)),
])
def test_gt(l1, r1, l2, r2):
    a = interval(l1, r1)
    b = interval(l2, r2)
    assert a > b

# note: not lt means that a is not a proper subinterval of b
@pytest.mark.parametrize('l1,r1,l2,r2', [
    ( 1, 2,  1, 2),
    (-1, 3,  0, 5),
    (1.3, 6, 5, 10),
    (1.3, 6, 7, 10),
])
def test_not_lt(l1, r1, l2, r2):
    a = interval(l1, r1)
    b = interval(l2, r2)
    assert not a < b

@pytest.mark.parametrize('l1,r1,l2,r2', [
    ( 0, 2,  1, 3),
    (-1, 3,  0, 5),
    (0.9, 3,  3, 5),
])
def test_not_gt(l1, r1, l2, r2):
    a = interval(l1, r1)
    b = interval(l2, r2)
    assert not a > b


# test overlap

@pytest.mark.parametrize('l1,r1,l2,r2', [
    ( 1, 2,  1, 2),
    (-1, 1,  1, 2),
    (-1, 7,  0, 5),
    (1.3, 6, 0, 10),
    (1.3, 6, 5, 10),
])
def test_overlaps(l1, r1, l2, r2):
    a = interval(l1, r1)
    b = interval(l2, r2)
    assert a.overlaps(b)

@pytest.mark.parametrize('l1,r1,l2,r2', [
    ( 1, 2,  3, 4),
    ( 4, 5,  0, 3),
])
def test_not_overlaps(l1, r1, l2, r2):
    a = interval(l1, r1)
    b = interval(l2, r2)
    assert not a.overlaps(b)

# test distance

@pytest.mark.parametrize('left,right,obj,distance', [
    ( 1, 2,  1, 0),
    (-1, 1,  -1, 0),
    ( 1, 2,  3, 1),
    ( 4, 5,  0, -4),
])
def test_not_overlaps(left, right, obj, distance):
    a = interval(left, right)
    assert a.distance(obj) == distance

