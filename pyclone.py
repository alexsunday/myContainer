
import os
from ctypes import *
libc = CDLL("libc.so.6")

stack = c_char_p(" " * 1024 * 1024)
stack_top = c_void_p(cast(stack, c_void_p).value + 1024 * 1024)


def clone(func):
	f_c = CFUNCTYPE(c_int)(func)
	return libc.clone(f_c, stack_top, 0x20000000)

