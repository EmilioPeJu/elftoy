import os
import elftoy.elf

from collections import namedtuple

MapEntry = namedtuple('MapEntry', ['name', 'address', 'mode'])

def get_maps(pid):
    entries = []
    for line in open(os.path.join('/proc', str(pid), 'maps'), 'rb'):
        current = {}
        parts = line.split()
        address_part = parts[0].split(b'-')
        address_range = tuple(int(item, 16) for item in address_part)
        mode_part = parts[1]
        mode = 0
        if b'r' in mode_part:
            mode |= 4
        if b'w' in mode_part:
            mode |= 2
        if b'x' in mode_part:
            mode |= 1
        name = parts[-1]
        entries.append(MapEntry(name, address_range, mode))
    return entries
