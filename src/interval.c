#include <Python.h>
#include "structmember.h"

static PyTypeObject IntervalType;

typedef struct {
    PyObject_HEAD
    PyObject *left;
    PyObject *right;
    PyObject *data;
} Interval;

PyObject *
interval_new(PyTypeObject *type, PyObject *args)
{
    PyObject *left = NULL, *right = NULL, *data = NULL;
    Interval *self;
    int cmp_result;

    if (!PyArg_ParseTuple(args, "OO|O:interval", &left, &right, &data))
        return NULL;

    cmp_result = PyObject_RichCompareBool(left, right, Py_LT);

    if (cmp_result == 0) {
        PyErr_SetString(PyExc_ValueError,
                "left must be strictly less than right");
        return NULL;
    }
    else if (cmp_result < 0) {
        PyErr_SetString(PyExc_TypeError,
                "left must be comparable to right");
        return NULL;
    }

    self = (Interval *)type->tp_alloc(type, 0);
    if (self == NULL)
        return NULL;

    self->left = left;
    Py_INCREF(left);

    self->right = right;
    Py_INCREF(right);

    if (data != NULL) {
        self->data = data;
        Py_INCREF(data);
    } else {
        self->data = Py_None;
        Py_INCREF(Py_None);
    }

    return (PyObject *)self;
}

static int
interval_traverse(Interval *self, visitproc visit, void *arg)
{
    Py_VISIT(self->left);
    Py_VISIT(self->right);
    Py_VISIT(self->data);
    return 0;
}

static int
interval_clear(Interval *self)
{
    Py_CLEAR(self->left);
    Py_CLEAR(self->right);
    Py_CLEAR(self->data);
    return 0;
}

static void
interval_dealloc(Interval *self)
{
    interval_clear(self);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject *
interval_repr(Interval *v)
{
    return PyUnicode_FromFormat("interval(%R, %R)", v->left, v->right);
}

/* Allen's Interval Algebra methods:
 *
 * before
 * after
 * overlaps
 * starts
 * finishes
 * during
 * equals
 * meets
 *
 * TODO: eliminate repitition from the code below
 */

static PyObject *
interval_before(Interval *v, PyObject *w)
{
    Interval *wi;
    int cmp;

    if (!PyObject_TypeCheck(w, &IntervalType)) {
        PyErr_SetString(PyExc_TypeError,
                  "Object must be of Interval type");
        return NULL;
    }
    wi = (Interval *)w;
    cmp = PyObject_RichCompareBool(v->right, wi->left, Py_LT);

    if (cmp < 0)
        return NULL;
    else if (cmp == 1)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

static PyObject *
interval_after(Interval *v, PyObject *w)
{
    Interval *wi;
    int cmp;

    if (!PyObject_TypeCheck(w, &IntervalType)) {
        PyErr_SetString(PyExc_TypeError,
                  "Object must be of Interval type");
        return NULL;
    }
    wi = (Interval *)w;
    cmp = PyObject_RichCompareBool(v->left, wi->right, Py_GT);

    if (cmp < 0)
        return NULL;
    else if (cmp == 1)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

static PyObject *
interval_overlaps(Interval *u, PyObject *v)
{
    Interval *vi;
    int result, cmp;

    if (!PyObject_TypeCheck(v, &IntervalType)) {
        PyErr_SetString(PyExc_TypeError,
                  "Object must be of Interval type");
        return NULL;
    }
    vi = (Interval *)v;
    cmp = PyObject_RichCompareBool(u->left, vi->left, Py_LE);

    if (cmp < 0)
        return NULL;

    if (cmp == 1)
        result = PyObject_RichCompareBool(u->right, vi->left, Py_GE);
    else
        result = PyObject_RichCompareBool(u->left, vi->right, Py_LT);

    if (result == 1)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

static PyObject *
interval_equals(Interval *v, PyObject *w)
{
    Interval *wi;
    int cmp_left, cmp_right;

    if (!PyObject_TypeCheck(w, &IntervalType)) {
        PyErr_SetString(PyExc_TypeError,
                  "Object must be of Interval type");
        return NULL;
    }
    wi = (Interval *)w;
    cmp_left = PyObject_RichCompareBool(v->left, wi->left, Py_EQ);
    cmp_right = PyObject_RichCompareBool(v->right, wi->right, Py_EQ);

    if (cmp_left < 0 || cmp_right < 0)
        return NULL;

    if (cmp_left == 1 && cmp_right == 1)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

static PyObject *
interval_during(Interval *u, PyObject *v)
{
    /* determine whether u is completely contained in v */
    int cmp_left, cmp_right;
    Interval *vi;

    if (!PyObject_TypeCheck(v, &IntervalType)) {
        PyErr_SetString(PyExc_TypeError,
                  "Object must be of Interval type");
        return NULL;
    }
    vi = (Interval *)v;
    cmp_left = PyObject_RichCompareBool(u->left, vi->left, Py_GT);
    cmp_right = PyObject_RichCompareBool(u->right, vi->right, Py_LT);

    if (cmp_left < 0 || cmp_right < 0)
        return NULL;

    if (cmp_left == 1 && cmp_right == 1)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

static PyObject *
interval_starts(Interval *u, PyObject *v)
{
    int cmp_left, cmp_right;
    Interval *vi;

    if (!PyObject_TypeCheck(v, &IntervalType)) {
        PyErr_SetString(PyExc_TypeError,
                  "Object must be of Interval type");
        return NULL;
    }
    vi = (Interval *)v;
    cmp_left = PyObject_RichCompareBool(u->left, vi->left, Py_EQ);
    cmp_right = PyObject_RichCompareBool(u->right, vi->right, Py_LT);

    if (cmp_left < 0 || cmp_right < 0)
        return NULL;

    if (cmp_left == 1 && cmp_right == 1)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

static PyObject *
interval_finishes(Interval *u, PyObject *v)
{
    int cmp_left, cmp_right;
    Interval *vi;

    if (!PyObject_TypeCheck(v, &IntervalType)) {
        PyErr_SetString(PyExc_TypeError,
                  "Object must be of Interval type");
        return NULL;
    }
    vi = (Interval *)v;
    cmp_left = PyObject_RichCompareBool(u->left, vi->left, Py_GT);
    cmp_right = PyObject_RichCompareBool(u->right, vi->right, Py_EQ);

    if (cmp_left < 0 || cmp_right < 0)
        return NULL;

    if (cmp_left == 1 && cmp_right == 1)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

static PyObject *
interval_meets(Interval *u, PyObject *v)
{
    int cmp_left, cmp_right;
    Interval *vi;

    if (!PyObject_TypeCheck(v, &IntervalType)) {
        PyErr_SetString(PyExc_TypeError,
                  "Object must be of Interval type");
        return NULL;
    }
    vi = (Interval *)v;
    cmp_left = PyObject_RichCompareBool(u->right, vi->left, Py_EQ);
    cmp_right = PyObject_RichCompareBool(u->left, vi->right, Py_EQ);

    if (cmp_left < 0 || cmp_right < 0)
        return NULL;

    if (cmp_left == 1 || cmp_right == 1)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

/* Miscellaneous methods:
 *
 * span
 * contains
 * distancetopoint
 */

static PyObject *
interval_span(Interval *v)
{
    PyObject *result;

    result = PyNumber_Subtract(v->right, v->left);
    return result;
}

static int
interval_contains(Interval *op, PyObject *v)
{
    int cmp_left, cmp_right;

    cmp_left = PyObject_RichCompareBool(v, op->left, Py_GE);
    cmp_right = PyObject_RichCompareBool(v, op->right, Py_LE);

    if (cmp_left < 0 || cmp_right < 0)
        return -1;

    if (cmp_left == 1 && cmp_right == 1)
        return 1;
    else
        return 0;
}

static PyObject *
interval_containsdirect(Interval *op, PyObject *v)
{
    int result = interval_contains(op, v);

    if (result < 0)
        return NULL;

    if (result == 1)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

static PyObject *
interval_distancetopoint(Interval *u, PyObject *v)
{
    PyObject *result;
    int cmp;

    cmp = interval_contains(u, v);
    if (cmp < 0)
        return NULL;

    if (cmp == 1) {
        /* It would be useful to return "zero" in whatever
         * type is compatible with the left and right
         * attributes. For instance, timedelta(0) if it
         * is a date interval. This may be quite difficult
         * to do though.
         */
        result = PyLong_FromLong(0L);
        Py_INCREF(result);
        return result;
    }

    /* We don't need to check whether the comparison
     * raises an error as we've already checked this
     * using interval_contains.
     */
    cmp = PyObject_RichCompareBool(u->left, v, Py_GT);
    if (cmp == 1)
        result = PyNumber_Subtract(v, u->left);
    else
        result = PyNumber_Subtract(v, u->right);

    if (result == NULL)
        return NULL;

    Py_INCREF(result);
    return result;
}

/* Rich comparison */

static PyObject *
interval_richcompare(Interval *v, PyObject *w, int op)
{
    PyObject *r1;
    int r3;

    if(!PyObject_TypeCheck(w, &IntervalType))
        Py_RETURN_NOTIMPLEMENTED;

    switch (op) {
        case Py_EQ:
            return interval_equals(v, w);

        case Py_NE:
            r1 = interval_richcompare(v, w, Py_EQ);
            if (r1 == NULL)
                return NULL;
            r3 = PyObject_IsTrue(r1);
            Py_DECREF(r1);
            if (r3 < 0)
                return NULL;
            return PyBool_FromLong(!r3);

        case Py_LE:
            Py_RETURN_NOTIMPLEMENTED;

        case Py_GE:
            Py_RETURN_NOTIMPLEMENTED;

        case Py_LT:
            return interval_before(v, w);

        case Py_GT:
            return interval_after(v, w);
    }
    Py_RETURN_NOTIMPLEMENTED;
}

static PySequenceMethods interval_as_sequence = {
    0,                                  /* sq_length */
    0,                                  /* sq_concat */
    0,                                  /* sq_repeat */
    0,                                  /* sq_item */
    0,                                  /* sq_slice */
    0,                                  /* sq_ass_item */
    0,                                  /* sq_ass_slice */
    (objobjproc)interval_contains,      /* sq_contains */
};

static PyMemberDef interval_members[] = {
    {"left",  T_OBJECT_EX, offsetof(Interval, left),  READONLY, "left bound"},
    {"right", T_OBJECT_EX, offsetof(Interval, right), READONLY, "right bound"},
    {"data",  T_OBJECT_EX, offsetof(Interval, data),  READONLY, "interval metadata"},
    {NULL}  /* Sentinel */
};

static PyMethodDef
interval_methods[] = {
    /* Allen's Interval Algebra methods */
    {"before",         (PyCFunction)interval_before,             METH_O,
     "Return whether the interval occurs before the other interval begins"},
    {"after",          (PyCFunction)interval_after,              METH_O,
     "Return whether the interval occurs after the other interval ends"},
    {"overlaps",       (PyCFunction)interval_overlaps,           METH_O,
     "Return whether the interval overlaps the other interval"},
    {"during",         (PyCFunction)interval_during,             METH_O,
     "Return whether the interval is properly contained within the other interval"},
    {"equals",         (PyCFunction)interval_equals,             METH_O,
     "Return whether the interval is equal to the other interval"},
    {"starts",         (PyCFunction)interval_starts,             METH_O,
     "Return whether the interval starts the other interval (finishing before its end)"},
    {"finishes",       (PyCFunction)interval_finishes,           METH_O,
     "Return whether the interval finishes the other interval (starting after its start)"},
    {"meets",          (PyCFunction)interval_meets,              METH_O,
     "Return whether the interval's end matches the other's start, or vice versa"},
    /* Other methods */
    {"distance",       (PyCFunction)interval_distancetopoint,    METH_O,
     "Return the interval's distance to another object"},
    {"span",           (PyCFunction)interval_span,               METH_NOARGS,
     "Return the distance spanned by the interval"},
    {"contains",       (PyCFunction)interval_containsdirect,     METH_O,
     "Return whether the object lies within the interval"},
    {"__contains__",   (PyCFunction)interval_contains,           METH_O | METH_COEXIST,
     "Return whether the object lies within the interval"},
    {NULL, NULL}  /* sentinel */
};

static PyTypeObject IntervalType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "interval.interval",               /* tp_name */
    sizeof(Interval),                  /* tp_basicsize */
    0,                                 /* tp_itemsize */
    (destructor)interval_dealloc,      /* tp_dealloc */
    0,                                 /* tp_print */
    0,                                 /* tp_getattr */
    0,                                 /* tp_setattr */
    0,                                 /* tp_reserved */
    (reprfunc)interval_repr,           /* tp_repr */
    0,                                 /* tp_as_number */
    &interval_as_sequence,             /* tp_as_sequence */
    0,                                 /* tp_as_mapping */
    0,                                 /* tp_hash  */
    0,                                 /* tp_call */
    0,                                 /* tp_str */
    0,                                 /* tp_getattro */
    0,                                 /* tp_setattro */
    0,                                 /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE |
            Py_TPFLAGS_HAVE_GC,        /* tp_flags */
    "Interval object",                 /* tp_doc */
    (traverseproc)interval_traverse,   /* tp_traverse */
    (inquiry)interval_clear,           /* tp_clear */
    (richcmpfunc)interval_richcompare, /* tp_richcompare */
    0,                                 /* tp_weaklistoffset */
    0,                                 /* tp_iter */
    0,                                 /* tp_iternext */
    interval_methods,                  /* tp_methods */
    interval_members,                  /* tp_members */
    0,                                 /* tp_getset */
    0,                                 /* tp_base */
    0,                                 /* tp_dict */
    0,                                 /* tp_descr_get */
    0,                                 /* tp_descr_set */
    0,                                 /* tp_dictoffset */
    0,                                 /* tp_init */
    0,                                 /* tp_alloc */
    (newfunc)interval_new,             /* tp_new */
};

static PyModuleDef Intervalmodule = {
    PyModuleDef_HEAD_INIT,
    "interval",
    "A simple Interval type for Python.",
    -1,
    NULL, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC
PyInit_interval(void)
{
    PyObject* m;

    if (PyType_Ready(&IntervalType) < 0)
        return NULL;

    m = PyModule_Create(&Intervalmodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&IntervalType);
    PyModule_AddObject(m, "interval", (PyObject *)&IntervalType);
    return m;
}
