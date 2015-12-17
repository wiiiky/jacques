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
#include <sph.h>
#include <stdio.h>
#include <stdlib.h>
#include <Python.h>

typedef struct {
    PyObject *main_module;
} ModuleData;


static int m_init(void);
static void m_finalize(void);
static int m_accept(SphSocket *socket);
static int m_recv(SphSocket *socket,const uint8_t *data, unsigned int len);

static JacModule mod = {
    m_init,
    m_finalize,
    m_accept,
    m_recv
};

JACQUES_MODULE(mod);

static int m_init(void) {
    printf("m_init\n");
    Py_Initialize();

    PyObject* module_name = PyUnicode_FromString("mod.ptest");
    PyObject *main_module = PyImport_Import(module_name);
    Py_DECREF(module_name);

    if(!main_module) {
        PyErr_Print();
        Py_Finalize();
        return -1;
    }
    ModuleData *data=(ModuleData*)malloc(sizeof(ModuleData));
    data->main_module = main_module;

    mod.user_data = data;
    return 0;
}

static void m_finalize(void) {
    printf("m_finalize!\n");
    ModuleData *data=(ModuleData*)mod.user_data;
    Py_DECREF(data->main_module);
    Py_Finalize();
}

static int m_accept(SphSocket *socket) {
    ModuleData *data=(ModuleData*)mod.user_data;
    PyObject* func = PyObject_GetAttrString(data->main_module, (char*)"echo");
    if(func && PyCallable_Check(func)) {
        PyObject *args = PyTuple_New(1);
        PyTuple_SetItem(args, 0, PyUnicode_FromString("accept"));
        PyObject *ret = PyObject_CallObject(func, args);
        if (ret != NULL) {
            printf("Result of call: %ld\n", PyLong_AsLong(ret));
            Py_DECREF(ret);
        } else {
            PyErr_Print();
        }
        Py_DECREF(args);
    } else {
        PyErr_Print();
    }
    Py_DECREF(func);
    return 0;
}

static int m_recv(SphSocket *socket,const uint8_t *data, unsigned int len) {
    sph_socket_prepare_data(socket, data, len);
    return 0;
}
