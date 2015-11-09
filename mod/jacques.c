/*
 * Copyright (C) 2015 Wiky L
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.";
 */
#include <libjac.h>
#include <stdlib.h>
#include <Python.h>
#include "structmember.h"

typedef struct {
    PyObject_HEAD
    
    Socket *socket;
}PySocket;

static void Socket_dealloc(PySocket *self){
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject *Socket_new(PyTypeObject *type, PyObject *args, PyObject *kwds){
    PySocket *self=(PySocket*)type->tp_alloc(type, 0);
    return (PyObject*)self;
}

static int Socket_init(PySocket *self, PyObject *args, PyObject *kwds){
    return 0;
}

static PyMemberDef Socket_members[] = {
    {NULL}
};

static PyObject *Socket_address(PySocket *self){
    return PyUnicode_FromFormat("NO");
}

static PyMethodDef Socket_methods[] = {
    {"address", (PyCFunction)Socket_address, METH_NOARGS,
     "return the address of socket"},
    {NULL}
};

static PyTypeObject SocketType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "jacques.Socket",             /* tp_name */
    sizeof(PySocket), /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)Socket_dealloc,/* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE,        /* tp_flags */
    "Socket objects",           /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    Socket_methods,             /* tp_methods */
    Socket_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)Socket_init,      /* tp_init */
    0,                         /* tp_alloc */
    Socket_new,                 /* tp_new */
};

static PyModuleDef jacquesmodule= {
    PyModuleDef_HEAD_INIT,
    "jacques",
    "module that extends libjac",
    -1,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC PyInit_jacques(void){
    PyObject *m;
    if(PyType_Ready(&SocketType)<0){
        return NULL;
    }
    if((m = PyModule_Create(&jacquesmodule))==NULL){
        return NULL;
    }
    
    Py_INCREF(&SocketType);
    PyModule_AddObject(m, "Socket", (PyObject*)&SocketType);
    return m;
}
