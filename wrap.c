#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <stdio.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>

#include <Python.h>

#define STACK_SIZE (1024*1024)
static char container_stack[STACK_SIZE];
static PyObject* my_callback = NULL;


int clone_func(void* arg)
{
	printf("clone func inside, cb: [%p], pid: [%d]\n", my_callback, getpid());
	PyObject* result = NULL;
	printf("trace1\n");
	result = PyObject_CallObject(my_callback, NULL);
	if(!result) {
		printf("ERROR occured, errno: [%d]\n", errno);
		return 1;
	}
	printf("trace2, result: [%p]\n", result);
	printf("trace3\n");

	return 0;
}


PyObject* wrap_clone(PyObject* self, PyObject* args)
{
	int n, result, flags;
	PyObject* temp;

	if(PyArg_ParseTuple(args, "O:set_callback", &temp)) {
		if(!PyCallable_Check(temp)) {
			PyErr_SetString(PyExc_TypeError, "parameter must be callable.");
			return NULL;
		}

		Py_XINCREF(temp);
		Py_XDECREF(my_callback);
		my_callback = temp;
	}

	result = clone(clone_func, container_stack + STACK_SIZE, CLONE_NEWUTS | CLONE_NEWIPC | CLONE_NEWPID | CLONE_NEWNS | SIGCHLD, NULL);
	if(result == -1) {
		printf("ERRNO: [%d]\n", errno);
	}
	return Py_BuildValue("i", result);
}


static PyMethodDef cloneMethods[] = {
	{"clone", wrap_clone, METH_VARARGS, "clone clone system call."},
	{NULL, NULL}
};

static struct PyModuleDef clonemodule = {
	PyModuleDef_HEAD_INIT,
	"clone",
	"HELLO",
	-1,
	cloneMethods
};

PyMODINIT_FUNC PyInit_clone(void)
{
	return PyModule_Create(&clonemodule);
}

