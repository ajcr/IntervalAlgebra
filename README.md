# IntervalType

A C extension module for Python implementing a basic Interval type and the key relations of [Allen's Interval Algebra](https://en.wikipedia.org/wiki/Allen%27s_interval_algebra).

This was primarily a quick toy project for me to familiarise myself with some of the intracacies of the CPython API. It may be a useful starting point for something more ambitious and serves as a usable type in the meantime.

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
>>> 3 in a # the interval is closed
True
>>> 0 in a
False
```

The `left` and `right` attributes can theoretically be any Python objects at all, but must satisfy `left < right`. You can also add metadata (any Python object you like) to an interval when it is created. The `.span()` method shows the length of the interval:

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

Intervals are compared by looking at the `left` and `right` attributes internally. One interval is strictly greater than another if ends before the other begins:

``` python
>>> b = interval(6, 12)
>>> a == b # equivalent to a.equals(b)
False
>>> a < b  # equivalent to a.before(b)
False
>>> a > b  # equivalent to a.after(b)
False
```

Note the `<=` and `>=` are not supported as they are ambiguous in the context of Allen's Interval Algebra.

We can also test if there is any overlap between two intervals (i.e. they contain common points - not just an endpoint). Two intervals are said to meet if the `left` of one equals the `right` of the other:

``` python
>>> c = interval(0, 3)
>>> a.overlaps(b)
True
>>> a.meets(c) # they meet at 3
True
>>> b.overlaps(c)
False
```

An interval may occur during another, or it may start or finish another interval:

``` python
>>> x = interval(0, 5)
>>> interval(1, 2).during(x)
True
>>> interval(0, 2).starts(x)
True
>>> interval(1, 5).finishes(x)
True
>>> interval(4, 7).during(x)
False
```

## Future work

Given more time and fewer distractions, I'll add more of Python's object protocol for the class (hashing, pickling, alternative constructors). Allen's [original paper](http://cse.unl.edu/~choueiry/Documents/Allen-CACM1983.pdf) contains an algorithm for reasoning on temporal intervals that may be insightful to implement.
