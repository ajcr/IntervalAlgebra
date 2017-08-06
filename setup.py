from distutils.core import setup, Extension

interval_module = Extension('interval',
                            sources = ['src/interval.c'])

setup(name = 'interval',
      version = '0.1',
      description = 'A simple interval type for Python.',
      ext_modules = [interval_module])

