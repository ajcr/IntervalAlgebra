import pytest

from interval import interval


@pytest.mark.parametrize('x,y,expected', [
    ((1, 2), (3, 4), True),
    ((1, 2), (2, 4), False),
    ((1, 3), (2, 4), False),
    ((3, 4), (1, 2), False),
])
def test_before(x, y, expected):
    a = interval(*x)
    b = interval(*y)
    assert a.before(b) == expected

@pytest.mark.parametrize('x,y,expected', [
    ((1, 2), (3, 4), False),
    ((1, 2), (2, 4), False),
    ((1, 3), (2, 4), False),
    ((3, 4), (1, 2), True),
])
def test_after(x, y, expected):
    a = interval(*x)
    b = interval(*y)
    assert a.after(b) == expected

@pytest.mark.parametrize('x,y', [
    (( 1, 2),  (1, 2)),
    ((-1, 1),  (1, 2)),
    ((-1, 7),  (0, 5)),
    ((1.3, 6), (0, 10)),
    ((1.3, 6), (5, 10)),
])
def test_overlaps(x, y):
    a = interval(*x)
    b = interval(*y)
    assert a.overlaps(b)

@pytest.mark.parametrize('x,y', [
    ((1, 2),  (3, 4)),
    ((4, 5),  (0, 3)),
])
def test_not_overlaps(x, y):
    a = interval(*x)
    b = interval(*y)
    assert not a.overlaps(b)

@pytest.mark.parametrize('x,y,expected', [
    ((1, 2), (3, 4), False),
    ((1, 2), (1, 4), True),
    ((1, 2), (1, 2), False),
    ((1, 4), (1, 2), False),
])
def test_starts(x, y, expected):
    a = interval(*x)
    b = interval(*y)
    assert a.starts(b) == expected

@pytest.mark.parametrize('x,y,expected', [
    ((1, 4), (3, 4), False),
    ((2, 4), (1, 4), True),
    ((1, 2), (1, 2), False),
    ((1, 4), (1, 2), False),
])
def test_finishes(x, y, expected):
    a = interval(*x)
    b = interval(*y)
    assert a.finishes(b) == expected

@pytest.mark.parametrize('x,y,expected', [
    ((1, 4), (3, 4), False),
    ((2, 3), (1, 4), True),
    ((1, 2), (1, 2), False),
    ((1, 4), (2, 3), False),
])
def test_during(x, y, expected):
    a = interval(*x)
    b = interval(*y)
    assert a.during(b) == expected

@pytest.mark.parametrize('x,y,expected', [
    ((1, 4), (3, 4), False),
    ((2, 3), (1, 4), False),
    ((1, 2), (1, 2), True),
])
def test_equals(x, y, expected):
    a = interval(*x)
    b = interval(*y)
    assert a.equals(b) == expected

@pytest.mark.parametrize('x,y,expected', [
    ((1, 4), (3, 4), False),
    ((2, 3), (1, 2), True),
    ((1, 2), (2, 3), True),
    ((1, 4), (2, 3), False),
])
def test_meets(x, y, expected):
    a = interval(*x)
    b = interval(*y)
    assert a.meets(b) == expected
