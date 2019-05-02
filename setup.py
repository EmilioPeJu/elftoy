import glob
from distutils.core import setup, Extension

elf = Extension('elftoy.elf', sources=['elf/elfutils.c',
                                       'elf/elfinjections.c',
                                       'elf/elfhijacking.c',
                                       'elf/elfpatching.c',
                                       'elf/elffunctions.c',
                                       'elf/elfimport.c'])
debug = Extension('elftoy.debug', sources=glob.glob('debug/*.c'))

setup (name = 'elftoy',
       version = '1.0',
       packages = ['elftoy'],
       description = 'Tools for learning about elf',
       ext_modules = [elf, debug])

