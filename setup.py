from distutils.core import setup, Extension

elfhelper = Extension('elftoy.elfhelper', sources = ['elfhelper/elfhelper.c'])

setup (name = 'elftoy',
       version = '1.0',
       packages = ['elftoy'],
       description = 'Toy framework for elf analysis',
       ext_modules = [elfhelper])

