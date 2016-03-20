from distutils.core import setup, Extension

module1 = Extension('clone', sources=['wrap.c'])

setup(name='clone',version='0.1', description='clone clone call', ext_modules=[module1])
