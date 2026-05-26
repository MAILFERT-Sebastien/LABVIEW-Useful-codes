#!/usr/bin/python
# -*- coding: utf-8 -*-
"""This example shows how to move all connected axes of an E-872 open loop."""

from pipython import GCSDevice
from pipython.pitools import waitonwalk


def main():
    """Connect E-872 and move all connected axes open loop.
    Remember you can move only one axis at a time.
    """
    with GCSDevice('E-872') as pidevice:
        pidevice.InterfaceSetupDlg(key='sample')
        print('connected: {}'.format(pidevice.qIDN().strip()))
        for axis in pidevice.axes:
            print('move axis {} forward...'.format(axis))
            pidevice.OSM(axis, 1000)
            waitonwalk(pidevice, axis)
            print('move axis {} backward...'.format(axis))
            pidevice.OSM(axis, -1000)
            waitonwalk(pidevice, axis)
    print('finished')


if __name__ == '__main__':
    # import logging
    # logging.basicConfig(level=logging.DEBUG)
    main()
