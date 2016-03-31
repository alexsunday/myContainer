#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <stdio.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>

#include <Python.h>
#include <patchlevel.h>

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
	int result, flags;
	PyObject* temp;

	if(PyArg_ParseTuple(args, "Oi", &temp, &flags)) {
		if(!PyCallable_Check(temp)) {
			PyErr_SetString(PyExc_TypeError, "parameter must be callable.");
			return NULL;
		}

		Py_XINCREF(temp);
		Py_XDECREF(my_callback);
		my_callback = temp;
	}

	result = clone(clone_func, container_stack + STACK_SIZE, flags, NULL);
	if(result == -1) {
		printf("ERRNO: [%d]\n", errno);
	}
	return Py_BuildValue("i", result);
}


static PyMethodDef cloneMethods[] = {
	{"clone", wrap_clone, METH_VARARGS, "clone clone system call."},
	{NULL, NULL}
};

typedef struct {
	char* varname;
	unsigned int varval;
} PyConst;

static PyConst vars[] = {
	{"SIGCHLD", 17},
	{"CSIGNAL",  0x000000ff},
	{"CLONE_VM",  0x00000100},
	{"CLONE_FS",  0x00000200},
	{"CLONE_FILES",  0x00000400},
	{"CLONE_SIGHAND",  0x00000800},
	{"CLONE_PTRACE",  0x00002000},
	{"CLONE_VFORK",  0x00004000},
	{"CLONE_PARENT",  0x00008000},
	{"CLONE_THREAD",  0x00010000},
	{"CLONE_NEWNS",  0x00020000},
	{"CLONE_SYSVSEM",  0x00040000},
	{"CLONE_SETTLS",  0x00080000},
	{"CLONE_PARENT_SETTID",  0x00100000},
	{"CLONE_CHILD_CLEARTID",  0x00200000},
	{"CLONE_DETACHED",  0x00400000},
	{"CLONE_UNTRACED",  0x00800000},
	{"CLONE_CHILD_SETTID",  0x01000000},
	{"CLONE_NEWUTS",  0x04000000},
	{"CLONE_NEWIPC",  0x08000000},
	{"CLONE_NEWUSER",  0x10000000},
	{"CLONE_NEWPID",  0x20000000},
	{"CLONE_NEWNET",  0x40000000},
	{"CLONE_IO",  0x80000000}
};

int add_const(PyObject* m)
{
	int i = 0;
	int arrlen = sizeof(vars) / sizeof(PyConst);
	for(; i != arrlen; ++i) {
		if(PyModule_AddIntConstant(m, vars[i].varname, vars[i].varval)) {
			return -1;
		}
	}

	return 0;
}


#if (PY_MAJOR_VERSION == 3)

static struct PyModuleDef clonemodule = {
	PyModuleDef_HEAD_INIT,
	"clone",
	"HELLO",
	-1,
	cloneMethods
};

PyMODINIT_FUNC PyInit_clone(void)
{
	PyObject* m = PyModule_Create(&clonemodule);
	if(!m) {
		return NULL;
	}
	add_const(m);

	return m;
}

#elif (PY_MAJOR_VERSION == 2)


PyMODINIT_FUNC
initclone(void)
{
	PyObject* m = Py_InitModule("clone", cloneMethods);
	add_const(m);
}

#endif

