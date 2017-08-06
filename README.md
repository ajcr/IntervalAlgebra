# IntervalType

A simple C extension module for Python implementing a basic Interval type.

This was primarily a quick toy project to better familiarise myself with the intracacies of the CPython API. The set of features is small, but it may be a useful starting point for something more ambitious and serves as a usable type in the meantime.

## Installation

Clone the repository and then `python setup.py install` should work. Run `pytest` at the command line to run the tests. I've only tried the module with Python 3.6 on Linux. Other versions of Python 3 on other operating systems should work fine, but Python 2 probably won't be compatible.

## Examples

An interval captures a (possibly infinite) collection of points between a *left* bound and a *right* bound. For instance `a` has a left bound of 3 and a right bound of 7. Intervals are closed, meaning that the bounds are included in the set of points belonging to the interval:

``` python
>>> from interval import interval
>>> a = interval(3, 7)
>>> a
interval(3, 7)
>>> 5.89 in a
True
>>> 3 in a
True
>>> 0 in a
False
```

The `left` and `right` attributes can be any objects at all, but must satisfy `left < right`. You can also add metadata to an interval. The `.span()` method shows the length of the interval:

``` python
>>> a.span()
4
>>> from datetime import datetime
>>> feb2000 = interval(datetime(2000, 2, 1), datetime, 2, 29), 'leap month!')
>>> feb2000.data
'Leap month!'
>>> feb2000.span()
timedelta(28)
```

Intervals are compared by looking at the `left` and `right` attributes internally. One interval is greater than another if its span completely covers that interval (the `<` here is analogous to the builtin `set` type where it means 'subset of'):

``` python
>>> b = interval(4, 6)
>>> a == b
False
>>> a < b
False
>>> a > b
True
```

We can also test if there is any overlap between intervals (i.e. they contain any common points):

``` python
>>> c = interval(0, 3)
>>> a.overlaps(b)
True
>>> a.overlaps(c)
True
>>> b.overlaps(c)
False
```

## Future work

Given more time and fewer distractions, I'll implement more of Python's object protocol for the class (hashing, pickling, alternative constructors). There should also be options for intervals to be open or half-open. A good goal would be to recreate all of functionality for the interval type from the excellent [*intervaltree*](https://github.com/chaimleib/intervaltree) project by chaimleib.


